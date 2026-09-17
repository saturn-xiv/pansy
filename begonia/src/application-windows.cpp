#include "pansy/application.hpp"
#include "pansy/utils.hpp"

#if defined(_WIN32)

bool pansy::is_running_on_console() { return GetConsoleWindow() != NULL; }

#endif
