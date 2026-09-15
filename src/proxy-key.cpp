#include "pansy/proxy.hpp"
#include "pansy/utils.hpp"

#include <boost/log/trivial.hpp>

void pansy::proxy::Key::listen(const pansy::proxy::SshNode& node,
                               const std::string& host, uint16_t port) const {
  BOOST_LOG_TRIVIAL(debug) << "connect to " << node;
  // TODO
  BOOST_LOG_TRIVIAL(debug) << "linsten on http://" << host << ":" << port;
  // TODO
}

void pansy::proxy::Key::load(const std::string& username) {
  BOOST_LOG_TRIVIAL(debug) << "load the ssh(ed25519) key files";
  BOOST_LOG_TRIVIAL(debug)
      << "you can generate them by: ssh-keygen -t ed25519 -f " << username
      << " -C \"" << username << "@" << pansy::hostname() << '"';

  this->_public = pansy::read_file_to_string(username + ".pub");
  this->_private = pansy::read_file_to_string(username);
}
