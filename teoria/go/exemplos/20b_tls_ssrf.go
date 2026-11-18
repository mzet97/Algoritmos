// Aula 20b Go — TLS com certificado efemero e guarda anti-SSRF.
//
// Servidor HTTPS em 127.0.0.1 com certificado autofirmado gerado em
// memoria (crypto/x509, sem arquivo, sem CA): o cliente fixa a
// impressao SHA-256 (TOFU) e exige "TOKEN <segredo>". A segunda metade
// e o guarda anti-SSRF do cliente que *sai*: URL com host privado,
// redirecionamento para privado ou porta inesperada e recusado antes
// de qualquer byte viajar.
//
// Validar: go vet teoria/go/exemplos/20b_tls_ssrf.go && go run -race teoria/go/exemplos/20b_tls_ssrf.go
package main

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/sha256"
	"crypto/tls"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"fmt"
	"io"
	"math/big"
	"net"
	"net/http"
	"net/url"
	"strings"
	"time"
)

const segredo = "s3nha-da-23c"

// Certificado efemero em memoria: vale 10 minutos, nunca toca o disco.
func certEfemero() (tls.Certificate, [32]byte, error) {
	chave, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		return tls.Certificate{}, [32]byte{}, err
	}
	serial, _ := rand.Int(rand.Reader, big.NewInt(1<<62))
	modelo := &x509.Certificate{
		SerialNumber: serial,
		Subject:      pkix.Name{CommonName: "localhost-20b"},
		NotBefore:    time.Now().Add(-time.Minute),
		NotAfter:     time.Now().Add(10 * time.Minute),
		KeyUsage:     x509.KeyUsageDigitalSignature,
		ExtKeyUsage:  []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth},
		DNSNames:     []string{"localhost"},
		IPAddresses:  []net.IP{net.ParseIP("127.0.0.1")},
	}
	der, err := x509.CreateCertificate(rand.Reader, modelo, modelo, &chave.PublicKey, chave)
	if err != nil {
		return tls.Certificate{}, [32]byte{}, err
	}
	certPEM := pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: der})
	chaveDER, err := x509.MarshalPKCS8PrivateKey(chave)
	if err != nil {
		return tls.Certificate{}, [32]byte{}, err
	}
	chavePEM := pem.EncodeToMemory(&pem.Block{Type: "PRIVATE KEY", Bytes: chaveDER})
	cert, err := tls.X509KeyPair(certPEM, chavePEM)
	if err != nil {
		return tls.Certificate{}, [32]byte{}, err
	}
	folha, err := x509.ParseCertificate(der)
	if err != nil {
		return tls.Certificate{}, [32]byte{}, err
	}
	return cert, sha256.Sum256(folha.Raw), nil
}

// Guarda anti-SSRF: so http/https, sem usuario embutido, sem fragmento
// que vire caminho, host precisa resolver para IP publico (neste lab,
// a lista de bloqueios inclui loopback, privadas, link-local, metadata
// de nuvem e o proprio 169.254.169.254), e nada de redirecionamento:
// o cliente nao segue redirect nenhum.
func bloqueado(u *url.URL) string {
	if u.Scheme != "https" && u.Scheme != "http" {
		return "esquema " + u.Scheme
	}
	if u.User != nil {
		return "userinfo embutido"
	}
	host := u.Hostname()
	if host == "" {
		return "host vazio"
	}
	if ip := net.ParseIP(host); ip != nil {
		if ip.IsLoopback() || ip.IsPrivate() || ip.IsLinkLocalUnicast() || ip.IsLinkLocalMulticast() {
			return "IP literal privado/reservado"
		}
		if ip.Equal(net.ParseIP("169.254.169.254")) {
			return "metadata de nuvem"
		}
		return ""
	}
	// Nome: precisa resolver, e TODOS os resultados precisam ser publicos.
	ips, err := net.LookupIP(host)
	if err != nil || len(ips) == 0 {
		return "nao resolve"
	}
	for _, ip := range ips {
		if ip.IsLoopback() || ip.IsPrivate() || ip.IsLinkLocalUnicast() || ip.IsUnspecified() {
			return "resolve para privado: " + ip.String()
		}
	}
	return ""
}

