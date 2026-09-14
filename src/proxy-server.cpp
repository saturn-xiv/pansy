#include "pansy/proxy.hpp"

#include <boost/log/trivial.hpp>

void pansy::proxy::Server::start(const std::string& host, const std::string& ip,
                                 uint16_t port,
                                 const std::string& password) const {
  if (!this->_config->_nodes.contains(host)) {
    BOOST_LOG_TRIVIAL(error) << "couldn't found host " << host;
    return;
  }
  BOOST_LOG_TRIVIAL(info) << "listen " << host << " for "
                          << this->_config->_username << " on http://" << ip
                          << ":" << port;
  // TODO
}
