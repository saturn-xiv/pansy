#include "pansy/proxy.hpp"

#include <stdexcept>

#include <boost/log/trivial.hpp>

#define PANSY_PROXY_PASSWORD_MIN_LENGTH 6

void pansy::proxy::Server::start(const std::string& hostname,
                                 const std::string& ip, uint16_t port,
                                 const std::string& password) const {
  BOOST_LOG_TRIVIAL(info) << "listen " << hostname << " for "
                          << this->_config._username << " on http://" << ip
                          << ":" << port;
  if (!this->_config._nodes.contains(hostname)) {
    throw std::invalid_argument("couldn't found host " + hostname);
  }
  if (password.length() < PANSY_PROXY_PASSWORD_MIN_LENGTH) {
    throw std::invalid_argument("password is too short");
  }
  // const auto host = this->_config[hostname];

  // TODO
}
