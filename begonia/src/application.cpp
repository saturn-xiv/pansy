#include "pansy/application.hpp"
#include "pansy/proxy.hpp"
#include "pansy/screen.hpp"
#include "pansy/version.hpp"

#include <libssh2.h>
#include <openssl/opensslv.h>
#include <sodium.h>
#include <argparse/argparse.hpp>

#define PANSY_PROXY_PASSWORD_MIN_LENGTH 6

int pansy::Application::launch(int argc, char* argv[]) const {
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
        .default_value("127.0.0.1")
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
    return EXIT_SUCCESS;

  } else if (program.is_subcommand_used(proxy_server_command)) {
    const int local_port = proxy_server_command.get<int>("--port");
    const std::string remote_host =
        proxy_server_command.get<std::string>("--host");
    const std::string local_ip = proxy_server_command.get<std::string>("--ip");
    const std::string password =
        proxy_server_command.get<std::string>("--password");

    const pansy::proxy::Server server(config_file);
    server.startup(remote_host, local_ip, local_port, password);
    return EXIT_SUCCESS;
  }

  // if (this->is_running_on_console()) {
  //   BOOST_LOG_TRIVIAL(error)
  //       << "please running from a GUI or with redirected input";
  //   return EXIT_FAILURE;
  // }

  pansy::Screen screen(config_file);
  screen.render();
  return EXIT_SUCCESS;
}

void pansy::Application::create_new_user(const std::string& config_file,
                                         const std::string& username,
                                         const std::string& password) const {
  if (password.length() < PANSY_PROXY_PASSWORD_MIN_LENGTH) {
    throw std::invalid_argument("password is too short");
  }
  if (std::filesystem::exists(config_file)) {
    BOOST_LOG_TRIVIAL(error) << "file " << config_file << " already exists.";
    return;
  }
  BOOST_LOG_TRIVIAL(warning)
      << "create " << config_file << ".toml for " << username;
  pansy::proxy::Config config;
  config.sample(username, password);
  config.save(config_file);
  BOOST_LOG_TRIVIAL(info) << "done.";
}
