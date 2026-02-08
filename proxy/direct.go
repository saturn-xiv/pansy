package proxy

import (
	"context"
	"io"
	"log/slog"
	"net"
	"net/http"
	"time"
)

type Direct struct {
	transport *http.Transport
}

func NewDirect(timeout time.Duration) *Direct {
	tr := http.DefaultTransport.(*http.Transport)
	tr.DialContext = func(ctx context.Context, network, addr string) (net.Conn, error) {
		return (&net.Dialer{
			Timeout: 200 * time.Millisecond,
		}).Dial(network, addr)
	}
	return &Direct{transport: tr}
}

func (p *Direct) ServeHTTP(wrt http.ResponseWriter, req *http.Request) error {
	if req.Method == "CONNECT" {
		slog.Error("can't handle", "method", req.Method)
		http.Error(wrt, req.Method, http.StatusMethodNotAllowed)
		return nil
	}
	start := time.Now()

	res, err := p.transport.RoundTrip(req)
	if err != nil {
		if ner, ok := err.(net.Error); ok && ner.Timeout() {
			slog.Error("round-trip timeout", "reason", err)
			return err
		}

		slog.Error("round-trip", "reason", err)
		http.Error(wrt, err.Error(), http.StatusInternalServerError)
		return err
	}
	defer res.Body.Close()

	copy_header(wrt, res)
	wrt.WriteHeader(res.StatusCode)

	cnt, err := io.Copy(wrt, res.Body)
	if err != nil {
		slog.Error("copy body", "reason", err)
		http.Error(wrt, err.Error(), http.StatusInternalServerError)
		return err
	}

	slog.Debug("response", "url", req.URL.String(), "status", res.Status, "duration", beautify_duration(time.Since(start)), "size", beautify_size(cnt))
	return nil
}

func (p *Direct) Connect(ctx context.Context, wrt http.ResponseWriter, req *http.Request) error {
	if req.Method != "CONNECT" {
		slog.Error("can't handle", "method", req.Method)
		http.Error(wrt, req.Method, http.StatusMethodNotAllowed)
		return nil
	}
	start := time.Now()

	hij, ok := wrt.(http.Hijacker)
	if !ok {
		msg := "server does not support Hijacker"
		slog.Error(msg)
		http.Error(wrt, msg, http.StatusInternalServerError)
		return nil
	}

	dst, err := p.transport.DialContext(ctx, "tcp", req.URL.Host)
	if err != nil {
		if ner, ok := err.(net.Error); ok && ner.Timeout() {
			slog.Error("dial timeout", "reason", err)
			return err
		}
		slog.Error("dial", "reason", err)
		http.Error(wrt, err.Error(), http.StatusInternalServerError)
		return err
	}
	defer dst.Close()

	src, _, err := hij.Hijack()
	if err != nil {
		slog.Error("hijack", "reason", err)
		http.Error(wrt, err.Error(), http.StatusInternalServerError)
		return err
	}
	defer src.Close()

	src.Write([]byte("HTTP/1.1 200 OK\r\n\r\n"))

	copy_and_wait := func(dst, src net.Conn, c chan int64) {
		n, err := io.Copy(dst, src)
		if err != nil {
			slog.Error("copy data", "reason", err)
		}
		if con, ok := dst.(closeWriter); ok {
			con.CloseWrite()
		}
		c <- n
	}

	client_to_remote := make(chan int64)
	go copy_and_wait(dst, src, client_to_remote)

	remote_to_client := make(chan int64)
	go copy_and_wait(src, dst, remote_to_client)

	var client_to_remote_count, remote_to_client_count int64
	for i := 0; i < 2; {
		select {
		case client_to_remote_count = <-client_to_remote:
			i++
		case remote_to_client_count = <-remote_to_client:
			i++
		}
	}

	slog.Debug("close", "url", req.URL.String(), "after", beautify_duration(time.Since(start)), "up", beautify_size(client_to_remote_count), "down", beautify_size(remote_to_client_count))
	return nil
}

type closeWriter interface {
	CloseWrite() error
}
