// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":file-io",
    "//c8:string",
    "//c8/string:format",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x1f01ffe9);

#include <cstdio>
#include <cstdlib>

#include "c8/io/file-io.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "gtest/gtest.h"

namespace c8 {

class FileIOTest : public ::testing::Test {};

static const char *tmpPath() { return std::getenv("TEST_TMPDIR"); }

TEST_F(FileIOTest, FileExists) {
  // Create a test string.
  String contents = "My my hey hey, rock and roll is here to stay.";

  // Write the string to a file, test.txt
  auto dest = format("%s/test-text.txt", tmpPath());
  writeTextFile(dest, contents);

  EXPECT_TRUE(fileExists(dest));
  EXPECT_FALSE(fileExists("nonexistant"));
}

TEST_F(FileIOTest, WriteAndReadBinary) {
  // Create a test string.
  Vector<uint8_t> contents = {0, 1, 127, 128, 254, 255};

  // Write the string to a file, test.txt
  auto dest = format("%s/test-bin.bin", tmpPath());
  writeFile(dest, contents.data(), contents.size());

  // Read back the contents of the written file.
  auto readContents = readFile(dest);

  // Make sure the read-back contents matches the written contents.
  EXPECT_EQ(6, readContents.size());
  EXPECT_EQ(0, readContents[0]);
  EXPECT_EQ(1, readContents[1]);
  EXPECT_EQ(127, readContents[2]);
  EXPECT_EQ(128, readContents[3]);
  EXPECT_EQ(254, readContents[4]);
  EXPECT_EQ(255, readContents[5]);
}

TEST_F(FileIOTest, WriteAndReadText) {
  // Create a test string.
  String contents = "My my hey hey, rock and roll is here to stay.";

  // Write the string to a file, test.txt
  auto dest = format("%s/test-text.txt", tmpPath());
  writeTextFile(dest, contents);

  // Read back the contents of the written file.
  auto readStr = readTextFile(dest);

  // Make sure the read-back contents matches the written contents.
  EXPECT_EQ(contents, readStr);
}

TEST_F(FileIOTest, WriteAndReadFloat) {
  // Create test floats.
  Vector<float> contents = {0.3, 1.5, 12.7, 1.28, 254, 25.5};

  // Write the floats to a file, test-float.bin
  auto dest = format("%s/test-float.bin", tmpPath());
  writeFloatFile(dest, contents.data(), contents.size());

  // Read back the contents of the written file.
  auto readContents = readFloatFile(dest);

  // Make sure the read-back contents matches the written contents.
  EXPECT_EQ(24, readContents.size());
  EXPECT_EQ(0.3f, readContents[0]);
  EXPECT_EQ(1.5f, readContents[1]);
  EXPECT_EQ(12.7f, readContents[2]);
  EXPECT_EQ(1.28f, readContents[3]);
  EXPECT_EQ(254, readContents[4]);
  EXPECT_EQ(25.5f, readContents[5]);
}

TEST_F(FileIOTest, ReadJsonFileDoesntExist) {
  EXPECT_THROW(readJsonFile("nonexistant"), std::exception);
  EXPECT_THROW(readJsonFile("nonexistant", "", "file not found message"), std::exception);
}

TEST_F(FileIOTest, ReadJson) {
  // Create a test string.
  String contents = "{\"key\":1}";

  // Write the string to a file, test.txt
  auto dest = format("%s/test-text.txt", tmpPath());
  writeTextFile(dest, contents);

  // Read back the contents of the written file.
  auto json = readJsonFile(dest);

  // Make sure the read-back contents matches the written contents.
  EXPECT_EQ(json["key"], 1);

  // Also can read the key directly.
  EXPECT_EQ(readJsonFile(dest, "key"), 1);
}

}  // namespace c8
