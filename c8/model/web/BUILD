load("//bzl/wasm:wasm.bzl", "wasm_binary")
load("//bzl/js:js.bzl", "js_binary", "js_library")
load("//bzl/crosstool:wasm-pthread.bzl", "emoptsAllowMemoryGrowth")
load("//bzl/shader:shader.bzl", "shader_binary")
load("//bzl/js:js-stringloader.bzl", "js_stringloader")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "aframe-model-manager",
    srcs = [
        "aframe-model-manager.ts",
    ],
    deps = [
        ":threejs-model-manager",
    ],
)

js_library(
    name = "internal-config",
    srcs = [
        "internal-config.ts",
    ],
    deps = [],
)

js_library(
    name = "threejs-model-manager",
    srcs = [
        "bake-cubemap.ts",
        "threejs-model-manager.ts",
    ],
    deps = [
        ":model-manager",
        ":splat-shaders",
    ],
)

js_library(
    name = "model-manager-types",
    srcs = [
        "model-manager-types.d.ts",
    ],
    deps = [
    ],
)

# Worker script
js_binary(
    name = "model-manager-worker",
    main = [
        "model-manager-worker.ts",
    ],
    npm_rule = "@npm-c8-model-web//:npm-c8-model-web",
    deps = [
        ":model-manager-types",
        ":model-manager-wasm",
        "//c8/ems",
    ],
)

js_binary(
    name = "model-standalone",
    main = [
        "model-standalone.js",
    ],
    npm_rule = "@npm-c8-model-web//:npm-c8-model-web",
    deps = [
        ":model",
    ],
)

js_binary(
    name = "model-min",
    export_library = True,
    main = [
        "model.ts",
    ],
    npm_rule = "@npm-c8-model-web//:npm-c8-model-web",
    deps = [
        ":aframe-model-manager",
        ":internal-config",
        ":model-manager",
        ":threejs-model-manager",
        ":web-model-view",
    ],
)

js_library(
    name = "model",
    srcs = [
        "model.ts",
    ],
    deps = [
        ":aframe-model-manager",
        ":internal-config",
        ":model-manager",
        ":threejs-model-manager",
        ":web-model-view",
    ],
)

# Front end to calling ModelManagerWorker
js_library(
    name = "model-manager",
    srcs = [
        "model-manager.ts",
    ],
    deps = [
        ":internal-config",
        ":model-manager-types",
    ],
)

wasm_binary(
    name = "model-manager-wasm",
    cc_binary = ":model-manager-worker-cc",
    embind = 1,
    emopts = emoptsAllowMemoryGrowth() + [
        "EXPORTED_FUNCTIONS=[_malloc,_free]",
        "EXPORTED_RUNTIME_METHODS=[GL,writeArrayToMemory,stringToUTF8,lengthBytesUTF8]",
        "EXPORT_ES6=1",
        "EXPORT_NAME=ModelManagerWasm",
        "MODULARIZE=1",
        "TOTAL_MEMORY=128mb",  # Must be a multiple of 16MB.
        "USE_ES6_IMPORT_META=0",
        "USE_WEBGL2=1",
    ],
    html = 0,
    single_file = 1,
)

cc_binary(
    name = "model-manager-worker-cc",
    srcs = [
        "model-manager-worker.cc",
    ],
    target_compatible_with = select({
        "//bzl/conditions:wasm": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":constants",
        "//c8:hpoint",
        "//c8:quaternion",
        "//c8:symbol-visibility",
        "//c8/model:model-manager",
        "//c8/stats:scope-timer",
        "@json",
    ],
)

js_stringloader(
    name = "splat-shaders",
    srcs = [
        ":splat-frag",
        ":splat-instance-attributes-vert",
        ":splat-multi-texture-vert",
        ":splat-texture-vert",
    ],
)

shader_binary(
    name = "splat-frag",
    srcs = [
        "splat.frag",
    ],
    targets = ["webgl2"],
)

shader_binary(
    name = "splat-instance-attributes-vert",
    srcs = [
        "splat-instance-attributes.vert",
    ],
    targets = ["webgl2"],
    deps = [
        "//c8/pixels/render/shaders:splat-vert",
    ],
)

shader_binary(
    name = "splat-texture-vert",
    srcs = [
        "splat-texture.vert",
    ],
    targets = ["webgl2"],
    deps = [
        "//c8/pixels/render/shaders:splat-vert",
    ],
)

shader_binary(
    name = "splat-multi-texture-vert",
    srcs = [
        "splat-multi-texture.vert",
    ],
    targets = ["webgl2"],
    deps = [
        "//c8/pixels/render/shaders:splat-vert",
    ],
)

js_library(
    name = "web-model-view",
    srcs = [
        "web-model-view.ts",
    ],
    deps = [
        ":model-manager",
        ":web-model-view-wasm",
        "//c8/browser:mouse-to-touch-translater",
        "//c8/ems",
        "//c8/xrapi:xrapi-types",
    ],
)

wasm_binary(
    name = "web-model-view-wasm",
    cc_binary = ":web-model-view-cc",
    embind = 1,
    emopts = emoptsAllowMemoryGrowth() + [
        "EXPORTED_FUNCTIONS=[_malloc,_free]",
        "EXPORTED_RUNTIME_METHODS=[GL,writeArrayToMemory,stringToUTF8,lengthBytesUTF8]",
        "EXPORT_ES6=1",
        "EXPORT_NAME=MODELVIEW",
        "MODULARIZE=1",
        "TOTAL_MEMORY=128mb",  # Must be a multiple of 16MB.
        "USE_ES6_IMPORT_META=0",
        "USE_WEBGL2=1",
    ],
    html = 0,
    single_file = 1,
)

cc_binary(
    name = "web-model-view-cc",
    srcs = [
        "web-model-view.cc",
    ],
    target_compatible_with = select({
        "//bzl/conditions:wasm": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":constants",
        "//bzl/inliner:rules2",
        "//c8:c8-log",
        "//c8:symbol-visibility",
        "//c8/model:model-view",
        "//c8/pixels/render:gesture-detector",
        "//c8/time:now",
        "//c8/time:rolling-framerate",
        "@json",
    ],
)

# START_AUTOGENERATED_RULES
# Do not edit these rules manually, use inliner.

# file: constants.cc
cc_library(
    name = "constants",
    srcs = [
        "constants.cc",
    ],
    hdrs = [
        "constants.h",
    ],
    deps = [
        "//bzl/inliner:rules2",
    ],
)
# cc_end(0xec808e9b)
# END_AUTOGENERATED_RULES
