#include "pansy/proxy.hpp"

#include <fstream>

#include <boost/beast/core/detail/base64.hpp>
#include <boost/log/trivial.hpp>

#include <sodium.h>
#include <toml++/toml.hpp>

void pansy::proxy::Token::build(const std::string& password) {
  {
    const size_t len = 32;
    BOOST_LOG_TRIVIAL(debug) << "generate a " << len << "-bytes salt";
    this->_salt.resize(len);
    randombytes_buf(this->_salt.data(), len);
  }
  {
    BOOST_LOG_TRIVIAL(debug) << "generate a new ssh(ed25519) key";
    BOOST_LOG_TRIVIAL(debug) << "public key: " << "public key";
    this->_key.resize(125);
  }
}
void pansy::proxy::Token::parse(const std::string& secret) {
  std::string raw;
  {
    raw.resize(boost::beast::detail::base64::decoded_size(secret.size()));
    auto result = boost::beast::detail::base64::decode(
        raw.data(), secret.data(), secret.size());
    raw.resize(result.first);
  }

  std::stringstream ss(raw);
  boost::archive::binary_iarchive ia(ss);
  ia >> *this;
}

std::string pansy::proxy::Token::to_string() const {
  std::stringstream ss;
  boost::archive::binary_oarchive oa(ss);
  oa << *this;

  const std::string raw = ss.str();
  std::string buf;
  {
    buf.resize(boost::beast::detail::base64::encoded_size(raw.size()));
    auto len = boost::beast::detail::base64::encode(buf.data(), raw.data(),
                                                    raw.size());
    buf.resize(len);
  }

  return buf;
}

void pansy::proxy::Config::sample(const std::string& username,
                                  const std::string& password) {
  {
    Token token;
    token.build(password);
    this->_token = token.to_string();
  }

  this->_username = username;
  this->_nodes.emplace("host-1", SshNode("xxx.xxx.xxx.xx1", "user.1"));
  this->_nodes.emplace("host-2", SshNode("xxx.xxx.xxx.xx2", "user.2"));
}

pansy::proxy::Config::Config(const std::filesystem::path& config_file) {
  BOOST_LOG_TRIVIAL(debug) << "load configuration from" << config_file.string();
  const toml::table root = toml::parse_file(config_file.string());
  this->_username = root["username"].value<std::string>().value();
  this->_token = root["token"].value<std::string>().value();

  const auto nodes = root["nodes"].as_table();
  if (nodes != nullptr) {
    for (auto&& [name, value] : *nodes) {
      const auto node = value.as_table();
      const std::string host = (*node)["host"].value<std::string>().value();
      const std::string user = (*node)["user"].value<std::string>().value();
      const std::optional<uint16_t> port = (*node)["port"].value<uint16_t>();
      BOOST_LOG_TRIVIAL(debug) << "found node " << name << " " << user << "@"
                               << host << ":" << port.value_or(22);
      this->_nodes.emplace(name, SshNode(host, user, port));
    }
  }
}

void pansy::proxy::Config::save(
    const std::filesystem::path& config_file) const {
  auto nodes = toml::table{};
  for (const auto& [name, node] : this->_nodes) {
    auto it = toml::table{};
    it.insert_or_assign("host", node._host);
    it.insert_or_assign("user", node._user);
    if (node._port) {
      it.insert_or_assign("port", node._port.value());
    }

    nodes.insert_or_assign(name, it);
  }

  auto root = toml::table{
      {"username", this->_username}, {"token", this->_token}, {"nodes", nodes}};

  std::ofstream file(config_file);
  if (!file.is_open()) {
    BOOST_LOG_TRIVIAL(error) << "failed to open file for writing";
    return;
  }

  file << root;
}
