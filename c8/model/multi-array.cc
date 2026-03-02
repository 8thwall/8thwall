#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "multi-array.h",
  };
  deps = {};
}
cc_end(0x71c2dd87);

#include <ostream>
#include <type_traits>

#include "c8/model/multi-array.h"

namespace c8 {

namespace {

constexpr int32_t Magic = 0x5252414d;  // MARR big endian
constexpr int32_t Version = 1;

}  // namespace

MultiArray::MultiArray() {}

MultiArray::MultiArray(const std::vector<uint8_t> &data) : MultiArray() {
  items = parseItems(data);
}

size_t MultiArray::count() const { return items.size(); }

const std::vector<float> MultiArray::floatBuffer(int index) {
  const Item &item = items[index];
  std::vector<float> buffer(item.data.size() / sizeof(float));
  // CHECK_EQ(item.data.size(), buffer.size() * sizeof(float));
  std::copy(item.data.begin(), item.data.end(), reinterpret_cast<uint8_t *>(buffer.data()));
  return buffer;
}

const std::vector<int32_t> MultiArray::intBuffer(int index) {
  const Item &item = items[index];
  std::vector<int32_t> buffer(item.data.size() / sizeof(int32_t));
  // CHECK_EQ(item.data.size(), buffer.size() * sizeof(int32_t));
  std::copy(item.data.begin(), item.data.end(), reinterpret_cast<uint8_t *>(buffer.data()));
  return buffer;
}

const std::vector<uint8_t> MultiArray::ucharBuffer(int index) {
  const Item &item = items[index];
  std::vector<uint8_t> buffer(item.data.size() / sizeof(uint8_t));
  // CHECK_EQ(item.data.size(), buffer.size() * sizeof(uint8_t));
  std::copy(item.data.begin(), item.data.end(), reinterpret_cast<uint8_t *>(buffer.data()));
  return buffer;
}

const std::string MultiArray::string(int index) {
  const Item &item = items[index];
  return std::string(item.data.begin(), item.data.end());
}

std::vector<MultiArray::DataType> MultiArray::dataTypes() const {
  std::vector<DataType> types;
  for (const auto &item : items) {
    types.push_back(item.type);
  }
  return types;
}

MultiArray &MultiArray::append(const std::string &string) {
  append(DataType::STRING, std::vector<uint8_t>(string.begin(), string.end()));
  return *this;
}

MultiArray &MultiArray::append(const std::vector<float> &buffer) {
  append(
    DataType::FLOAT,
    std::vector<uint8_t>(
      reinterpret_cast<const uint8_t *>(buffer.data()),
      reinterpret_cast<const uint8_t *>(buffer.data()) + sizeof(float) * buffer.size()));
  return *this;
}

MultiArray &MultiArray::append(const std::vector<int32_t> &buffer) {
  append(
    DataType::INT32,
    std::vector<uint8_t>(
      reinterpret_cast<const uint8_t *>(buffer.data()),
      reinterpret_cast<const uint8_t *>(buffer.data()) + sizeof(int32_t) * buffer.size()));
  return *this;
}

MultiArray &MultiArray::append(const std::vector<uint8_t> &buffer) {
  append(
    DataType::UINT8,
    std::vector<uint8_t>(
      reinterpret_cast<const uint8_t *>(buffer.data()),
      reinterpret_cast<const uint8_t *>(buffer.data()) + sizeof(uint8_t) * buffer.size()));
  return *this;
}

std::vector<uint8_t> MultiArray::serialized() const {
  std::vector<int32_t> header = {Magic, Version, 0, static_cast<int32_t>(items.size())};
  for (const auto &item : items) {
    header.push_back(static_cast<int32_t>(item.type));
    header.push_back(static_cast<int32_t>(item.data.size()));
  }
  std::vector<uint8_t> data(
    reinterpret_cast<const uint8_t *>(header.data()),
    reinterpret_cast<const uint8_t *>(header.data()) + sizeof(int32_t) * header.size());
  for (const auto &item : items) {
    data.insert(data.end(), item.data.begin(), item.data.end());
  }
  return data;
}

void MultiArray::append(const DataType type, const std::vector<uint8_t> &data) {
  items.push_back(Item{type, data});
}

bool MultiArray::isMultiArray(const std::vector<uint8_t> &data) {
  if (data.size() < 16) {
    return false;
  }

  const int32_t *header = reinterpret_cast<const int32_t *>(data.data());
  int headerSlots = 4 + 2 * header[3];

  if (header[0] != Magic || header[1] != Version || data.size() < headerSlots * sizeof(int32_t)) {
    return false;
  }

  size_t start = headerSlots * sizeof(int32_t);

  for (int i = 4; i < headerSlots; i += 2) {
    int32_t size = header[i + 1];

    if (start + size > data.size()) {
      return false;
    }

    start += size;
  }

  return true;
}

std::vector<MultiArray::Item> MultiArray::parseItems(const std::vector<uint8_t> &data) {
  if (!isMultiArray(data)) {
    return std::vector<Item>();
  }

  const int32_t *header = reinterpret_cast<const int32_t *>(data.data());
  int headerSlots = 4 + 2 * header[3];

  std::vector<Item> items;
  size_t start = headerSlots * sizeof(int32_t);

  for (int i = 4; i < headerSlots; i += 2) {
    DataType type = static_cast<DataType>(header[i]);
    int32_t size = header[i + 1];

    std::vector<uint8_t> d =
      std::vector<uint8_t>(data.begin() + start, data.begin() + start + size);

    items.push_back(
      Item{type, std::vector<uint8_t>(data.begin() + start, data.begin() + start + size)});
    start += size;
  }

  return items;
}

}  // namespace c8
