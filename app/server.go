package app

import (
	"fmt"
	"log/slog"
	"net/http"

	"github.com/saturn-xiv/palm/pansy/proxy"
)

type Ssh struct {
	Host    string
	Port    uint16
	User    string
	KeyFile string
}

func (p *Ssh) StartHttpProxyServer(host string, port uint16) error {
	server, err := proxy.NewServer(p.Host, p.Port, p.User, p.KeyFile)
	if err != nil {
		return err
	}
	addr := fmt.Sprintf("%s:%d", host, port)
	slog.Info("start a proxy server at", "address", addr)
	slog.Debug("or launch an socks5 server", "command", p.socks5(addr))
	return http.ListenAndServe(addr, server)
}

func (p *Ssh) socks5(addr string) string {
	return fmt.Sprintf("ssh -f -p %d -i %s -D %s -CqTnN %s@%s", p.Port, p.KeyFile, addr, p.User, p.Host)
}
