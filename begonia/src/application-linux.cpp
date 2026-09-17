#include "pansy/application.hpp"
#include "pansy/utils.hpp"

#if defined(__linux__)

bool pansy::is_running_on_console() { return isatty(fileno(stdin)) != 0; }

#endif
