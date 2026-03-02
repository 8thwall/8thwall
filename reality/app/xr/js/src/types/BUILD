load("//bzl/js:js.bzl", "js_library")
load("@aspect_bazel_lib//lib:write_source_files.bzl", "write_source_files")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "callbacks",
    srcs = [
        "callbacks.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/shared/engine:image-targets",
    ],
)

js_library(
    name = "common",
    srcs = [
        "common.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:quaternion",
    ],
)

js_library(
    name = "emscripten",
    srcs = [
        "emscripten.ts",
    ],
)

js_library(
    name = "environment-update",
    srcs = [
        "environment-update.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:device",
    ],
)

js_library(
    name = "layers",
    srcs = [
        "layers.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:common",
    ],
)

js_library(
    name = "module",
    srcs = [
        "module.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:callbacks",
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:pipeline",
    ],
)

js_library(
    name = "pipeline",
    srcs = [
        "pipeline.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:emscripten",
        "//reality/app/xr/js/src/types:environment-update",
    ],
)

js_library(
    name = "ua-parser",
    srcs = [
        "ua-parser.ts",
    ],
)

write_source_files(
    name = "write",
    files = {
        "xrmodule.d.ts": ":xrmodule-generated",
        "xrcc.ts": ":xrcc-generated",
        "xrfacecc.ts": ":xrfacecc-generated",
        "xrtrackingcc.ts": ":xrtrackingcc-generated",
    },
    suggested_update_target = "write --config=wasm",
)

js_library(
    name = "xrmodule",
    srcs = [
        ":xrmodule.d.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:emscripten",
    ],
)

genrule(
    name = "xrmodule-generated",
    srcs = [
        "//reality/app/xr/js:xr-module-cc-srcs",
    ],
    outs = ["_xrmodule.d.ts"],
    cmd = "$(location //reality/app/xr/js/tools:generate-xrmodule-types) $(SRCS) > $@",
    tools = ["//reality/app/xr/js/tools:generate-xrmodule-types"],
)

js_library(
    name = "xrcc",
    srcs = [
        ":xrcc.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:xrmodule",
    ],
)

genrule(
    name = "xrcc-generated",
    srcs = [
        "//reality/app/xr/js:xr-js-cc-srcs",
    ],
    outs = ["_xrcc.ts"],
    cmd = "$(location //reality/app/xr/js/tools:generate-xrcc-types) $(SRCS) > $@",
    tools = ["//reality/app/xr/js/tools:generate-xrcc-types"],
)

js_library(
    name = "xrfacecc",
    srcs = [
        ":xrfacecc.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:xrmodule",
    ],
)

genrule(
    name = "xrfacecc-generated",
    srcs = [
        "//reality/app/xr/js:xrcc-face-srcs",
    ],
    outs = ["_xrfacecc.ts"],
    cmd = "$(location //reality/app/xr/js/tools:generate-xrchunk-types) XrFacecc $(SRCS) > $@",
    tools = ["//reality/app/xr/js/tools:generate-xrchunk-types"],
)

js_library(
    name = "xrtrackingcc",
    srcs = [
        ":xrtrackingcc.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:xrmodule",
    ],
)

genrule(
    name = "xrtrackingcc-generated",
    srcs = [
        "//reality/app/xr/js:xrcc-tracking-srcs",
    ],
    outs = ["_xrtrackingcc.ts"],
    cmd = "$(location //reality/app/xr/js/tools:generate-xrchunk-types) XrTrackingcc $(SRCS) > $@",
    tools = ["//reality/app/xr/js/tools:generate-xrchunk-types"],
)

js_library(
    name = "api",
    srcs = [
        "api.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:device",
        "//reality/app/xr/js/src/types:emscripten",
        "//reality/app/xr/js/src/types:module",
        "//reality/app/xr/js/src/types:pipeline",
    ],
)

js_library(
    name = "device",
    srcs = [
        "device.ts",
    ],
)

js_library(
    name = "recorder",
    srcs = [
        "recorder.ts",
    ],
)
