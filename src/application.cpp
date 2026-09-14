#include "pansy/application.hpp"
#include "pansy/version.hpp"

#include <cstdlib>

#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include <libssh2.h>
#include <openssl/opensslv.h>
#include <sodium.h>
#include <argparse/argparse.hpp>

int pansy::Application::launch(int argc, char* argv[]) {
  const std::string version =
      pansy::GIT_VERSION + "(" + pansy::BUILD_TIME + ")";

  argparse::ArgumentParser program(pansy::PROJECT_NAME, version);
  program.add_description(pansy::PROJECT_DESCRIPTION);
  program.add_epilog("https://github.com/saturn-xiv/pansy");
  program.add_argument("-d", "--debug")
      .default_value(false)
      .help("run on debug mode")
      .implicit_value(true);
  program.add_argument("-c", "--config")
      .default_value("config.toml")
      .required();

  argparse::ArgumentParser create_user_command("create-new-user");
  {
    create_user_command.add_argument("-n", "--name").required();
    create_user_command.add_argument("-p", "--password").required();
  }

  argparse::ArgumentParser proxy_server_command("start-proxy-server");
  {
    proxy_server_command.add_argument("-i", "--ip")
        .default_value("0.0.0.0")
        .help("local ip address to listen")
        .required();
    proxy_server_command.add_argument("-p", "--port")
        .default_value(8000)
        .help("local port to listen")
        .scan<'i', int>();
    proxy_server_command.add_argument("-H", "--host")
        .help("remote SSH host")
        .required();
    proxy_server_command.add_argument("-P", "--password").required();
  }

  program.add_subparser(create_user_command);
  program.add_subparser(proxy_server_command);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    BOOST_LOG_TRIVIAL(error) << err.what();
    return EXIT_FAILURE;
  }

  {
    const bool debug = program.get<bool>("--debug");
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >=
        (debug ? boost::log::trivial::debug : boost::log::trivial::info));
    BOOST_LOG_TRIVIAL(debug) << "run on debug mode";
    BOOST_LOG_TRIVIAL(debug) << "OpenSSL v" << OPENSSL_VERSION_STR;
    BOOST_LOG_TRIVIAL(debug) << "Libssh2 v" << LIBSSH2_VERSION;
    BOOST_LOG_TRIVIAL(debug) << "Libsodium v" << SODIUM_VERSION_STRING;
    BOOST_LOG_TRIVIAL(debug)
        << "Boost v" << (BOOST_VERSION / 100000) << "."
        << (BOOST_VERSION / 100 % 1000) << "." << (BOOST_VERSION % 100);

    if (sodium_init() < 0) {
      BOOST_LOG_TRIVIAL(error)
          << "the libsodium couldn't be initialized; it is not safe to use";
      return EXIT_FAILURE;
    }
  }

  const std::string config_file = program.get<std::string>("--config");

  if (program.is_subcommand_used(create_user_command)) {
    const std::string name = create_user_command.get<std::string>("--name");
    const std::string password =
        create_user_command.get<std::string>("--password");

    this->create_new_user(config_file, name, password);
    BOOST_LOG_TRIVIAL(info) << "done.";
    return EXIT_SUCCESS;

  } else if (program.is_subcommand_used(proxy_server_command)) {
    const int local_port = proxy_server_command.get<int>("--port");
    const std::string remote_host =
        proxy_server_command.get<std::string>("--host");
    const std::string local_ip = proxy_server_command.get<std::string>("--ip");
    const std::string password =
        create_user_command.get<std::string>("--password");

    this->start_proxy_server(config_file, remote_host, local_ip, local_port,
                             password);
    return EXIT_SUCCESS;
  }

  if (this->is_running_on_console()) {
    BOOST_LOG_TRIVIAL(error)
        << "please running from a GUI or with redirected input";
    return EXIT_FAILURE;
  }
  this->open_window(config_file);

  return EXIT_SUCCESS;
}

void pansy::Application::create_new_user(const std::string& config_file,
                                         const std::string& name,
                                         const std::string& password) {
  BOOST_LOG_TRIVIAL(info) << "create " << config_file << ".toml for " << name;
  // TODO
}

void pansy::Application::start_proxy_server(const std::string& config_file,
                                            const std::string& remote_host,
                                            const std::string& local_ip,
                                            uint16_t local_port,
                                            const std::string& password) {
  // TODO

  BOOST_LOG_TRIVIAL(info) << "listen on http://" << local_ip << ":"
                          << local_port;
}