func buscaSegura(cli *http.Client, destino string) (int, string) {
	u, err := url.Parse(destino)
	if err != nil {
		return 0, "parse: " + err.Error()
	}
	if motivo := bloqueado(u); motivo != "" {
		return 0, "SSRF bloqueado: " + motivo
	}
	resp, err := cli.Get(destino)
	if err != nil {
		return 0, "erro: " + err.Error()
	}
	defer resp.Body.Close()
	corpo, _ := io.ReadAll(io.LimitReader(resp.Body, 1<<20))
	return resp.StatusCode, strings.TrimSpace(string(corpo))
}

func main() {
	cert, pin, err := certEfemero()
	if err != nil {
		panic(err)
	}
	mux := http.NewServeMux()
	mux.HandleFunc("/quem", func(w http.ResponseWriter, r *http.Request) {
		if r.Header.Get("Authorization") != "Bearer "+segredo {
			w.WriteHeader(http.StatusUnauthorized)
			fmt.Fprintln(w, "NEGADO")
			return
		}
		fmt.Fprintln(w, "OK")
	})
	srv := &http.Server{
		Addr:         "127.0.0.1:0",
		Handler:      mux,
		ReadTimeout:  5 * time.Second,
		WriteTimeout: 5 * time.Second,
		TLSConfig:    &tls.Config{Certificates: []tls.Certificate{cert}, MinVersion: tls.VersionTLS12},
	}
	ouvinte, err := tls.Listen("tcp", "127.0.0.1:0", srv.TLSConfig)
	if err != nil {
		panic(err)
	}
	defer ouvinte.Close()
	go srv.Serve(ouvinte)
	base := "https://" + ouvinte.Addr().String()

	// Cliente com pinagem TOFU: confia exatamente neste certificado.
	cli := &http.Client{
		Timeout: 5 * time.Second,
		CheckRedirect: func(req *http.Request, via []*http.Request) error {
			return fmt.Errorf("redirect nao seguido")
		},
		Transport: &http.Transport{
			TLSClientConfig: &tls.Config{
				InsecureSkipVerify: true, // sem CA: o pin abaixo decide
				VerifyPeerCertificate: func(raw [][]byte, _ [][]*x509.Certificate) error {
					if len(raw) == 0 {
						return fmt.Errorf("sem certificado")
					}
					digest := sha256.Sum256(raw[0])
					if digest != pin {
						return fmt.Errorf("pin divergiu")
					}
					return nil
				},
			},
		},
	}

	pedido := func(token string) (int, string) {
		req, _ := http.NewRequest("GET", base+"/quem", nil)
		if token != "" {
			req.Header.Set("Authorization", "Bearer "+token)
		}
		resp, err := cli.Do(req)
		if err != nil {
			return 0, "erro: " + err.Error()
		}
		defer resp.Body.Close()
		corpo, _ := io.ReadAll(io.LimitReader(resp.Body, 1<<20))
		return resp.StatusCode, strings.TrimSpace(string(corpo))
	}

	if st, corpo := pedido(segredo); st != 200 || corpo != "OK" {
		panic(fmt.Sprintf("token certo negado: %d %s", st, corpo))
	}
	fmt.Println("token certo: OK")
	if st, corpo := pedido("errada"); st != 401 || corpo != "NEGADO" {
		panic(fmt.Sprintf("token errado passou: %d %s", st, corpo))
	}
	fmt.Println("token errado: NEGADO")
	if st, corpo := pedido(""); st != 401 {
		panic(fmt.Sprintf("sem token passou: %d %s", st, corpo))
	}
	fmt.Println("sem token: NEGADO")

	// SSRF: nenhum desses viaja.
	for _, alvo := range []string{
		"http://169.254.169.254/latest/meta-data/",
		"http://127.0.0.1/",
		"http://10.0.0.7/admin",
		"file:///etc/passwd",
		"http://localhost@127.0.0.1/",
	} {
		if _, corpo := buscaSegura(cli, alvo); !strings.HasPrefix(corpo, "SSRF bloqueado") {
			panic("SSRF passou: " + alvo + " -> " + corpo)
		}
		fmt.Println("ssrf bloqueado:", alvo)
	}
	fmt.Println("20b ok: TLS com pin TOFU + token, SSRF recusado antes do byte")
}
