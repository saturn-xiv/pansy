#include "pansy/proxy.hpp"

#include <arpa/inet.h>

std::string pansy::proxy::SshNode::ip(
    boost::asio::io_context& io_context) const {
  {
    struct in_addr ip4;
    if (inet_pton(AF_INET, this->_host.c_str(), &ip4) == 1) {
      return this->_host;
    }
  }

  {
    struct in6_addr ip6;
    if (inet_pton(AF_INET6, this->_host.c_str(), &ip6) == 1) {
      return this->_host;
    }
  }

  boost::asio::ip::tcp::resolver resolver(io_context);
  boost::asio::ip::tcp::resolver::results_type results =
      resolver.resolve(this->_host, "0");
  for (const auto& entry : results) {
    return entry.endpoint().address().to_string();
  }

  throw std::runtime_error("couldn't reslove " + this->_host);
}
