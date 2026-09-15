#include "pansy/proxy.hpp"
#include "pansy/utils.hpp"

#include <boost/log/trivial.hpp>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

void pansy::proxy::Key::listen(const pansy::proxy::SshNode& node,
                               const std::string& host, uint16_t port) const {
  BOOST_LOG_TRIVIAL(debug) << "connect to " << node;
  // TODO
  BOOST_LOG_TRIVIAL(debug) << "linsten on http://" << host << ":" << port;
  // TODO
}

void pansy::proxy::Key::generate() {
  BOOST_LOG_TRIVIAL(debug) << "generate an ed25519 key pair";
  EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);
  EVP_PKEY* pkey = NULL;

  if (!pctx || EVP_PKEY_keygen_init(pctx) <= 0 ||
      EVP_PKEY_keygen(pctx, &pkey) <= 0) {
    throw std::runtime_error("generating ed25519 keypair failed");
  }

  {
    size_t len = 0;
    EVP_PKEY_get_raw_public_key(pkey, NULL, &len);
    this->_public.resize(len);
    EVP_PKEY_get_raw_public_key(pkey, this->_public.data(), &len);
    BOOST_LOG_TRIVIAL(debug) << "public ed25519 key length(" << len << " bytes) "
                             << pansy::base64::encode(this->_public);
  }

  {
    size_t len = 0;
    EVP_PKEY_get_raw_private_key(pkey, NULL, &len);
    this->_private.resize(len);
    EVP_PKEY_get_raw_private_key(pkey, this->_private.data(), &len);
    BOOST_LOG_TRIVIAL(debug) << "private ed25519 key length(" << len << " bytes) "
                             << pansy::base64::encode(this->_private);
  }

  EVP_PKEY_free(pkey);
  EVP_PKEY_CTX_free(pctx);
}
