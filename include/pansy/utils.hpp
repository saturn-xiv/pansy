#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace pansy {
std::string hostname();
std::string read_file_to_string(const std::filesystem::path& file);
namespace base64 {
std::string encode(const std::vector<uint8_t>& buf);
std::vector<uint8_t> decode(const std::string& str);
}  // namespace base64
}  // namespace pansy
