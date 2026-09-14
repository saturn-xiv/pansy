#include "pansy/application.hpp"

#include <unistd.h>

#if defined(__linux__)

bool pansy::Application::is_running_on_console() {
  return isatty(fileno(stdin)) != 0;
}

void pansy::Application::open_window(const std::string& config_file) {
  // TODO
}

#endif
