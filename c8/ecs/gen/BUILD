load("@aspect_bazel_lib//lib:write_source_files.bzl", "write_source_files")
load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//c8/ecs:__subpackages__"])

exports_files([
    "runtime-tooltip-data.json",
])

write_source_files(
    name = "write",
    files = {
        "ecs-definition-file.ts": "//c8/ecs/tools:ecs-definition-generated",
        "asm-types.ts": "//c8/ecs/tools:asm-types-generated",
    },
    suggested_update_target = "write --config=wasm",
)

js_library(
    name = "ecs-definition-file",
    srcs = [
        "ecs-definition-file.ts",
    ],
)

js_library(
    name = "asm-types",
    srcs = [
        "asm-types.ts",
    ],
)
