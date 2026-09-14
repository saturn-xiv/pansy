#include "pansy/application.hpp"

#include <unistd.h>

#if defined(__APPLE__) || defined(__MACH__)

#include <TargetConditionals.h>

#if TARGET_OS_MAC

bool pansy::Application::is_running_on_console() {
  return isatty(fileno(stdin)) != 0;
}

void pansy::Application::open_window(const std::string& config_file) {
  // TODO
}

#endif

#endif
