load("//bzl/js:js.bzl", "js_test")

# These files provide extra functionality that we add to the GLFW library.
exports_files(
    [
        "glfw-extra-apple.h",
        "glfw-extra-apple.m",
        "glfw-extra.h",
        "glfw-extra.c",
    ],
    visibility = ["@glfw//:__pkg__"],
)

cc_test(
    name = "glfw-init-test",
    size = "small",
    srcs = ["glfw-init-test.cc"],
    target_compatible_with = select({
        "//bzl/conditions:apple": [],
        "//bzl/conditions:windows": [],
        "//bzl/conditions:linux": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps =
        [
            "@com_google_googletest//:gtest_main",
            "@glfw",
        ],
)

js_test(
    name = "glfw-node-init-test",
    main = [
        "glfw-node-init-test.js",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//third_party/glfw-raub",
    ],
)
