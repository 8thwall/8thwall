load("//bzl/shader:shader.bzl", "shader_binary", "shader_include")
load("//bzl/utils:embedded.bzl", "embedded_data")

package(
    default_visibility = [
        "//c8/pixels/render:__subpackages__",
        "//c8/scan:__subpackages__",
    ],
)

embedded_data(
    name = "render-shaders",
    srcs = [
        ":color-only-frag",
        ":color-only-vert",
        ":cubemap-frag",
        ":cubemap-vert",
        ":depth-frag",
        ":depth-vert",
        ":image-frag",
        ":image-vert",
        ":occlusion-frag",
        ":occlusion-vert",
        ":physical-frag",
        ":physical-vert",
        ":point-cloud-color-only-frag",
        ":point-cloud-color-only-vert",
        ":point-cloud-physical-frag",
        ":point-cloud-physical-vert",
        ":splat-attributes-vert",
        ":splat-frag",
        ":splat-multitex-final-vert",
        ":splat-multitex-noop-vert",
        ":splat-multitex-sh-frag",
        ":splat-sorted-texture-vert",
        ":splat-texture-stacked-vert",
        ":splat-texture-vert",
    ],
)

shader_binary(
    name = "color-only-frag",
    srcs = [
        "color-only.frag",
    ],
)

shader_binary(
    name = "color-only-vert",
    srcs = [
        "color-only.vert",
    ],
    deps = [
        ":geometry",
    ],
)

shader_binary(
    name = "physical-frag",
    srcs = [
        "physical.frag",
    ],
    deps = [
        ":occlusion-frag",
        ":physical-lighting-frag",
    ],
)

shader_binary(
    name = "physical-vert",
    srcs = [
        "physical.vert",
    ],
    deps = [
        ":geometry",
        ":occlusion-vert",
        ":physical-lighting-vert",
    ],
)

shader_binary(
    name = "splat-frag",
    srcs = [
        "splat.frag",
    ],
)

shader_include(
    name = "splat-vert",
    srcs = [
        "splat-vert.glsl",
    ],
    visibility = ["//visibility:public"],
)

shader_binary(
    name = "splat-attributes-vert",
    srcs = [
        "splat-attributes.vert",
    ],
    deps = [
        ":splat-vert",
    ],
)

SHADER_NO_WEBGL1_TARGETS = select({
    "//bzl/conditions:wasm": ["webgl2"],  # No WebGL 1
    "//c8/pixels/opengl:with-angle": ["glsl300es"],
    "//bzl/shader:osx-without-angle": ["glsl150"],
    "//conditions:default": ["glsl300es"],
})

shader_binary(
    name = "splat-texture-vert",
    srcs = [
        "splat-texture.vert",
    ],
    targets = SHADER_NO_WEBGL1_TARGETS,
    deps = [
        ":splat-vert",
    ],
)

shader_binary(
    name = "splat-texture-stacked-vert",
    srcs = [
        "splat-texture-stacked.vert",
    ],
    targets = SHADER_NO_WEBGL1_TARGETS,
    deps = [
        ":splat-vert",
    ],
)

shader_binary(
    name = "splat-multitex-final-vert",
    srcs = [
        "splat-multitex-final.vert",
    ],
    targets = SHADER_NO_WEBGL1_TARGETS,
    deps = [
        ":splat-vert",
    ],
)

shader_binary(
    name = "splat-multitex-noop-vert",
    srcs = [
        "splat-multitex-noop.vert",
    ],
    targets = SHADER_NO_WEBGL1_TARGETS,
    deps = [
        ":splat-vert",
    ],
)

shader_binary(
    name = "splat-multitex-sh-frag",
    srcs = [
        "splat-multitex-sh.frag",
    ],
    targets = SHADER_NO_WEBGL1_TARGETS,
    deps = [
        ":splat-vert",
    ],
)

shader_binary(
    name = "splat-sorted-texture-vert",
    srcs = [
        "splat-sorted-texture.vert",
    ],
    targets = SHADER_NO_WEBGL1_TARGETS,
    deps = [
        ":splat-vert",
    ],
)

shader_binary(
    name = "image-frag",
    srcs = [
        "image.frag",
    ],
)

shader_binary(
    name = "image-vert",
    srcs = [
        "image.vert",
    ],
    deps = [
        ":geometry",
    ],
)

shader_binary(
    name = "point-cloud-color-only-frag",
    srcs = [
        "point-cloud-color-only.frag",
    ],
)

shader_binary(
    name = "point-cloud-color-only-vert",
    srcs = [
        "point-cloud-color-only.vert",
    ],
    deps = [
        ":geometry",
    ],
)

shader_binary(
    name = "point-cloud-physical-frag",
    srcs = [
        "point-cloud-physical.frag",
    ],
    deps = [
        ":occlusion-frag",
        ":physical-lighting-frag",
    ],
)

shader_binary(
    name = "point-cloud-physical-vert",
    srcs = [
        "point-cloud-physical.vert",
    ],
    deps = [
        ":geometry",
        ":occlusion-vert",
        ":physical-lighting-vert",
    ],
)

shader_include(
    name = "physical-lighting-frag",
    srcs = [
        "physical-lighting-frag.glsl",
    ],
    visibility = [
        "//c8/pixels/render:__subpackages__",
        "//c8/scan:__subpackages__",
    ],
)

shader_include(
    name = "physical-lighting-vert",
    srcs = [
        "physical-lighting-vert.glsl",
    ],
    visibility = [
        "//c8/pixels/render:__subpackages__",
        "//c8/scan:__subpackages__",
    ],
)

shader_binary(
    name = "depth-frag",
    srcs = [
        "depth.frag",
    ],
)

shader_binary(
    name = "depth-vert",
    srcs = [
        "depth.vert",
    ],
    deps = [
        ":geometry",
    ],
)

shader_binary(
    name = "cubemap-frag",
    srcs = [
        "cubemap.frag",
    ],
)

shader_binary(
    name = "cubemap-vert",
    srcs = [
        "cubemap.vert",
    ],
    deps = [
        ":geometry",
    ],
)

shader_include(
    name = "geometry",
    srcs = [
        "geometry.glsl",
    ],
    visibility = [
        "//c8/pixels/render:__subpackages__",
        "//c8/scan:__subpackages__",
    ],
)

shader_include(
    name = "occlusion-frag",
    srcs = [
        "occlusion-frag.glsl",
    ],
    visibility = [
        "//c8/pixels/render:__subpackages__",
        "//c8/scan:__subpackages__",
    ],
)

shader_include(
    name = "occlusion-vert",
    srcs = [
        "occlusion-vert.glsl",
    ],
    visibility = [
        "//c8/pixels/render:__subpackages__",
        "//c8/scan:__subpackages__",
    ],
)
