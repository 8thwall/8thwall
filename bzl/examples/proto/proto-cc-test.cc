#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <filesystem>
#include <limits>
#include <vector>

#include "bzl/examples/proto/api/hello.pb.h"

namespace {

const char *SERIALIZED_FILE = "bzl/examples/proto/data/hello.pb";
const bool WRITE_FILE = false;  // Write a new canonical serialized file for updating this test.

// Write a file with no error checking.
void writeFile(const char *filename, const uint8_t *data, size_t size) {
  FILE *file = fopen(filename, "wb");
  fwrite(data, size, 1, file);
  fclose(file);
}

// Determine file size.
const long int fileSize(FILE *file) {
  fseek(file, 0L, SEEK_END);
  auto fileSize = ftell(file);
  rewind(file);
  return fileSize;
}

// Read a file with no error checking.
const std::vector<uint8_t> readFile(const char *filename) {
  FILE *file;
  file = fopen(filename, "rb");
  auto bytes = fileSize(file);
  std::vector<uint8_t> inputBuffer(bytes);
  fread(inputBuffer.data(), 1, bytes, file);
  return inputBuffer;
}

}  // namespace

TEST(ProtoCcTest, Serialize) {
  std::array<uint8_t, 11> fib = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89};
  Hello hello;
  hello.set_greeting("hello");
  hello.mutable_details()->set_answer(42);
  hello.mutable_details()->set_min(std::numeric_limits<int64_t>::min());
  hello.mutable_details()->set_max(std::numeric_limits<uint64_t>::max());
  hello.mutable_details()->set_fib(fib.data(), fib.size());

  std::vector<uint8_t> serialized(hello.ByteSize());
  hello.SerializeToArray(serialized.data(), serialized.size());

  auto expectedData = readFile(SERIALIZED_FILE);

  EXPECT_EQ(expectedData.size(), serialized.size());

  EXPECT_THAT(serialized, ::testing::ElementsAreArray(expectedData.data(), expectedData.size()));

  if (WRITE_FILE) {
    const char *outfile = "/tmp/hello.pb";
    writeFile(outfile, serialized.data(), serialized.size());
    std::cout << "Wrote " << serialized.size() << " bytes to " << outfile << std::endl;
  }
}

TEST(ProtoCcTest, Deserialize) {
  auto serializedData = readFile(SERIALIZED_FILE);
  Hello hello;
  hello.ParseFromString({reinterpret_cast<char *>(serializedData.data()), serializedData.size()});

  EXPECT_EQ("hello", hello.greeting());
  EXPECT_EQ(42, hello.details().answer());
  EXPECT_EQ(-9223372036854775808uLL, hello.details().min());
  EXPECT_EQ(18446744073709551615uLL, hello.details().max());
  // Proto returns byte arrays as strings for some reason, so we need to make a string here.
  std::string fib = (const char[]){1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 0};
  EXPECT_EQ(fib, hello.details().fib());
}
