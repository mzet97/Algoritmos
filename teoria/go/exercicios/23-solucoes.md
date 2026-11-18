---
layout: default
---

# Aula 23 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../23.md#19-exercicios-obrigatorios). Contrato, geracao e chamada executados nesta rodada (`protoc` + plugins Go do gRPC):

```proto
// usuarios.proto
syntax = "proto3";

package usuarios.v1;

option go_package = "usuarios/gen/usuariosv1";

service Usuarios {
  rpc BuscarPorID(BuscarPorIDRequest) returns (BuscarPorIDResponse);
}

message BuscarPorIDRequest {
  int64 id = 1;
}

message BuscarPorIDResponse {
  int64 id = 1;
  string nome = 2;
  string email = 3;
}
```

Geracao e execucao (modulo temporario `usuarios`, com o `.proto` em `gen/usuariosv1/` para o import gerado casar com o caminho):

    go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
    go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
    protoc --go_out=. --go-grpc_out=. --go_opt=paths=source_relative --go-grpc_opt=paths=source_relative gen/usuariosv1/usuarios.proto
    go get google.golang.org/grpc google.golang.org/protobuf

```go
// servidor e cliente no mesmo harness de laboratorio
package main

import (
	"context"
	"fmt"
	"net"
	"time"

	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/credentials/insecure"
	"google.golang.org/grpc/status"

	pb "usuarios/gen/usuariosv1"
)

type servidor struct {
	pb.UnimplementedUsuariosServer
	dados map[int64]*pb.BuscarPorIDResponse
}

func (s *servidor) BuscarPorID(ctx context.Context, req *pb.BuscarPorIDRequest) (*pb.BuscarPorIDResponse, error) {
	if req.GetId() <= 0 {
		return nil, status.Error(codes.InvalidArgument, "id deve ser positivo")
	}
	u, ok := s.dados[req.GetId()]
	if !ok {
		return nil, status.Error(codes.NotFound, "usuario inexistente")
	}
	return u, nil
}

func main() {
	lis, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		panic(err)
	}
	grpcServer := grpc.NewServer()
	pb.RegisterUsuariosServer(grpcServer, &servidor{dados: map[int64]*pb.BuscarPorIDResponse{
		1: {Id: 1, Nome: "Ada", Email: "ada@exemplo.dev"},
	}})
	go grpcServer.Serve(lis)
	defer grpcServer.Stop()

	conn, err := grpc.NewClient(lis.Addr().String(), grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		panic(err)
	}
	defer conn.Close()
	cliente := pb.NewUsuariosClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()
	resp, err := cliente.BuscarPorID(ctx, &pb.BuscarPorIDRequest{Id: 1})
	fmt.Println("id 1:", resp.GetNome(), resp.GetEmail(), "| erro:", err)

	_, err = cliente.BuscarPorID(ctx, &pb.BuscarPorIDRequest{Id: 99})
	fmt.Println("id 99 codigo:", status.Code(err), "(esperado NotFound)")
	_, err = cliente.BuscarPorID(ctx, &pb.BuscarPorIDRequest{Id: 0})
	fmt.Println("id 0 codigo:", status.Code(err), "(esperado InvalidArgument)")
	fmt.Println("testes ok")
}
```

3. `NotFound`: o pedido e valido e o recurso nao existe. `InvalidArgument`: o pedido nem deveria ter saido do cliente (id zero, formato errado) — repetir nao ajuda. `Unavailable`: o servico nao pode responder **agora** (sobrecarga, dependencia fora) — repetir com backoff pode ajudar. Confundir os tres ensina o cliente a repetir o irrepetivel ou desistir do temporario.

4. Timeout interno existe porque latencia compoe: um servico a 200 ms do limite chama outro sem limite, que chama outro... e a cascata descobre o limite na marra, em producao. Prazo no cliente (com `context` propagado) transforma vizinho lento em erro `DeadlineExceeded` tratavel, nao em goroutines empilhadas.

5. REST x gRPC no mesmo caso (buscar usuario por ID, servico interno): REST ganha em depuracao com `curl`, cache HTTP e consumidores heterogeneos; gRPC ganha em contrato compilado dos dois lados, payload binario menor e streaming tipado. Interno entre servicos Go, gRPC e a escolha natural; na borda com navegador e terceiros, REST continua.

6. Streaming vence unary quando a resposta e naturalmente uma sequencia: logs ao vivo, progresso de job, feed de precos, exportacao linha a linha. Unary forcado nesses casos vira polling (o cliente perguntando "e ai?") ou payload gigante de uma vez.

**Autoverificacao:** chamada unary devolve a Ada, 99 vira `NotFound`, 0 vira `InvalidArgument` — com o prazo do cliente ativo.
