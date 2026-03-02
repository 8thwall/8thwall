// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include <gtest/gtest.h>

extern "C" {
  // Prototype for the assembly function defined in hello-world-x64.asm.
  extern int exampleIntAsm();
}

namespace nc {

class HelloWorldX64Test : public ::testing::Test {};

TEST_F(HelloWorldX64Test, Test64bitAsmFunction) {
  // Call the MASM function and check the result.
  EXPECT_EQ(17, exampleIntAsm());
}

}  // namespace nc
