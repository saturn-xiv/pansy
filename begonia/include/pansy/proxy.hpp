#pragma once

#include "pansy/utils.hpp"

namespace pansy {
namespace proxy {
class SshNode {
 public:
  friend class Config;
  SshNode(const std::string& host, const std::string& user,
          std::optional<uint16_t> port = std::nullopt)
      : _host(host), _port(port), _user(user) {}

  friend std::ostream& operator<<(std::ostream& os, const SshNode& obj) {
    os << obj._user << "@" << obj._host << ":" << obj.port();
    return os;
  }

  std::string ip(boost::asio::io_context& io_context) const;
  inline uint16_t port() const { return this->_port.value_or(22); }
  inline std::string host() const { return this->_host; }
  inline std::string user() const { return this->_user; }

 private:
  std::string _host;
  std::optional<uint16_t> _port;
  std::string _user;
};
class Key {
 public:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & _public;
    ar & _private;
  }

  Key() {}

  void generate();
  std::string pem() const;
  std::string pub() const;

 private:
  std::vector<uint8_t> _public;
  std::vector<uint8_t> _private;
};
class Secrets {
 public:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & _salt;
    ar & _nonce;
  }

  Secrets() {}
  void generate();
  std::vector<uint8_t> encrypt(const std::string& password,
                               const std::vector<uint8_t>& plain) const;
  std::vector<uint8_t> decrypt(const std::string& password,
                               const std::vector<uint8_t>& cipher) const;

 private:
  std::vector<uint8_t> key(const std::string& cipher) const;

  std::vector<uint8_t> _salt;
  std::vector<uint8_t> _nonce;
};
class Server;
class Config {
 public:
  friend class pansy::proxy::Server;

  Config() {}
  Config(const std::filesystem::path& file);

  void sample(const std::string& username, const std::string& password);
  void save(const std::filesystem::path& file) const;
  std::unique_ptr<Secrets> secrets() const;
  std::unique_ptr<Key> key(const std::string& password) const;

 private:
  std::string _username;
  std::string _key;
  std::string _secrets;
  std::unordered_map<std::string, SshNode> _nodes;
};

class Server {
 public:
  Server(const std::filesystem::path& config_file) : _config(config_file) {}
  void startup(const std::string& remote_host, const std::string& local_ip,
               uint16_t local_port, const std::string& password) const;
  std::unordered_map<std::string, SshNode> nodes() const {
    return this->_config._nodes;
  }

 private:
  pansy::proxy::Config _config;
};
}  // namespace proxy
}  // namespace pansy
