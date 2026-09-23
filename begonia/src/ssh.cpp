#include "pansy/ssh.hpp"

bool pansy::ssh::SessionManager::init(const std::string& host, int port,
                                      const std::string& user,
                                      const std::string& private_key) {
  libssh2_init(0);

  this->_sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sin{};
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = inet_addr(host.c_str());

  if (connect(this->_sock, (struct sockaddr*)(&sin), sizeof(sin)) != 0) {
    BOOST_LOG_TRIVIAL(error) << "failed to connect ssh server";
    return false;
  }

  this->_session = libssh2_session_init();
  if (libssh2_session_handshake(this->_session, this->_sock)) {
    BOOST_LOG_TRIVIAL(error) << "failed to ssh session handshake";
    return false;
  }
  {
    const auto status = libssh2_userauth_publickey_frommemory(
        this->_session, user.c_str(), user.length(), NULL, 0,
        private_key.c_str(), private_key.length(), NULL);
    if (status) {
      BOOST_LOG_TRIVIAL(error)
          << "failed to auth user by key pairs(" << status << ")";
      return false;
    }
  }
  // if (libssh2_userauth_password(this->_session, user.c_str(), pass.c_str()))
  // {
  //   BOOST_LOG_TRIVIAL(error) << "failed to auth user by password";
  //   return false;
  // }

  BOOST_LOG_TRIVIAL(debug) << "succeed to open a ssh tunnel session";
  return true;
}

pansy::ssh::SessionManager::~SessionManager() {
  if (this->_session) {
    libssh2_session_disconnect(this->_session, "Shutdown");
    libssh2_session_free(this->_session);
  }
  if (this->_sock != -1) {
    close(this->_sock);
  }
  libssh2_exit();

  this->_session = nullptr;
  this->_sock = -1;
}

void pansy::ssh::ProxySession::forward_client_to_ssh() {
  char buf[8192];
  boost::system::error_code ec;
  while (true) {
    size_t len = this->_client_socket.read_some(boost::asio::buffer(buf), ec);
    if (ec) {
      BOOST_LOG_TRIVIAL(error)
          << "failed to read: " << ec.value() << " " << ec.message();
      break;
    }
    if (len == 0) {
      BOOST_LOG_TRIVIAL(debug) << "nothing need send to ssh";
      break;
    }

    ssize_t written = 0;
    while (written < static_cast<ssize_t>(len)) {
      ssize_t ret =
          libssh2_channel_write(this->_channel, buf + written, len - written);
      if (ret < 0) {
        BOOST_LOG_TRIVIAL(error) << "got " << ret;
        return;
      }
      written += ret;
    }
    BOOST_LOG_TRIVIAL(debug) << "send to server " << written << " bytes";
  }

  BOOST_LOG_TRIVIAL(debug) << "send eof to channel";
  libssh2_channel_send_eof(this->_channel);
}

void pansy::ssh::ProxySession::forward_ssh_to_client() {
  char buf[8192];
  boost::system::error_code ec;
  while (true) {
    ssize_t len = libssh2_channel_read(this->_channel, buf, sizeof(buf));
    if (len <= 0) {
      BOOST_LOG_TRIVIAL(debug)
          << "nothing need send to client (" << len << " bytes)";
      break;
    }
    BOOST_LOG_TRIVIAL(debug) << "write back to client " << len << " bytes";

    boost::asio::write(this->_client_socket, boost::asio::buffer(buf, len), ec);
    if (ec) {
      BOOST_LOG_TRIVIAL(error)
          << "failed to write: " << ec.value() << " " << ec.message();
      break;
    }
  }
}

void pansy::ssh::ProxySession::handle_proxy() {
  try {
    std::vector<char> buffer(8192);
    boost::system::error_code ec;

    // pre-read request header
    size_t bytes_transferred = this->_client_socket.receive(
        boost::asio::buffer(buffer), boost::asio::ip::tcp::socket::message_peek,
        ec);

    if (ec) {
      BOOST_LOG_TRIVIAL(error)
          << "failed to receive: " << ec.value() << " " << ec.message();
      return;
    }
    if (bytes_transferred == 0) {
      BOOST_LOG_TRIVIAL(debug) << "nothing need to transferred";
      return;
    }

    std::string req_str(buffer.data(), bytes_transferred);
    std::string target_host;
    int target_port = 80;
    bool is_connect = false;

    if (!parse_http_target(req_str, target_host, target_port, is_connect)) {
      BOOST_LOG_TRIVIAL(error) << "failed to parse http target";
      return;
    }

    // create port forwarding channel
    this->_channel = libssh2_channel_direct_tcpip(
        this->_session, target_host.c_str(), target_port);
    if (!this->_channel) {
      BOOST_LOG_TRIVIAL(error) << "cann't open ssh channel for " << target_host
                               << ":" << target_port;
      return;
    }

    if (is_connect) {
      // HTTP 200 OK
      std::string ok_resp = "HTTP/1.1 200 Connection Established\r\n\r\n";
      boost::asio::write(this->_client_socket, boost::asio::buffer(ok_resp));
      // clear raw CONNECT request in buffer
      this->_client_socket.read_some(boost::asio::buffer(buffer));
    }

    // start two-ways forwarding threads
    std::thread t1([this, &buffer]() { forward_client_to_ssh(); });
    std::thread t2([this]() { forward_ssh_to_client(); });

    if (t1.joinable()) {
      t1.join();
    }
    if (t2.joinable()) {
      t2.join();
    }

    if (this->_channel) {
      libssh2_channel_free(this->_channel);
      this->_channel = nullptr;
    }

  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(error) << "proxy " << e.what();
  }
}

bool pansy::ssh::ProxySession::parse_http_target(const std::string& request,
                                                 std::string& host, int& port,
                                                 bool& is_connect) {
  std::istringstream stream(request);
  std::string method, url, protocol;
  if (!(stream >> method >> url >> protocol)) {
    BOOST_LOG_TRIVIAL(error) << "failed to parse http method/url/protocol";
    return false;
  }
  BOOST_LOG_TRIVIAL(debug) << protocol << " " << method << " " << url;

  if (method == "CONNECT") {
    is_connect = true;
    size_t pos = url.find(':');
    if (pos != std::string::npos) {
      host = url.substr(0, pos);
      port = std::stoi(url.substr(pos + 1));
    } else {
      return false;
    }
  } else {
    is_connect = false;
    port = 80;
    size_t host_pos = request.find("Host: ");
    if (host_pos != std::string::npos) {
      size_t end_pos = request.find("\r\n", host_pos);
      std::string host_line =
          request.substr(host_pos + 6, end_pos - (host_pos + 6));
      size_t colon_pos = host_line.find(':');
      if (colon_pos != std::string::npos) {
        host = host_line.substr(0, colon_pos);
        port = std::stoi(host_line.substr(colon_pos + 1));
      } else {
        host = host_line;
      }
    } else {
      return false;
    }
  }
  return true;
}

void pansy::ssh::ProxySession::start() {
  // run on background thread pool
  auto self = shared_from_this();
  boost::asio::post(this->_client_socket.get_executor(),
                    [this, self]() { handle_proxy(); });
}

void pansy::ssh::ProxyServer::do_accept() {
  this->_acceptor.async_accept([this](boost::system::error_code ec,
                                      boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      std::make_shared<ProxySession>(std::move(socket), this->_session)
          ->start();
    }
    do_accept();
  });
}

void pansy::ssh::ProxyServer::shutdown() {
  boost::system::error_code ec;
  this->_acceptor.close(ec);

  if (ec) {
    BOOST_LOG_TRIVIAL(error) << ec.message();
  }
}
