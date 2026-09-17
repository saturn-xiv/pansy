#include "pansy/application.hpp"
#include "pansy/utils.hpp"

#if defined(__APPLE__) || defined(__MACH__)

#include <TargetConditionals.h>

#if TARGET_OS_MAC

bool pansy::is_running_on_console() { return isatty(fileno(stdin)) != 0; }

#endif

#endif
