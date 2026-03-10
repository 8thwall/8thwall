// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nwaters@8thwall.com)
//
// Used for testing the performance of querying of GL state in different platforms and demonstrates
// the usefulness of contextWrapper

#include <random>

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":offscreen-gl-context",
    ":gl-headers",
    "@com_google_benchmark//:benchmark",
  };
  tags = {"manual"};
}
cc_end(0x03d0a68e);

#include <benchmark/benchmark.h>

#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"

namespace c8 {

class GLStateBenchmark : public benchmark::Fixture {
public:
  GLStateBenchmark() = default;
};

// Testing how efficient querying GL parameters are in different settings.
// Results:
// Native - 72 ns, 9347292 iterations
//   bazel test //c8/pixels/opengl:gl-state-benchmark-test --test_output=all --nocache_test_results
//
// Headless - 1443 ns, 491045 iterations
//   bazel test //c8/pixels/opengl:gl-state-benchmark-test --test_output=all --nocache_test_results
//   --config=jsrun
//
// Safari - 781 ns, 823529 iterations
// bazel test //c8/pixels/opengl:gl-state-benchmark-test --test_output=all --nocache_test_results
//   --config=jsrun --test_arg=--browser --test_arg=safari
//
// Chrome: 126111 ns, 5449 iterations
// bazel test //c8/pixels/opengl:gl-state-benchmark-test --test_output=all --nocache_test_results
//   --config=jsrun --test_arg=--browser --test_arg=chrome
BENCHMARK_F(GLStateBenchmark, TEST_GL_CACHE)(benchmark::State &state) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GLint binding;
  for (auto _ : state) {
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &binding);
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &binding);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &binding);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binding);
    glGetIntegerv(GL_PACK_ALIGNMENT, &binding);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &binding);
  }
}

}  // namespace c8
