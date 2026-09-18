#pragma once

#include "pansy/env.hpp"

namespace pansy {
class Application {
 public:
  Application() {}
  int launch(int argc, char* argv[]) const;

 private:
  void create_new_user(const std::string& config_file,
                       const std::string& username,
                       const std::string& password) const;
};
}  // namespace pansy
