#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

TEST(GlfwInitTest, InitSuccess) {
  // Test that GLFW init succeeds.
  ASSERT_TRUE(glfwInit());
  glfwTerminate();
}

