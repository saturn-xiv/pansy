#pragma once

#include "pansy/proxy.hpp"

namespace pansy {
class Screen {
 public:
  Screen(const pansy::proxy::Config& config) : _config(config) {}
  void render() const;

 private:
  pansy::proxy::Config _config;
};
}  // namespace pansy
