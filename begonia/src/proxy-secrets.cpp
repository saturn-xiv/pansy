#include "pansy/proxy.hpp"

#include <sodium.h>

std::vector<uint8_t> pansy::proxy::Secrets::key(
    const std::string& password) const {
  std::vector<uint8_t> buf;

  const size_t len = std::min(static_cast<size_t>(16), password.length());
  std::copy_n(password.begin(), len, std::back_inserter(buf));
  std::copy_n(this->_salt.begin(),
              static_cast<size_t>(crypto_secretbox_KEYBYTES - len),
              std::back_inserter(buf));
  // BOOST_LOG_TRIVIAL(debug) << "using " << buf.size()
  //                          << "-bytes aes key: " <<
  //                          pansy::base64::encode(buf);
  return buf;
}

void pansy::proxy::Secrets::generate() {
  {
    this->_salt.resize(crypto_secretbox_KEYBYTES);
    crypto_secretbox_keygen(this->_salt.data());
    BOOST_LOG_TRIVIAL(debug)
        << "generate a " << this->_salt.size()
        << "-bytes salt: " << pansy::base64::encode(this->_salt);
  }
  {
    this->_nonce.resize(crypto_secretbox_NONCEBYTES);
    randombytes_buf(this->_nonce.data(), crypto_secretbox_NONCEBYTES);
    BOOST_LOG_TRIVIAL(debug)
        << "generate a " << this->_nonce.size()
        << "-bytes nonce: " << pansy::base64::encode(this->_nonce);
  }
}

std::vector<uint8_t> pansy::proxy::Secrets::encrypt(
    const std::string& password, const std::vector<uint8_t>& plain) const {
  const auto key = this->key(password);
  std::vector<uint8_t> buf(crypto_secretbox_MACBYTES + plain.size());
  crypto_secretbox_easy(buf.data(), plain.data(), plain.size(),
                        this->_nonce.data(), key.data());
  return buf;
}
std::vector<uint8_t> pansy::proxy::Secrets::decrypt(
    const std::string& password, const std::vector<uint8_t>& cipher) const {
  if (cipher.size() <= crypto_secretbox_MACBYTES) {
    throw std::invalid_argument("invalid cipher length");
  }
  const auto key = this->key(password);
  std::vector<uint8_t> buf(cipher.size() - crypto_secretbox_MACBYTES);
  if (crypto_secretbox_open_easy(buf.data(), cipher.data(), cipher.size(),
                                 this->_nonce.data(), key.data()) != 0) {
    throw std::runtime_error("message forged");
  }
  return buf;
}

std::vector<uint8_t> pansy::random::bytes(size_t len) {
  std::vector<uint8_t> buf(len);
  randombytes_buf(buf.data(), buf.size());
  return buf;
}
