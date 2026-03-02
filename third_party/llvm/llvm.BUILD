# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.

package(default_visibility = ["//visibility:public"])

filegroup(
  name = "stub",
  srcs = [],
)

filegroup(
  name = "all-files",
  srcs = glob(["**/*"]),
)

cc_library(
    name = "clang_rt-builtins",
    srcs = glob([
      "**/libclang_rt.builtins.a", # This will be empty on windows platforms
      "**/clang_rt.builtins-x86_64.lib" # This will be empty on non-windows platforms
    ]),
)
