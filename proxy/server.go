package proxy

import (
	"context"
	"fmt"
	"log/slog"
	"net"
	"net/http"
	"os"

	"golang.org/x/crypto/ssh"
)

type Server struct {
	client *ssh.Client
	direct *Direct
}

func NewServer(host string, port uint16, user string, key_file string) (*Server, error) {
	cfg := ssh.ClientConfig{
		User:            user,
		HostKeyCallback: ssh.InsecureIgnoreHostKey(),
	}
	slog.Debug("load key from", "file", key_file)
	pem, err := os.ReadFile(key_file)
	if err != nil {
		return nil, err
	}
	signer, err := ssh.ParsePrivateKey(pem)
	if err != nil {
		return nil, err
	}

	cfg.Auth = append(cfg.Auth, ssh.PublicKeys(signer))
	addr := fmt.Sprintf("%s:%d", host, port)
	slog.Debug("dial to ssh server", "address", addr, "user", user)
	cli, err := ssh.Dial("tcp", addr, &cfg)
	if err != nil {
		return nil, err
	}

	server := &Server{client: cli}

	dial := func(ctx context.Context, network string, addr string) (net.Conn, error) {
		slog.Debug("dial", "network", network, "address", addr)
		return server.client.Dial(network, addr)
	}

	return &Server{
		client: cli,
		direct: &Direct{
			transport: &http.Transport{DialContext: dial},
		}}, nil
}

func (p *Server) ServeHTTP(wrt http.ResponseWriter, req *http.Request) {
	slog.Debug("serve", "proto", req.Proto, "method", req.Method, "url", req.URL.String())
	if req.Method == "CONNECT" {
		p.direct.Connect(context.Background(), wrt, req)
	} else if req.URL.IsAbs() {
		slog.Warn("clear request uri and headers", "url", req.URL.String())
		req.RequestURI = ""
		clear(req.Header)
		p.direct.ServeHTTP(wrt, req)
	} else {
		slog.Error("not a full URL path", "url", req.URL.String())
	}
}
