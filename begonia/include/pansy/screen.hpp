#pragma once

#include "pansy/proxy.hpp"

namespace pansy {
class Screen {
 public:
  Screen(const std::filesystem::path& config_file) : _server(config_file) {}
  void render() const;

 private:
  pansy::proxy::Server _server;
};
}  // namespace pansy
