load("//bzl/wasm:wasm.bzl", "wasm_binary")
load("//bzl/js:js.bzl", "js_library")

js_library(
    name = "gesture-detector",
    srcs = [
        "gesture-detector.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":browser-wasm",
        ":mouse-to-touch-translater",
        "//c8/ems",
    ],
)

js_library(
    name = "interaction-provider",
    srcs = [
        "interaction-provider.ts",
    ],
)

js_library(
    name = "mouse-to-touch-translater",
    srcs = [
        "mouse-to-touch-translater.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":interaction-provider",
    ],
)

wasm_binary(
    name = "browser-wasm",
    cc_binary = ":browser-wasm-cc",
    embind = 1,
    emopts = [
        "ALLOW_MEMORY_GROWTH=0",
        "EXPORTED_FUNCTIONS=[_malloc,_free]",
        "EXPORTED_RUNTIME_METHODS=[GL,writeArrayToMemory,stringToUTF8,lengthBytesUTF8]",
        "EXPORT_ES6=1",
        "EXPORT_NAME=MODELVIEW",
        "MODULARIZE=1",
        "TOTAL_MEMORY=16mb",  # Must be a multiple of 16MB.
        "USE_ES6_IMPORT_META=0",
        "USE_WEBGL2=1",
    ],
    html = 0,
    single_file = 1,
)

cc_binary(
    name = "browser-wasm-cc",
    srcs = [
        "browser-wasm.cc",
    ],
    target_compatible_with = select({
        "//bzl/conditions:wasm": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//bzl/inliner:rules2",
        "//c8:c8-log",
        "//c8:map",
        "//c8:set",
        "//c8:symbol-visibility",
        "//c8/pixels/render:gesture-detector",
        "@json",
    ],
)
