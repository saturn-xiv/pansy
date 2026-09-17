#include "pansy/proxy.hpp"

#if defined(_WIN32)

bool pansy::Application::is_running_on_console() {
  return GetConsoleWindow() != NULL;
}

void pansy::Application::open_window(const std::string& config_file) {
  // TODO
}

#endif
