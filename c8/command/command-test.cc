// Copyright (c) 2025 Niantic, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":command",
    ":transfer-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x4a130255);

#include "c8/command/command.h"
#include "c8/command/transfer-buffer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
// Function to add an int and a float.
int funcA(int a, float b) {
  // Do something
  return a + static_cast<int>(b);
}

// Another function to add a double and a uint32_t.
int funcB(double a, uint32_t b) { return static_cast<int>(a) + static_cast<int>(b); }

// Third function with same type signature as funcB.
int funcC(double a, uint32_t b) { return static_cast<int>(a) + static_cast<int>(b); }

// What about overloaded functions
char overloadedFunc(int a, float b) { return static_cast<char>(a + static_cast<int>(b)); }

// What about overloaded functions
char overloadedFunc(int a, float b, char c) {
  return static_cast<char>(a + static_cast<int>(b) + c);
}

// Differ only by param types
char overloadedFunc(double a, uint32_t b) { return static_cast<char>(a + static_cast<int>(b)); }

// Adds up four numbers in an array.
int addFour(const int *arr) { return arr[0] + arr[1] + arr[2] + arr[3]; }

}  // namespace

namespace c8 {

using cmd::FixedArrayWrap;
using cmd::TransferWrap;

class CommandTest : public ::testing::Test {};

TEST_F(CommandTest, TestRunCommand) {
  Command commandA{funcA, 1, 2.0f};
  Command commandB{funcB, 3.0, 4};
  Command commandC{funcC, 2.0, 3};
  Command commandD{+[](int a, float b) { return a + static_cast<int>(b); }, 1, 2.0f};

  // Run the commands.
  EXPECT_EQ(3, commandA.run());
  EXPECT_EQ(7, commandB.run());
  EXPECT_EQ(5, commandC.run());
  EXPECT_EQ(3, commandD.run());

  // Confirm that commandA and commandB have different type signatures.
  static_assert(!std::is_same_v<decltype(commandA), decltype(commandB)>);

  // Confirm that commandB and commandC have the same type signature.
  static_assert(std::is_same_v<decltype(commandB), decltype(commandC)>);

  // Confirm that commandA and commandD have the same type signature.
  static_assert(std::is_same_v<decltype(commandA), decltype(commandD)>);

  // Confirm that the commands satisfy the CommandConcept.
  static_assert(CommandConcept<decltype(commandA)>);
  static_assert(CommandConcept<decltype(commandB)>);
  static_assert(CommandConcept<decltype(commandC)>);
  static_assert(CommandConcept<decltype(commandD)>);
}

TEST_F(CommandTest, TestRunStaticCommand) {
  TransferBuffer tb(0);

  Command commandA{funcA, 1, 2.0f};
  Command commandB{funcB, 3.0, 4};
  Command commandC{funcC, 2.0, 3};

  // Run the commands using the static run function.
  const char *src = reinterpret_cast<const char *>(&commandA);
  EXPECT_EQ(
    3, decltype(commandA)::deserializeRun(src, nullptr, src + sizeof(decltype(commandA)), tb));
  src = reinterpret_cast<const char *>(&commandB);
  EXPECT_EQ(
    7, decltype(commandB)::deserializeRun(src, nullptr, src + sizeof(decltype(commandB)), tb));
  src = reinterpret_cast<const char *>(&commandC);
  EXPECT_EQ(
    5, decltype(commandC)::deserializeRun(src, nullptr, src + sizeof(decltype(commandC)), tb));
}

TEST_F(CommandTest, TestOverloadedMethods) {
  // Use function pointers to identify the correct overloads.
  char (*overloadA)(int, float) = &overloadedFunc;
  char (*overloadB)(int, float, char) = &overloadedFunc;
  char (*overloadC)(double, uint32_t) = &overloadedFunc;

  Command commandA{overloadA, 1, 2.0f};
  Command commandB{overloadB, 3, 4.0f, 'c'};
  Command commandC{overloadC, 5, 6.0f};

  // Run the commands.
  EXPECT_EQ(3, commandA.run());
  EXPECT_EQ(106, commandB.run());
  EXPECT_EQ(11, commandC.run());
}

TEST_F(CommandTest, TestRunCommandWithArray) {
  std::array<int, 4> arr{1, 2, 3, 4};
  int arr2[4] = {1, 2, 3, 4};

  // When creating a command with an array wrapper, the data should be copied.
  Command command{addFour, FixedArrayWrap<int, 4>(arr.data())};  // Raw pointer.
  Command command2{addFour, FixedArrayWrap(arr)};                // std::array.
  Command command3{addFour, FixedArrayWrap(arr2)};               // Compile-time array.

  static_assert(sizeof(FixedArrayWrap<int, 100>) == 400);
  static_assert(
    sizeof(Command<int (*)(int *), FixedArrayWrap<int, 100>>) > sizeof(FixedArrayWrap<int, 100>));

  // Change the array numbers to ensure command is running on a copy of the arrays.
  arr[3] = 5;
  arr2[3] = 5;

  // Run the command.
  EXPECT_EQ(10, command.run());
  EXPECT_EQ(10, command2.run());
  EXPECT_EQ(10, command3.run());
}

TEST_F(CommandTest, TestRunCommandWithTransferBuffer) {
  // Create a transfer buffer.
  TransferBuffer buffer(256);

  std::array<int, 4> arr{1, 2, 3, 4};

  // Create a command that reads the array from the buffer.
  Command command{addFour, TransferWrap{arr.data(), arr.size()}};

  // Store the transfer wrap data in the transfer buffer.
  command.storeTransferWraps(buffer);

  // Change the array numbers to ensure command is running on a copy of the array.
  arr[3] = 5;

  // Run the command.
  EXPECT_EQ(10, command.run());

  // Release the transfer wrap data from the transfer buffer.
  command.releaseTransferWraps(buffer);

  // Try again with an offset.
  constexpr int OFFSET = 16;
  Command command2{addFour, TransferWrap{arr.data() - OFFSET, arr.size(), OFFSET}};

  // Store the transfer wrap data in the transfer buffer.
  command2.storeTransferWraps(buffer);

  // Change the array numbers to again to ensure command is running on a copy of the array.
  arr[3] = 6;

  // Run the command.
  EXPECT_EQ(11, command2.run());

  // Release the transfer wrap data from the transfer buffer.
  command2.releaseTransferWraps(buffer);
}

TEST_F(CommandTest, TestVoidRunCommandWithTransferBuffer) {
  // Create a transfer buffer.
  TransferBuffer buffer(256);

  std::array<int, 4> arr{1, 2, 3, 4};

  static int result = 0;

  // Create a command that returns void and reads the array from the buffer.
  Command command{
    +[](const int *arr) { result = arr[0] + arr[1] + arr[2] + arr[3]; },
    TransferWrap{arr.data(), arr.size()}};

  // Store the transfer wrap data in the transfer buffer.
  command.storeTransferWraps(buffer);

  // Change the array numbers to ensure command is running on a copy of the arrays.
  arr[3] = 5;

  // Run the command, which releases the transfer wrap.
  command.run();
  EXPECT_EQ(10, result);

  // Release the transfer wrap data from the transfer buffer.
  command.releaseTransferWraps(buffer);

  // Try again with an offset.
  constexpr int OFFSET = -16;
  Command command2{
    +[](const int *arr) { result = arr[0] + arr[1] + arr[2] + arr[3]; },
    TransferWrap{arr.data() - OFFSET, arr.size(), OFFSET}};

  // Store the transfer wrap data in the transfer buffer.
  command2.storeTransferWraps(buffer);

  // Change the array numbers to again to ensure command is running on a copy of the array.
  arr[3] = 6;

  // Run the command, which releases the transfer wrap.
  command2.run();
  EXPECT_EQ(11, result);

  // Release the transfer wrap data from the transfer buffer.
  command2.releaseTransferWraps(buffer);
}

}  // namespace c8
