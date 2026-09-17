#pragma once

#include "pansy/env.hpp"

namespace pansy {
class Application {
 public:
  Application() {}
  int launch(int argc, char* argv[]);

 private:
  void create_new_user(const std::string& config_file,
                       const std::string& username,
                       const std::string& password);
  void start_proxy_server(const std::string& config_file,
                          const std::string& remote_host,
                          const std::string& local_ip, uint16_t local_port,
                          const std::string& password);
  void open_window(const std::string& config_file);
  bool is_running_on_console();
};
}  // namespace pansy
