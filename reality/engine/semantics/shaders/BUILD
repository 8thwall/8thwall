load("//bzl/utils:embedded.bzl", "embedded_data")
load("//bzl/shader:shader.bzl", "shader_binary")

package(
    default_visibility = [
        "//reality/engine/semantics:__subpackages__",
    ],
)

embedded_data(
    name = "render-shaders",
    srcs = [
        ":semantics-gaussian-linear-frag",
        ":semantics-project-frag",
        ":semantics-project-vert",
        ":semantics-sky-frag",
        ":semantics-sky-vert",
        ":texture-project-frag",
        ":texture-project-vert",
    ],
)

shader_binary(
    name = "texture-project-frag",
    srcs = [
        "texture-project.frag",
    ],
)

shader_binary(
    name = "texture-project-vert",
    srcs = [
        "texture-project.vert",
    ],
)

shader_binary(
    name = "semantics-project-frag",
    srcs = [
        "semantics-project.frag",
    ],
)

shader_binary(
    name = "semantics-project-vert",
    srcs = [
        "semantics-project.vert",
    ],
)

shader_binary(
    name = "semantics-sky-frag",
    srcs = [
        "semantics-sky.frag",
    ],
)

shader_binary(
    name = "semantics-sky-vert",
    srcs = [
        "semantics-sky.vert",
    ],
)

shader_binary(
    name = "semantics-gaussian-linear-frag",
    srcs = [
        "semantics-gaussian-linear.frag",
    ],
)
