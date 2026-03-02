load("//bzl/wasm:wasm.bzl", "wasm_binary")
load("//bzl/js:js.bzl", "js_binary")
load("//bzl/crosstool:wasm-pthread.bzl", "emoptsAllowMemoryGrowth")

js_binary(
    name = "hello-emscripten",
    main = [
        "hello.ts",
    ],
    deps = [
        ":hello-emscripten-wasm",
    ],
)

wasm_binary(
    name = "hello-emscripten-wasm",
    cc_binary = ":hello-emscripten-cc",
    embind = 1,
    emopts = emoptsAllowMemoryGrowth() + [
        "EXPORTED_RUNTIME_METHODS=[writeArrayToMemory,stringToUTF8]",
        "EXPORT_ES6=1",
        "EXPORT_NAME=HELLOMODULE",
        "FETCH=1",
        "MODULARIZE=1",
        "TOTAL_MEMORY=96mb",  # Must be a multiple of 16MB.
        "USE_ES6_IMPORT_META=0",
    ],
    html = 0,
    single_file = 1,
)

cc_binary(
    name = "hello-emscripten-cc",
    srcs = [
        "hello.cc",
    ],
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:wasm": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//bzl/inliner:rules2",
        "//c8:symbol-visibility",
    ],
)
