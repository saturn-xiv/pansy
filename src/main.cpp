#include "pansy/application.hpp"

#include <cstdlib>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/log/trivial.hpp>

int main(int argc, char* argv[]) {
  pansy::Application app;

  try {
    return app.launch(argc, argv);
  } catch (const std::exception& e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
  } catch (...) {
    BOOST_LOG_TRIVIAL(error)
        << boost::current_exception_diagnostic_information();
  }

  return EXIT_FAILURE;
}
