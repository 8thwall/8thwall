load("//bzl/shader:shader.bzl", "shader_binary", "shader_include")

shader_include(
    name = "shared",
    srcs = [
        "shared.glsl",
    ],
)

shader_binary(
    name = "hello-shader",
    srcs = [
        "hello-shader.frag",
    ],
    deps = [
        ":shared",
    ],
)
