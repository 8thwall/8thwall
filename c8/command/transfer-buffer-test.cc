// Copyright (c) 2024 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":transfer-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xcdf6674a);

#include <deque>

#include "c8/command/transfer-buffer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;

namespace c8 {

class TransferBufferTest : public ::testing::Test {};

TEST_F(TransferBufferTest, TestTryWrite) {
  // Create a 256 byte transfer buffer
  TransferBuffer buffer(256);

  // Write some data.
  const char *marker;
  marker = buffer.store("Hello", 6);
  EXPECT_STREQ(marker, "Hello");

  // Can write less than supplied by controlling `count`
  marker = buffer.store("there", 3);
  EXPECT_THAT(std::vector(marker, marker + 3), ElementsAreArray({'t', 'h', 'e'}));

  // Release the data.
  buffer.release(marker + 6);
}

TEST_F(TransferBufferTest, TestZeroBuffer) {
  // Create a 0 byte transfer buffer stub.
  TransferBuffer buffer(0);

  // Write some data and expect it to fail.
  const char *marker;
  marker = buffer.store("Hello", 6);
  EXPECT_EQ(nullptr, marker);

  // Try it again.
  marker = buffer.store("there", 7);
  EXPECT_EQ(nullptr, marker);
}

TEST_F(TransferBufferTest, TestTryWriteWithWrapping) {
  // Create a small transfer buffer, which will wrap around frequently.
  TransferBuffer buffer(29);

  const char *marker, *marker2;
  for (int i = 0; i < 500; ++i) {
    if (i % 2 == 0) {
      // Add two writes.
      marker = buffer.store("AAA", 4);
      marker2 = buffer.store("BBBB", 5);

      EXPECT_STREQ(marker, "AAA");
      EXPECT_STREQ(marker2, "BBBB");

      // Release the data.
      buffer.release(marker2 + 5);
    } else {
      // Add one write.
      marker = buffer.store("CCCCC", 6);
      EXPECT_STREQ(marker, "CCCCC");
      // Release the data.
      buffer.release(marker + 6);
    }
  }
}

TEST_F(TransferBufferTest, TestFullBuffer) {
  // Write until the buffer runs out of space.
  constexpr int BUF_SIZE = 1024;
  int count = 0;

  TransferBuffer buffer(BUF_SIZE);

  while (buffer.store("HELLO!", 7)) {
    count++;
  }

  EXPECT_EQ(count, BUF_SIZE / 7);
}

// Test that the buffer does aligned writes.
TEST_F(TransferBufferTest, TestAlignment) {
  TransferBuffer buffer(227);

  const char *marker;

  const uint64_t u64[3] = {0x123456789ABCDEF0, 0x0FEDCBA987654321, 0x1122334455667788};
  const int64_t i64[2] = {0x123456789ABCDEF0, 0x0FEDCBA987654321};
  const float f32[2] = {3.14159f, 2.71828f};
  const double f64[5] = {1.6180339887, 2.7182818284, 3.1415926535, 0.5772156649, 1.4142135623};
  const int32_t i32[4] = {100, 200, 300, 400};

  for (int i = 0; i < 300; ++i) {
    // Alternate between aligned and not aligned data.
    marker = buffer.store("xxxxxxx", (i % 6) + 1);
    EXPECT_TRUE(marker != nullptr);

    const uint64_t *u64e = buffer.store(u64, sizeof(u64) / sizeof(u64[0]));
    EXPECT_THAT(std::vector(u64e, u64e + sizeof(u64) / sizeof(u64[0])), ElementsAreArray(u64));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(u64e) % alignof(uint64_t), 0);

    const int64_t *i64e = buffer.store(i64, sizeof(i64) / sizeof(i64[0]));
    EXPECT_THAT(std::vector(i64e, i64e + sizeof(i64) / sizeof(i64[0])), ElementsAreArray(i64));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i64e) % alignof(int64_t), 0);

    const float *f32e = buffer.store(f32, sizeof(f32) / sizeof(f32[0]));
    EXPECT_THAT(std::vector(f32e, f32e + sizeof(f32) / sizeof(f32[0])), ElementsAreArray(f32));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(f32e) % alignof(float), 0);

    const double *f64e = buffer.store(f64, sizeof(f64) / sizeof(f64[0]));
    EXPECT_THAT(std::vector(f64e, f64e + sizeof(f64) / sizeof(f64[0])), ElementsAreArray(f64));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(f64e) % alignof(double), 0);

    const int32_t *i32e = buffer.store(i32, sizeof(i32) / sizeof(i32[0]));
    EXPECT_THAT(std::vector(i32e, i32e + sizeof(i32) / sizeof(i32[0])), ElementsAreArray(i32));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i32e) % alignof(int32_t), 0);

    // Release the data.
    buffer.release(i32e + sizeof(i32) / sizeof(i32[0]));
  }
}

