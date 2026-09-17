#include "pansy/proxy.hpp"
#include "pansy/utils.hpp"

#include <toml++/toml.hpp>

std::unique_ptr<pansy::proxy::Secrets> pansy::proxy::Config::secrets() const {
  return pansy::deserialize<Secrets>(pansy::base64::decode(this->_secrets));
}
std::unique_ptr<pansy::proxy::Key> pansy::proxy::Config::key(
    const std::string& password) const {
  const auto ciper = pansy::base64::decode(this->_key);
  const auto secrets = this->secrets();
  const auto buf = secrets->decrypt(password, ciper);
  return pansy::deserialize<Key>(buf);
}

void pansy::proxy::Config::sample(const std::string& username,
                                  const std::string& password) {
  {
    Secrets secrets;
    secrets.generate();
    this->_secrets = pansy::base64::encode(pansy::serialize(secrets));
    {
      Key key;
      key.generate();
      this->_key = pansy::base64::encode(
          secrets.encrypt(password, pansy::serialize(key)));
    }
  }

  this->_username = username;
  this->_nodes.emplace("host-1", SshNode("xxx.xxx.xxx.xx1", "user.1"));
  this->_nodes.emplace("host-2", SshNode("xxx.xxx.xxx.xx2", "user.2"));
}

pansy::proxy::Config::Config(const std::filesystem::path& config_file) {
  BOOST_LOG_TRIVIAL(debug) << "load configuration from "
                           << config_file.string();
  const toml::table root = toml::parse_file(config_file.string());
  this->_username = root["username"].value<std::string>().value();
  this->_secrets = root["secrets"].value<std::string>().value();
  this->_key = root["key"].value<std::string>().value();

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

  auto root = toml::table{{"username", this->_username},
                          {"key", this->_key},
                          {"secrets", this->_secrets},
                          {"nodes", nodes}};

  std::ofstream file(config_file);
  if (!file.is_open()) {
    throw std::invalid_argument("failed to open file" + config_file.string() +
                                " for writing");
  }

  file << root;
}
