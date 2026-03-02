load("//bzl/utils:embedded.bzl", "embedded_data")
load("//bzl/shader:shader.bzl", "shader_binary")

package(
    default_visibility = [
        "//c8/model:__subpackages__",
    ],
)

embedded_data(
    name = "model-shaders",
    srcs = [
        "cropbox-cutout-frag",
        "cropbox-cutout-vert",
    ],
)

shader_binary(
    name = "cropbox-cutout-frag",
    srcs = [
        "cropbox-cutout.frag",
    ],
)

shader_binary(
    name = "cropbox-cutout-vert",
    srcs = [
        "cropbox-cutout.vert",
    ],
)
