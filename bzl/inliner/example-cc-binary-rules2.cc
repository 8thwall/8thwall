// Example build file with a cc_library rule.

#include <stdio.h>

#include "bzl/inliner/rules2.h"

cc_binary {
  deps = {
    "//c8:vector",
  };
  target_compatible_with = {
    "@platforms//os:macos",
  };
}
cc_end(0x77866aa0)

  int main() {
  printf("\nInliner Hash: 0x%08x\n", inlinerRule());
  return 0;
}
