load("//bzl/js:js.bzl", "js_library", "js_test")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "compatibility",
    srcs = [
        "compatibility.ts",
    ],
    npm_rule = "@npm-jsxr//:npm-jsxr",
    deps = [
        "//reality/app/xr/js/src/devices:override",
        "//reality/app/xr/js/src/types:common",
        "//reality/engine/api/device:info.capnp-ts",
    ],
)

js_library(
    name = "override",
    srcs = [
        "override.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:canvas",
        "//reality/app/xr/js/src:factory",
        "//reality/app/xr/js/src/devices:getRenderer",
        "//reality/app/xr/js/src/types:pipeline",
    ],
)

js_library(
    name = "fingerprint",
    srcs = [
        "fingerprint.ts",
    ],
)

js_library(
    name = "getRenderer",
    srcs = [
        "getRenderer.js",
    ],
)

js_test(
    name = "override-test",
    main = [
        "override-test.ts",
    ],
    npm_rule = "@npm-jsxr//:npm-jsxr",
    target = "web",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "//bzl/js:chai-js",
        "//reality/app/xr/js/src/devices:override",
    ],
)
