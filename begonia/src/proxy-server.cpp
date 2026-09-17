#include "pansy/proxy.hpp"
#include "pansy/ssh.hpp"

void pansy::proxy::Server::startup(const std::string& remote_host,
                                   const std::string& local_ip,
                                   uint16_t local_port,
                                   const std::string& password) const {
  BOOST_LOG_TRIVIAL(info) << "listen " << remote_host << " for "
                          << this->_config._username << " on http://"
                          << local_ip << ":" << local_port;
  if (!this->_config._nodes.contains(remote_host)) {
    throw std::invalid_argument("couldn't found host " + remote_host);
  }
  const auto node = this->_config._nodes.at(remote_host);
  const auto key = this->_config.key(password);

  BOOST_LOG_TRIVIAL(debug)
      << "please append this line into your ~/.ssh/authorized_keys: "
      << key->pub();
  BOOST_LOG_TRIVIAL(debug) << "connect to " << node;

  const std::string pem_key = key->pem();
  // BOOST_LOG_TRIVIAL(debug) << pem_key;

  boost::asio::io_context io_context;

  pansy::ssh::SessionManager manager;
  if (!manager.init(node.ip(io_context), node.port(), node.user(), pem_key)) {
    throw std::runtime_error("failed to connect the server");
  }
  pansy::ssh::ProxyServer server(io_context, local_port, manager.session());

  const auto thread_count = std::thread::hardware_concurrency();
  BOOST_LOG_TRIVIAL(debug) << "linstening on http://" << local_ip << ":"
                           << local_port << " with " << thread_count
                           << " threads";
  std::vector<std::thread> threads;
  for (auto i = 0; i < thread_count; ++i) {
    threads.emplace_back([&io_context]() { io_context.run(); });
  }

  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}
