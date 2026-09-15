#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/serialization/vector.hpp>

namespace pansy {
std::string hostname();
std::string read_file_to_string(const std::filesystem::path& file);
namespace base64 {
std::string encode(const std::vector<uint8_t>& buf);
std::vector<uint8_t> decode(const std::string& str);
}  // namespace base64

template <typename T>
std::vector<uint8_t> serialize(const T& object) {
  std::vector<char> buf;

  {
    boost::iostreams::filtering_ostream out;
    out.push(boost::iostreams::bzip2_compressor());
    out.push(boost::iostreams::back_inserter(
        *reinterpret_cast<std::vector<char>*>(&buf)));

    // boost::iostreams::stream<
    //     boost::iostreams::back_insert_device<std::vector<char>>>
    //     out(boost::iostreams::back_inserter(buf));

    boost::archive::binary_oarchive oa(out);
    oa << object;
  }
  return std::vector<uint8_t>(buf.begin(), buf.end());
}

template <typename T>
std::unique_ptr<T> deserialize(const std::vector<uint8_t>& buffer) {
  boost::iostreams::filtering_istream dp;
  dp.push(boost::iostreams::bzip2_decompressor());

  std::string cs(reinterpret_cast<const char*>(buffer.data()), buffer.size());
  std::stringstream css(cs, std::ios::in | std::ios::binary);
  dp.push(css);

  std::stringstream dss(std::ios::in | std::ios::out | std::ios::binary);
  boost::iostreams::copy(dp, dss);
  std::string ds = dss.str();

  std::unique_ptr<T> it = std::make_unique<T>();

  std::istringstream input(ds, std::ios::binary);
  boost::archive::binary_iarchive ia(input);
  ia >> *it;

  return std::move(it);
}

}  // namespace pansy
