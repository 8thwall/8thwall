# Derived from https://github.com/jbeder/yaml-cpp/blob/yaml-cpp-0.7.0/BUILD.bazel

yaml_cpp_defines = select({
    # On Windows, ensure static linking is used.
    "@platforms//os:windows": [
        "YAML_CPP_STATIC_DEFINE",
        "YAML_CPP_NO_CONTRIB",
    ],
    "//conditions:default": [],
})

cc_library(
    name = "yaml-cpp",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    hdrs = glob([
        "include/**/*.h",
    ]),
    defines = yaml_cpp_defines,
    includes = ["include"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "yaml-cpp_internal",
    hdrs = glob(["src/**/*.h"]),
    strip_include_prefix = "src",
)

cc_test(
    name = "test",
    srcs = glob([
        "test/*.cpp",
        "test/*.h",
        "test/integrations/*.cpp",
        "test/node/*.cpp",
    ]),
    deps = [
        ":yaml-cpp",
        ":yaml-cpp_internal",
        "@com_google_googletest//:gtest_main",
    ],
)