// Test that the buffer does aligned writes when using void* pointers, inferring alignment from the
// source data.
TEST_F(TransferBufferTest, TestVoidPointerAlignment) {
  TransferBuffer buffer(227);

  const void *marker;

  const uint64_t u64[3] = {0x123456789ABCDEF0, 0x0FEDCBA987654321, 0x1122334455667788};
  const int64_t i64[2] = {0x123456789ABCDEF0, 0x0FEDCBA987654321};
  const float f32[2] = {3.14159f, 2.71828f};
  const double f64[5] = {1.6180339887, 2.7182818284, 3.1415926535, 0.5772156649, 1.4142135623};
  const int32_t i32[4] = {100, 200, 300, 400};

  for (int i = 0; i < 300; ++i) {
    // Alternate between aligned and not aligned data.
    marker = buffer.store(reinterpret_cast<const void *>("xxxxxxx"), (i % 6) + 1);
    EXPECT_TRUE(marker != nullptr);

    const uint64_t *u64e = reinterpret_cast<const uint64_t *>(
      buffer.store(reinterpret_cast<const void *>(u64), sizeof(u64)));
    EXPECT_THAT(std::vector(u64e, u64e + sizeof(u64) / sizeof(u64[0])), ElementsAreArray(u64));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(u64e) % alignof(uint64_t), 0);

    const int64_t *i64e = reinterpret_cast<const int64_t *>(
      buffer.store(reinterpret_cast<const void *>(i64), sizeof(i64)));
    EXPECT_THAT(std::vector(i64e, i64e + sizeof(i64) / sizeof(i64[0])), ElementsAreArray(i64));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i64e) % alignof(int64_t), 0);

    const float *f32e = reinterpret_cast<const float *>(
      buffer.store(reinterpret_cast<const void *>(f32), sizeof(f32)));
    EXPECT_THAT(std::vector(f32e, f32e + sizeof(f32) / sizeof(f32[0])), ElementsAreArray(f32));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(f32e) % alignof(float), 0);

    const double *f64e = reinterpret_cast<const double *>(
      buffer.store(reinterpret_cast<const void *>(f64), sizeof(f64)));
    EXPECT_THAT(std::vector(f64e, f64e + sizeof(f64) / sizeof(f64[0])), ElementsAreArray(f64));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(f64e) % alignof(double), 0);

    const int32_t *i32e = reinterpret_cast<const int32_t *>(
      buffer.store(reinterpret_cast<const void *>(i32), sizeof(i32)));
    EXPECT_THAT(std::vector(i32e, i32e + sizeof(i32) / sizeof(i32[0])), ElementsAreArray(i32));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i32e) % alignof(int32_t), 0);

    // Release the data.
    buffer.release(reinterpret_cast<const char *>(i32e) + sizeof(i32));
  }
}

// Test that everything works if the TransferBuffer only releases items when it is full.
TEST_F(TransferBufferTest, TestAlignmentWhenOftenFull) {
  TransferBuffer buffer(227);

  const uint64_t u64[3] = {0x123456789ABCDEF0, 0x0FEDCBA987654321, 0x1122334455667788};
  const int64_t i64[2] = {0x123456789ABCDEF0, 0x0FEDCBA987654321};
  const float f32[2] = {3.14159f, 2.71828f};
  const double f64[5] = {1.6180339887, 2.7182818284, 3.1415926535, 0.5772156649, 1.4142135623};
  const int32_t i32[4] = {100, 200, 300, 400};

  std::deque<std::tuple<std::vector<char>, const char *, std::size_t>> ledger;

  const auto storeCheckRelease = [&ledger, &buffer]<typename T>(const T *data, std::size_t count) {
    const T *actual = buffer.store(data, count);
    while (actual == nullptr) {
      const auto &[exp, act, align] = ledger.front();
      // Check alignment.
      EXPECT_EQ(reinterpret_cast<uintptr_t>(act) % align, 0);
      // Check contents.
      EXPECT_THAT(std::vector(act, act + exp.size()), ElementsAreArray(exp));

      buffer.release(act + exp.size());
      ledger.pop_front();

      actual = buffer.store(data, count);
    }

    std::vector<char> expected = std::vector<char>(
      reinterpret_cast<const char *>(data),
      reinterpret_cast<const char *>(data) + count * sizeof(T));
    ledger.push_back(std::make_tuple(expected, reinterpret_cast<const char *>(actual), alignof(T)));
  };

  for (int i = 0; i < 3000; ++i) {
    // Alternate between aligned and not aligned data.
    storeCheckRelease("xxxxxxx", (i % 6) + 1);
    storeCheckRelease(u64, sizeof(u64) / sizeof(u64[0]));
    storeCheckRelease(i64, sizeof(i64) / sizeof(i64[0]));
    storeCheckRelease(f32, sizeof(f32) / sizeof(f32[0]));
    storeCheckRelease(f64, sizeof(f64) / sizeof(f64[0]));
    storeCheckRelease(i32, sizeof(i32) / sizeof(i32[0]));
  }
}

}  // namespace c8
