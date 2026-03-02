load("@bazel_skylib//lib:selects.bzl", "selects")
load("@pip-deps//:requirements.bzl", "requirement")
load("//bzl/crosstool:wasm-pthread.bzl", "emoptsAllowMemoryGrowth")
load("//bzl/js:js.bzl", "js_cli", "js_test")
load("//bzl/node:node-addon.bzl", "node_addon")
load("//bzl/redist:linux_binary_redist.bzl", "linux_redist")
load("//bzl/wasm:wasm.bzl", "wasm_binary", "wasm_test")

exports_files(["hello.cc"])

cc_binary(
    name = "hello",
    srcs = [
        "hello.cc",
    ],
    visibility = ["//bzl/examples:__subpackages__"],
)

cc_binary(
    name = "hello-exceptions",
    srcs = [
        "hello-exceptions.cc",
    ],
    target_compatible_with = select({
        "//bzl/conditions:wasm": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

linux_redist(
    name = "hello-exceptions_linux_redist",
    binary_executable = ":hello-exceptions",
    target_compatible_with = select({
        "//bzl/conditions:linux": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)

alias(
    name = "hello-exceptions_redist",
    actual = select({
        "//bzl/conditions:linux": ":hello-exceptions_linux_redist",
        "//conditions:default": ":hello-exceptions",
    }),
)

cc_binary(
    name = "hello-c",
    srcs = [
        "hello-c.c",
    ],
)

cc_test(
    name = "hello-cc-test",
    size = "small",
    srcs = [
        "hello-cc-test.cc",
    ],
    deps = [
        "@com_google_googletest//:gtest_main",
    ],
)

py_binary(
    name = "hello-py",
    srcs = ["hello-py.py"],
    deps = [
        # Available pip deps are listed in //bzl/python/requirements.txt.
        requirement("python-dateutil"),
    ],
)

cc_binary(
    name = "bio-hello-c",
    srcs = [
        "bio_hello.c",
    ],
    deps = [
        "@openssl//:ssl",
    ],
)

js_cli(
    name = "hello-js-cli",
    main = [
        "hello-js.js",
    ],
)

js_test(
    name = "hello-js-test",
    main = [
        "hello-js-test.js",
    ],
    deps = [
        "//bzl/js:chai-js",
    ],
)

js_test(
    name = "node-addon-test",
    main = [
        "node-addon-test.js",
    ],
    deps = [
        ":example-node-addon",
        "//bzl/js:chai-js",
    ],
)

js_test(
    name = "hello-ts-test",
    main = [
        "hello-ts-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
    ],
)

java_binary(
    name = "hello-java",
    srcs = [
        "HelloJava.java",
    ],
    main_class = "HelloJava",
    target_compatible_with = selects.with_or({
        (
            "//bzl/conditions:android",
            "//bzl/conditions:ios",
            "//bzl/conditions:wasm",
        ): ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "example-methods",
    srcs = [
        "example-methods.cc",
    ],
    hdrs = ["example-methods.h"],
    visibility = ["//bzl/examples:__subpackages__"],
)

# This is an example of an explicit WebAssembly rule. Use this if you need to
# define a wasm binary that needs custom emscripten flags. If not, you can build
# and cc_binary or cc_test implicitly for wasm with --config=wasm.
wasm_binary(
    name = "hello-wasm",
    cc_binary = ":hello",
    emopts = emoptsAllowMemoryGrowth(),
    html = 0,
    single_file = 0,
)

# This is an example of an explicit WebAssembly test. Use this if you need to
# define a wasm test that needs custom emscripten flags. If not, you can build
# and cc_binary or cc_test implicitly for wasm with --config=wasm.
wasm_test(
    name = "hello-wasm-test",
    testonly = 1,
    cc_test = ":hello-cc-test",
    emopts = emoptsAllowMemoryGrowth() + [
        "EXIT_RUNTIME=1",
    ],
    html = 0,
    single_file = 0,
)

# Example of exporting certain methods for public dynamic libraries.  These
# methods use C8_PUBLIC attributes to indicate that they are exported symbols
# and the target has alwayslink=1 to indicate that the symbols should be
# included even if they are not referenced. Since alwayslink=1 is would force
# these symbols in other code that doesn't need them as public symbols, we use
# a visibility group to allow this to only be included in specific DLL targets.
cc_library(
    name = "export-methods",
    srcs = [
        "export-methods.cc",
    ],
    hdrs = ["export-methods.h"],
    visibility = [":export-methods-dlls"],
    deps = [
        ":example-methods",
        "//bzl/macro:visibility",
    ],
    alwayslink = 1,
)

node_addon(
    name = "example-node-addon",
    srcs = [
        "node-api-example-methods.cc",
    ],
    linkshared = 1,
    linkstatic = 1,
    target_compatible_with = select({
        "//bzl/conditions:apple": [],
        "//bzl/conditions:windows": [],
        "//bzl/conditions:linux": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":example-methods",
        "//bzl/node:node-addons",
    ],
)

# Restrict export-methods to these specific packages with dynamic libraries.
package_group(
    name = "export-methods-dlls",
    packages = [
        "//bzl/examples/unity",
        "//bzl/examples/windows",
    ],
)

cc_library(
    name = "rust-ffi-clib",
    hdrs = ["rust-ffi.h"],
)
