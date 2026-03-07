load("@rules_pkg//pkg:mappings.bzl", "pkg_files")
load("//bzl/utils:aliased-file.bzl", "aliased_file")
load("//bzl/js:reset-target.bzl", "reset_js_target")

package(default_visibility = ["//c8/ecs:__subpackages__"])

pkg_files(
    name = "splat",
    srcs = [
        ":splat-inner",
    ],
    prefix = "splat",
)

# NOTE(christoph): This ensures we're always building for browser targets
reset_js_target(
    name = "splat-inner",
    deps = [
        ":splat-loader",
        ":splat-worker",
    ],
)

aliased_file(
    name = "splat-loader",
    filename = "splat-loader.js",
    source_filename = "model-standalone.js",
    source_target = "//c8/model/web:model-standalone",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

aliased_file(
    name = "splat-worker",
    filename = "splat-worker.js",
    source_filename = "model-manager-worker.js",
    source_target = "//c8/model/web:model-manager-worker",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)
