load("//bzl/js:js.bzl", "js_binary", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "options",
    srcs = [
        "options.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:quaternion",
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:layers",
        "//reality/engine/api:semantics.capnp-ts",
        "//reality/engine/api/base:camera-intrinsics.capnp-ts",
    ],
)

js_binary(
    name = "semantics-worker",
    main = [
        "semantics-worker.js",
    ],
    npm_rule = "@npm-capnp-ts//:npm-capnp-ts",
    polyfill = "none",
    deps = [
        "//reality/app/xr/js:semantics-worker-wasm",
        "//reality/app/xr/js/src:resources",
    ],
)
