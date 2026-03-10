// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":client-gl",
    ":gl-headers",
    ":offscreen-gl-context",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xc0eed5e3);

#include "c8/pixels/opengl/client-gl.h"

#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/glext.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "gtest/gtest.h"

namespace c8 {

class ClientGlTest : public ::testing::Test {};

TEST_F(ClientGlTest, ExtensionFunctionPointers) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

#if C8_OPENGL_VERSION_3
  // Get an extension method that we expect to exist in OpenGL 3
  auto glGetObjectLabel =
    (PFNGLGETOBJECTLABELEXTPROC)clientGlGetProcAddress("glGetObjectLabelEXT");

  // Ensure the pointer is not nullptr.
  EXPECT_NE(nullptr, glGetObjectLabel);
#endif

#if C8_OPENGL_VERSION_2
  // Get an extension method that we expect to exist in OpenGL 2
  auto glGenVertexArrays =
    (PFNGLGENVERTEXARRAYSOESPROC)clientGlGetProcAddress("glGenVertexArraysOES");

  // Ensure the pointer is not nullptr.
  EXPECT_NE(nullptr, glGenVertexArrays);

#ifdef JAVASCRIPT
  // Get an extension method that we know doesn't exist in WebGL.
  auto glCopyImageSubData =
    (PFNGLCOPYIMAGESUBDATAEXTPROC)clientGlGetProcAddress("glCopyImageSubDataEXT");

  // Ensure the pointer is nullptr.
  EXPECT_EQ(nullptr, glCopyImageSubData);
#endif
#endif
}

}  // namespace c8
