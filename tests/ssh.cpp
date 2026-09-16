#define BOOST_TEST_MODULE test secrets
#include <boost/test/included/unit_test.hpp>

#include "pansy/proxy.hpp"

#include <boost/algorithm/string/predicate.hpp>

BOOST_AUTO_TEST_CASE(ssh_client) {
  pansy::proxy::Key key;
  key.generate();
  {
    const auto pub = key.pub();
    const auto pem = key.pem();
    std::cout << "=== OPENSSH KEY ===\n" << pub << "\n" << pem << std::endl;
    BOOST_REQUIRE(
        boost::algorithm::starts_with(pub, "ssh-ed25519 AAAAC3NzaC1lZD"));
  }
}
