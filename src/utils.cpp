#include "pansy/utils.hpp"

#include <limits.h>
#include <unistd.h>
#include <fstream>
#include <stdexcept>

#include <boost/beast/core/detail/base64.hpp>
#include <boost/log/trivial.hpp>

std::string pansy::base64::encode(const std::vector<uint8_t>& buf) {
  std::string it;

  it.resize(boost::beast::detail::base64::encoded_size(buf.size()));
  const auto written =
      boost::beast::detail::base64::encode(it.data(), buf.data(), buf.size());
  it.resize(written);

  return it;
}
std::vector<uint8_t> pansy::base64::decode(const std::string& str) {
  std::vector<uint8_t> buf;

  buf.resize(boost::beast::detail::base64::decoded_size(str.size()));
  const auto result =
      boost::beast::detail::base64::decode(buf.data(), str.data(), str.size());
  buf.resize(result.first);

  return buf;
}

std::string pansy::hostname() {
  char it[HOST_NAME_MAX + 1];

  if (gethostname(it, sizeof(it)) != 0) {
    throw std::runtime_error("couldn't get hostname");
  }
  return it;
}
std::string pansy::read_file_to_string(const std::filesystem::path& file) {
  std::ifstream it(file);
  if (!it.is_open()) {
    throw std::invalid_argument("couldn't open file " + file.string());
  }
  std::string buf((std::istreambuf_iterator<char>(it)),
                  std::istreambuf_iterator<char>());
  return buf;
}
