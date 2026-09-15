#define BOOST_TEST_MODULE test secrets
#include <boost/test/included/unit_test.hpp>

#include "pansy/proxy.hpp"

#include <bits/stdc++.h>

BOOST_AUTO_TEST_CASE(base64) {
  const auto plain = pansy::random::bytes(127);

  std::vector<uint8_t> backup;
  std::copy(plain.begin(), plain.end(), std::back_inserter(backup));
  BOOST_REQUIRE_EQUAL_COLLECTIONS(plain.begin(), plain.end(), backup.begin(),
                                  backup.end());

  {
    const auto b64s = pansy::base64::encode(plain);
    std::cout << "BASE64(" << plain.size() << "," << b64s.length()
              << "): " << b64s << std::endl;
    BOOST_REQUIRE_EQUAL_COLLECTIONS(plain.begin(), plain.end(), backup.begin(),
                                    backup.end());
    const auto tmp = pansy::base64::decode(b64s);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(tmp.begin(), tmp.end(), backup.begin(),
                                    backup.end());
  }
}

static inline void __test_aes(const pansy::proxy::Secrets& secrets,
                              const std::string& password,
                              const std::vector<uint8_t>& plain) {
  std::cout << "PLAIN(" << plain.size() << "): " << pansy::base64::encode(plain)
            << std::endl;
  const auto cipher = secrets.encrypt(password, plain);
  std::cout << "CIPHER(" << cipher.size()
            << "): " << pansy::base64::encode(cipher) << std::endl;
  {
    const auto tmp = secrets.decrypt(password, cipher);
    std::cout << "TMP(" << tmp.size() << "): " << pansy::base64::encode(tmp)
              << std::endl;
    BOOST_REQUIRE_EQUAL_COLLECTIONS(tmp.begin(), tmp.end(), plain.begin(),
                                    plain.end());
  }
}

BOOST_AUTO_TEST_CASE(aes) {
  const std::string password = "Hello, Pansy!";
  pansy::proxy::Secrets secrets;
  secrets.generate();

  {
    const auto plain = pansy::random::bytes(127);
    __test_aes(secrets, password, plain);
  }

  {
    pansy::proxy::Key key;
    key.generate();

    const auto plain = pansy::serialize(key);
    __test_aes(secrets, password, plain);

    {
      const auto cipher = secrets.encrypt(password, plain);
      const auto cipher_s = pansy::base64::encode(cipher);
      const auto buf = pansy::base64::decode(cipher_s);
      BOOST_REQUIRE_EQUAL_COLLECTIONS(buf.begin(), buf.end(), cipher.begin(),
                                      cipher.end());
      const auto tmp = secrets.decrypt(password, buf);
      BOOST_REQUIRE_EQUAL_COLLECTIONS(tmp.begin(), tmp.end(), plain.begin(),
                                      plain.end());
    }
  }
}
