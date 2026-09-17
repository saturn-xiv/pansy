#include "pansy/proxy.hpp"

std::string pansy::proxy::SshNode::ip(
    boost::asio::io_context& io_context) const {
  {
    boost::system::error_code ec;
    boost::asio::ip::make_address(this->_host, ec);
    if (!ec) {
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
