load("@rules_pkg//pkg:mappings.bzl", "pkg_files")

package(default_visibility = ["//c8/ecs:__subpackages__"])

pkg_files(
    name = "draco",
    srcs = [
        "LICENSE",
        "draco_decoder.js",
        "draco_decoder.wasm",
        "draco_wasm_wrapper.js",
    ],
    prefix = "draco",
)
