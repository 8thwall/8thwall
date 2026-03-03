#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace c8 {

class MultiArray {
public:
  enum class DataType : int32_t {
    STRING = 0x1,
    FLOAT16 = 0x12,
    FLOAT = 0x14,
    UINT8 = 0x21,
    UINT16 = 0x22,
    INT32 = 0x34
  };

  // Default move constructors.
  MultiArray(MultiArray &&b) = default;
  MultiArray &operator=(MultiArray &&b) = default;
  // Disallow copying.
  MultiArray(const MultiArray &) = delete;
  MultiArray &operator=(const MultiArray &) = delete;

  MultiArray();

  MultiArray(const std::vector<uint8_t> &data);

  static bool isMultiArray(const std::vector<uint8_t> &data);

  size_t count() const;
  std::vector<DataType> dataTypes() const;

  MultiArray &append(const std::string &string);
  MultiArray &append(const std::vector<float> &buffer);
  MultiArray &append(const std::vector<int32_t> &buffer);
  MultiArray &append(const std::vector<uint8_t> &buffer);

  const std::vector<float> floatBuffer(int index);
  const std::vector<int32_t> intBuffer(int index);
  const std::vector<uint8_t> ucharBuffer(int index);
  const std::string string(int index);

  std::vector<uint8_t> serialized() const;

private:
  struct Item {
    DataType type;
    std::vector<uint8_t> data;
  };

  std::vector<Item> items;

  void append(const DataType type, const std::vector<uint8_t> &data);
  std::vector<Item> parseItems(const std::vector<uint8_t> &data);
};

}  // namespace c8
