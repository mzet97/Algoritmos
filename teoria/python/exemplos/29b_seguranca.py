"""Aula 29b — seguranca defensiva em Python: dois riscos, duas correcoes.

Risco 1: subprocess com shell=True e entrada externa (injecao de comando).
Correcao: lista de argumentos, sem shell, com entrada validada.

Risco 2: desserializacao nao confiavel com pickle (execucao na leitura).
Correcao: JSON com esquema minimo e limites de tamanho/tipos.
"""
import json
import pickle
import subprocess
import sys

TETO_BYTES = 65536


# ---------- risco 2: o veneno (so demonstrado, nunca desserializado) ----------
class Exploit:
    def __reduce__(self):
        return (print, ("PWNED: pickle executou na leitura",))


# ---------- caminho seguro: JSON com validacao de fronteira ----------
def analisar_pedido(dados: bytes) -> dict:
    if len(dados) > TETO_BYTES:
        raise ValueError("pedido acima do teto")
    try:
        obj = json.loads(dados.decode("utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError) as e:
        raise ValueError(f"json invalido: {e}") from e
    if not isinstance(obj, dict):
        raise ValueError("pedido deve ser objeto")
    if set(obj) - {"chave", "valor"}:
        raise ValueError("campos nao permitidos")
    chave, valor = obj.get("chave"), obj.get("valor")
    if not isinstance(chave, str) or not (1 <= len(chave) <= 64):
        raise ValueError("chave invalida")
    if not isinstance(valor, str) or len(valor) > 1024:
        raise ValueError("valor invalido")
    return {"chave": chave, "valor": valor}


# ---------- caminho seguro: subprocess sem shell ----------
def contar_ocorrencias(texto: str, padrao: str) -> int:
    if "\n" in padrao or "\x00" in padrao or len(padrao) > 64:
        raise ValueError("padrao invalido")
    # Sem shell: padrao e argumento, nunca parte de um comando interpretado.
    r = subprocess.run(
        [sys.executable, "-c",
         "import sys; print(sys.stdin.read().count(sys.argv[1]))", padrao],
        input=texto, capture_output=True, text=True, timeout=10, check=True,
    )
    return int(r.stdout.strip())


def main() -> int:
    # O pickle malicioso existe como bytes; a aula NAO o carrega. A prova
    # de que ele executaria esta no __reduce__ acima, lido como codigo.
    veneno = pickle.dumps(Exploit())
    print(f"pickle hostil gerado ({len(veneno)} bytes) e NAO desserializado")

    # Fronteira JSON: valido passa, abusos param na validacao.
    ok = analisar_pedido(b'{"chave": "k1", "valor": "v1"}')
    assert ok == {"chave": "k1", "valor": "v1"}
    for hostil in (
        b'{"chave": "k", "valor": "v", "admin": true}',  # campo a mais
        b'{"chave": "", "valor": "v"}',                  # chave vazia
        b'{"chave": 7, "valor": "v"}',                   # tipo errado
        b"[1,2,3]",                                      # nao objeto
        b"x" * (TETO_BYTES + 1),                         # acima do teto
        veneno,                                          # pickle nao e JSON
    ):
        try:
            analisar_pedido(hostil)
        except ValueError:
            pass
        else:
            raise AssertionError(f"deveria recusar: {hostil[:40]!r}")
    print("json com esquema: valido passa, 6 abusos recusados na fronteira")

    # Subprocess sem shell: tentativa de injecao vira argumento literal.
    n = contar_ocorrencias("um dois um", "um")
    assert n == 2
    injecao = contar_ocorrencias("texto qualquer", '"; print(999); "')
    assert injecao == 0  # contou o literal; nada executou
    print("subprocess sem shell: injecao tratada como texto, contagem correta")
    print("29b ok: pickle recusado por desenho, json validado, shell fora do caminho")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
