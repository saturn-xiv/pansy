#pragma once

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

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

  inline uint16_t port() const { return this->_port.value_or(22); }

 private:
  std::string _host;
  std::optional<uint16_t> _port;
  std::string _user;
};
class Config {
 public:
  friend class Server;

  Config() {}
  Config(const std::filesystem::path& file);

  void sample(const std::string& username, const std::string& password);
  void save(const std::filesystem::path& file) const;

 private:
  std::string _username;
  std::string _key;
  std::string _secrets;
  std::unordered_map<std::string, SshNode> _nodes;
};
class Key {
 public:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & _public;
    ar & _private;
  }
  friend class Secrets;

  Key() {}

  void load(const std::string& username);

 private:
  std::string _public;
  std::string _private;
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

class Server {
 public:
  Server(const std::filesystem::path& config_file) : _config(config_file) {}
  void start(const std::string& host, const std::string& ip, uint16_t port,
             const std::string& password) const;

 private:
  Config _config;
};
}  // namespace proxy
}  // namespace pansy
