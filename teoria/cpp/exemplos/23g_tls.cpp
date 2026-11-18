// Aula 23g C++ — TLS com autenticacao por token sobre a base da 23d.
//
// Servidor TLS de verdade (OpenSSL) em 127.0.0.1 com certificado
// autofirmado EFERO gerado em memoria: nada e lido de disco e nada
// e confiavel fora deste processo. O cliente fixa a impressao digital
// (SHA-256) recebida pelo pipe — pinagem de primeiro contato (TOFU):
// em producao, essa etapa seria uma CA. Dentro do tunel, o servidor
// exige "TOKEN <segredo>": TLS sem autenticacao e so metade do contrato.
// Token errado recebe NEGADO; o caso sem token morre no parse.
//
// Compilar: g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23g_tls.cpp -o /tmp/23g_tls -lssl -lcrypto
// Executar: /tmp/23g_tls
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace {

constexpr char SEGREDO[] = "s3nha-da-23c";
constexpr int VOLTAS = 2; // uma com token certo, uma com errado

// Gera chave RSA + certificado X509 autofirmado em memoria (10 min) e
// devolve o contexto com o pin SHA-256. Erro aqui e defeito do
// experimento: aborta com mensagem do OpenSSL.
struct ServidorPronto {
    SSL_CTX* ctx;
    std::string pin;
};

std::string digest_hex(X509* cert) {
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int n = 0;
    assert(X509_digest(cert, EVP_sha256(), md, &n) == 1);
    char hex[2 * EVP_MAX_MD_SIZE + 1];
    for (unsigned int i = 0; i < n; i++) sprintf(hex + 2 * i, "%02x", md[i]);
    return std::string(hex, 2 * n);
}

ServidorPronto contexto_servidor() {
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    assert(ctx != nullptr);
    EVP_PKEY* chave = EVP_RSA_gen(2048);
    assert(chave != nullptr);
    X509* cert = X509_new();
    assert(cert != nullptr);
    X509_set_version(cert, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
    X509_gmtime_adj(X509_get_notBefore(cert), 0);
    X509_gmtime_adj(X509_get_notAfter(cert), 600);
    X509_NAME* nome = X509_get_subject_name(cert);
    X509_NAME_add_entry_by_txt(nome, "CN", MBSTRING_ASC,
                               (const unsigned char*)"localhost-23g", -1, -1, 0);
    X509_set_issuer_name(cert, nome);
    X509_set_pubkey(cert, chave);
    assert(X509_sign(cert, chave, EVP_sha256()) > 0);
    assert(SSL_CTX_use_certificate(ctx, cert) == 1);
    assert(SSL_CTX_use_PrivateKey(ctx, chave) == 1);
    std::string pin = digest_hex(cert);
    X509_free(cert);
    EVP_PKEY_free(chave);
    return {ctx, pin};
}

std::string impressao(SSL* s) {
    X509* cert = SSL_get_peer_certificate(s);
    assert(cert != nullptr);
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int n = 0;
    assert(X509_digest(cert, EVP_sha256(), md, &n) == 1);
    X509_free(cert);
    char hex[2 * EVP_MAX_MD_SIZE + 1];
    for (unsigned int i = 0; i < n; i++) sprintf(hex + 2 * i, "%02x", md[i]);
    return std::string(hex, 2 * n);
}

// Uma sessao: le linha, cobra o token, responde OK ou NEGADO.
void atender(SSL* s) {
    std::string linha;
    char c = 0;
    while (linha.size() < 256) {
        int r = SSL_read(s, &c, 1);
        if (r <= 0 || c == '\n') break;
        if (c != '\r') linha += c;
    }
    std::string resposta = (linha == std::string("TOKEN ") + SEGREDO) ? "OK\n" : "NEGADO\n";
    assert(SSL_write(s, resposta.c_str(), (int)resposta.size()) == (int)resposta.size());
}

int tcp_ouvinte() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    int um = 1;
    assert(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &um, sizeof(um)) == 0);
    sockaddr_in end{};
    end.sin_family = AF_INET;
    end.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    end.sin_port = 0; // efemera: sem disputa de porta na maquina
    assert(bind(fd, (sockaddr*)&end, sizeof(end)) == 0);
    assert(listen(fd, 4) == 0);
    return fd;
}

