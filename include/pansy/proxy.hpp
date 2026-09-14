#pragma once

#include <cstdint>
#include <filesystem>
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
  friend class Server;
  SshNode(const std::string& host, const std::string& user,
          std::optional<uint16_t> port = std::nullopt)
      : _host(host), _port(port), _user(user) {}

  uint16_t port() const { return this->_port.value_or(22); }

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
  std::string _token;
  std::unordered_map<std::string, SshNode> _nodes;
};
class Token {
 public:
  friend class boost::serialization::access;
  friend class Config;
  friend class Server;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & _salt;
    ar & _key;
  }

  Token() {}
  void build(const std::string& password);
  void parse(const std::string& secret);

  std::string to_string() const;

 private:
  std::vector<uint8_t> _salt;
  std::vector<uint8_t> _key;
};

class Server {
 public:
  Server(const std::filesystem::path& config_file)
      : _config(std::make_unique<Config>(config_file)) {}
  void start(const std::string& host, const std::string& ip, uint16_t port,
             const std::string& password) const;

 private:
  std::unique_ptr<Config> _config;
};
}  // namespace proxy
}  // namespace pansy
