#include "pansy/proxy.hpp"
#include "pansy/ssh.hpp"
#include "pansy/utils.hpp"

#include <stdexcept>

#include <boost/log/trivial.hpp>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

static std::string build_openssh_pem_private_key(
    const std::vector<uint8_t>& key) {
  if (key.size() != 32) {
    throw std::invalid_argument("invalid ed25519 private key buffer length");
  }

  EVP_PKEY* raw = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr,
                                               key.data(), key.size());

  if (!raw) {
    throw std::runtime_error(
        "failed to create EVP_PKEY from raw Ed25519 buffer.");
  }

  std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey(raw, EVP_PKEY_free);

  BIO* bio = BIO_new(BIO_s_mem());
  if (!bio) {
    throw std::runtime_error("failed to create memory BIO.");
  }
  std::unique_ptr<BIO, decltype(&BIO_free)> bio_deleter(bio, BIO_free);

  if (!PEM_write_bio_PrivateKey(bio, pkey.get(), nullptr, nullptr, 0, nullptr,
                                nullptr)) {
    throw std::runtime_error("failed to write private key to PEM BIO.");
  }

  BUF_MEM* tmp;
  BIO_get_mem_ptr(bio, &tmp);
  std::string buf(tmp->data, tmp->length);

  return buf;
}

static std::string base64_encode_openssh_key(const std::vector<uint8_t>& key) {
  BIO* bio = BIO_new(BIO_f_base64());
  BIO* mem = BIO_new(BIO_s_mem());
  bio = BIO_push(bio, mem);
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

  BIO_write(bio, key.data(), static_cast<int>(key.size()));
  BIO_flush(bio);

  BUF_MEM* tmp;
  BIO_get_mem_ptr(bio, &tmp);
  std::string buf(tmp->data, tmp->length);

  BIO_free_all(bio);
  return buf;
}

static std::string format_openssh_public_key(
    const std::vector<uint8_t>& raw_pub_key) {
  if (raw_pub_key.size() != 32) {
    throw std::invalid_argument("invalid key length");
  }

  std::vector<uint8_t> buf;

  // 1. Prefix length of algorithm name (7) as a 4-byte big-endian integer
  buf.insert(buf.end(), {0, 0, 0, 7});
  // 2. Algorithm name: "ssh-ed25519"
  const std::string alg = "ssh-ed25519";
  buf.insert(buf.end(), alg.begin(), alg.end());
  // 3. Prefix length of the key (32) as a 4-byte big-endian integer
  buf.insert(buf.end(), {0, 0, 0, 32});
  // 4. The raw 32 bytes of the public key
  buf.insert(buf.end(), raw_pub_key.begin(), raw_pub_key.end());

  const std::string it =
      std::format("ssh-ed25519 {} {}@{}", base64_encode_openssh_key(buf),
                  pansy::username(), pansy::hostname());
  return it;
}

void pansy::proxy::Key::listen(const pansy::proxy::SshNode& node,
                               const std::string& host, uint16_t port) const {
  BOOST_LOG_TRIVIAL(debug)
      << "please append this line into your ~/.ssh/authorized_keys: "
      << this->pub();
  BOOST_LOG_TRIVIAL(debug) << "connect to " << node;

  pansy::ssh::SessionManager manager;

  const std::string pem_key = this->pem();
  BOOST_LOG_TRIVIAL(debug) << pem_key;
  if (!manager.init(node.host(), node.port(), node.user(), pem_key)) {
    throw std::runtime_error("failed to connect the server");
  }

  boost::asio::io_context io_context;
  pansy::ssh::ProxyServer server(io_context, port, manager.session());

  const auto thread_count = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  BOOST_LOG_TRIVIAL(debug) << "linstening on http://" << host << ":" << port
                           << " with " << thread_count << " threads";

  for (auto i = 0; i < thread_count; ++i) {
    threads.emplace_back([&io_context]() { io_context.run(); });
  }

  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

std::string pansy::proxy::Key::pem() const {
  return build_openssh_pem_private_key(this->_private);
}
std::string pansy::proxy::Key::pub() const {
  return format_openssh_public_key(this->_public);
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
    BOOST_LOG_TRIVIAL(debug)
        << "public ed25519 key length(" << len << " bytes) "
        << pansy::base64::encode(this->_public);
  }

  {
    size_t len = 0;
    EVP_PKEY_get_raw_private_key(pkey, NULL, &len);
    this->_private.resize(len);
    EVP_PKEY_get_raw_private_key(pkey, this->_private.data(), &len);
    BOOST_LOG_TRIVIAL(debug)
        << "private ed25519 key length(" << len << " bytes) "
        << pansy::base64::encode(this->_private);
  }

  EVP_PKEY_free(pkey);
  EVP_PKEY_CTX_free(pctx);
}
