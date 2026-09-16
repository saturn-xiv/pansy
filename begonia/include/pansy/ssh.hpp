#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <vector>

#include <libssh2.h>

namespace pansy {
namespace ssh {
class SessionManager {
 public:
  bool init(const std::string& host, int port, const std::string& user,
            const std::string& private_key);

  ~SessionManager();
  LIBSSH2_SESSION* session() { return this->_session; }

 private:
  LIBSSH2_SESSION* _session = nullptr;
  int _sock = -1;
};

class ProxySession : public std::enable_shared_from_this<ProxySession> {
 public:
  ProxySession(boost::asio::ip::tcp::socket socket,
               LIBSSH2_SESSION* ssh_session)
      : _client_socket(std::move(socket)), _session(ssh_session) {}

  void start();

 private:
  boost::asio::ip::tcp::socket _client_socket;
  LIBSSH2_SESSION* _session;
  LIBSSH2_CHANNEL* _channel = nullptr;

  bool parse_http_target(const std::string& request, std::string& host,
                         int& port, bool& is_connect);
  void handle_proxy();
  void forward_client_to_ssh();
  void forward_ssh_to_client();
};

class ProxyServer {
 public:
  ProxyServer(boost::asio::io_context& io_context, short port,
              LIBSSH2_SESSION* ssh_session)
      : _acceptor(io_context, boost::asio::ip::tcp::endpoint(
                                  boost::asio::ip::tcp::v4(), port)),
        _session(ssh_session) {
    do_accept();
  }

 private:
  void do_accept();

  boost::asio::ip::tcp::acceptor _acceptor;
  LIBSSH2_SESSION* _session;
};
}  // namespace ssh
}  // namespace pansy
