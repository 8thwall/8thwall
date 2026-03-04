#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "physics-shared.h",
  };
  deps = {
    "@flecs//:flecs",
    "@joltphysics",
    "//c8:vector",
    "//c8:map",
  };
}
cc_end(0xa77c8e8d);

namespace c8 {

// Empty

}  // namespace c8
