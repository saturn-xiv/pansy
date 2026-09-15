#include "pansy/utils.hpp"

#include <limits.h>
#include <unistd.h>
#include <fstream>
#include <stdexcept>

// #include <boost/beast/core/detail/base64.hpp>
#include <boost/log/trivial.hpp>

#include <cppcodec/base64_url_unpadded.hpp>

#if defined(_WIN32)

#include <windows.h>

std::string pansy::hostname() {
  DWORD buf_size = 0;
  GetComputerNameExW(ComputerNameDnsHostname, nullptr, &buf_size);

  if (buf_size == 0) {
    throw std::runtime_error("failed to get computer name buffer size: " +
                             GetLastError());
  }

  std::vector<wchar_t> buf(buf_size);
  if (!GetComputerNameExW(ComputerNameDnsHostname, buf.data(), &buf_size)) {
    throw std::runtime_error("failed to get computer name: " + GetLastError());
  }

  std::wstring w_str(buf.data());
  int size_needed = WideCharToMultiByte(
      CP_UTF8, 0, w_str.c_str(), (int)w_str.length(), NULL, 0, NULL, NULL);
  std::string str(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, w_str.c_str(), (int)w_str.length(),
                      str.data(), size_needed, NULL, NULL);
  return str;
}
#else
std::string pansy::hostname() {
  char it[HOST_NAME_MAX + 1];

  if (gethostname(it, sizeof(it)) != 0) {
    throw std::runtime_error("couldn't get hostname");
  }
  return it;
}
#endif

std::string pansy::base64::encode(const std::vector<uint8_t>& buf) {
  return cppcodec::base64_url_unpadded::encode(buf);
}
std::vector<uint8_t> pansy::base64::decode(const std::string& str) {
  return cppcodec::base64_url_unpadded::decode(str);
}
/* FIXME boost base64 encode will change the buffer
std::string pansy::base64::encode(const std::vector<uint8_t>& buf) {
  const size_t len = buf.size();
  std::string it;

  it.resize(boost::beast::detail::base64::encoded_size(len));
  const auto written =
      boost::beast::detail::base64::encode(it.data(), buf.data(), len);
  it.resize(written);

  return it;
}
std::vector<uint8_t> pansy::base64::decode(const std::string& str) {
  const size_t len = str.size();
  std::vector<uint8_t> buf;

  buf.resize(boost::beast::detail::base64::decoded_size(len));
  const auto result =
      boost::beast::detail::base64::decode(buf.data(), str.data(), len);
  buf.resize(result.first);

  return buf;
}
*/

std::string pansy::read_file_to_string(const std::filesystem::path& file) {
  std::ifstream it(file);
  if (!it.is_open()) {
    throw std::invalid_argument("couldn't open file " + file.string());
  }
  std::string buf((std::istreambuf_iterator<char>(it)),
                  std::istreambuf_iterator<char>());
  return buf;
}
