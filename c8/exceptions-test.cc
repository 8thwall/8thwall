// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "c8/exceptions.h"

#include <system_error>

#include "gtest/gtest.h"

namespace c8 {

template <typename T>
void throwAnException(const char *what) {
  throw T(what);
}

void throwString(const char *what) { C8_THROW(what); }
void throwInt(int what) { C8_THROW("%d", what); }
void throwStringOutOfRange(const char *what) { C8_THROW_OUT_OF_RANGE(what); }
void throwIntOutOfRange(int what) { C8_THROW_OUT_OF_RANGE("%d", what); }

class ExceptionsTestTest : public ::testing::Test {};

TEST_F(ExceptionsTestTest, TestLogicErrors) {
  EXPECT_THROW(throwAnException<InvalidArgument>("invalid argument"), std::invalid_argument);

  EXPECT_THROW(throwAnException<OutOfRange>("out of range"), std::out_of_range);

  EXPECT_THROW(throwAnException<LogicError>("logic_error"), std::logic_error);
}

TEST_F(ExceptionsTestTest, TestRuntimeErrors) {
  EXPECT_THROW(throwAnException<RuntimeError>("runtime error"), std::runtime_error);

  auto systemError = []() { throw SystemError(std::make_error_code(std::errc::address_in_use)); };
  EXPECT_THROW(systemError(), std::system_error);
}

TEST_F(ExceptionsTestTest, TestThrows) {
  EXPECT_THROW(throwString("one"), std::runtime_error);
  EXPECT_THROW(throwInt(2), std::runtime_error);
  EXPECT_THROW(throwStringOutOfRange("three"), std::out_of_range);
  EXPECT_THROW(throwIntOutOfRange(4), std::out_of_range);
}

}  // namespace c8