int porta_de(int fd) {
    sockaddr_in end{};
    socklen_t n = sizeof(end);
    assert(getsockname(fd, (sockaddr*)&end, &n) == 0);
    return ntohs(end.sin_port);
}

void servidor(int pronto_fd, int porta_fd) {
    ServidorPronto pronto = contexto_servidor();
    // Publica o pin TOFU pelo pipe ANTES do accept: o pai bloqueia nele
    // e so conecta depois de conhece-lo (sem isso, deadlock).
    assert(write(pronto_fd, pronto.pin.c_str(), pronto.pin.size()) == (ssize_t)pronto.pin.size());
    for (int i = 0; i < VOLTAS; i++) {
        int cli = accept(porta_fd, nullptr, nullptr);
        assert(cli >= 0);
        SSL* s = SSL_new(pronto.ctx);
        SSL_set_fd(s, cli);
        assert(SSL_accept(s) == 1);
        atender(s);
        SSL_shutdown(s);
        SSL_free(s);
        close(cli);
    }
    SSL_CTX_free(pronto.ctx);
}

void cliente(int porta, const std::string& pin, const std::string& token, bool espera_ok) {
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    assert(ctx != nullptr);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr); // sem CA: o pin decide
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    sockaddr_in end{};
    end.sin_family = AF_INET;
    end.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    end.sin_port = htons((uint16_t)porta);
    assert(connect(fd, (sockaddr*)&end, sizeof(end)) == 0);
    SSL* s = SSL_new(ctx);
    SSL_set_fd(s, fd);
    assert(SSL_connect(s) == 1);
    // Pinagem: so fala com quem tem a impressao do primeiro contato.
    assert(impressao(s) == pin);
    std::string pedido = "TOKEN " + token + "\n";
    assert(SSL_write(s, pedido.c_str(), (int)pedido.size()) == (int)pedido.size());
    char resp[16] = {0};
    int r = SSL_read(s, resp, (int)sizeof(resp) - 1);
    assert(r > 0);
    bool ok = std::string(resp) == "OK\n";
    std::cout << "token '" << token << "': " << resp;
    assert(ok == espera_ok);
    SSL_shutdown(s);
    SSL_free(s);
    close(fd);
    SSL_CTX_free(ctx);
}

} // namespace

int main() {
    int cano[2];
    assert(pipe(cano) == 0);
    int ouvinte = tcp_ouvinte();
    int porta = porta_de(ouvinte);

    // O filho gera o cert efemero e publica o pin TOFU pelo pipe antes
    // do accept; o pai o adota como raiz (em producao, seria uma CA) e
    // so entao conecta as duas voltas.
    pid_t filho = fork();
    assert(filho >= 0);
    if (filho == 0) {
        close(cano[0]);
        servidor(cano[1], ouvinte);
        close(cano[1]);
        _exit(0);
    }
    close(cano[1]);
    char pinbuf[2 * EVP_MAX_MD_SIZE + 1] = {0};
    ssize_t total = 0;
    while ((size_t)total < 2 * SHA256_DIGEST_LENGTH) {
        ssize_t r = read(cano[0], pinbuf + total, sizeof(pinbuf) - 1 - (size_t)total);
        assert(r > 0);
        total += r;
    }
    std::string pin(pinbuf, (size_t)total);
    cliente(porta, pin, SEGREDO, true);
    cliente(porta, pin, "errada", false);
    int estado = 0;
    assert(waitpid(filho, &estado, 0) == filho);
    assert(WIFEXITED(estado) && WEXITSTATUS(estado) == 0);
    std::cout << "23g ok: TLS com pin TOFU + token certo passa, errado nega\n";
    return 0;
}
