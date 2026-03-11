load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//reality/cloud:shared"])

js_library(
    name = "base",
    srcs = [
        "base.ts",
    ],
)

js_library(
    name = "image-to-image",
    srcs = [
        "image-to-image.ts",
    ],
    deps = [
        "//reality/cloud/xrhome/src/shared/genai/types:base",
    ],
)

js_library(
    name = "image-to-mesh",
    srcs = [
        "image-to-mesh.ts",
    ],
    deps = [
        "//reality/cloud/xrhome/src/shared/genai/types:base",
    ],
)

js_library(
    name = "mesh-to-animation",
    srcs = [
        "mesh-to-animation.ts",
    ],
    deps = [
        "//reality/cloud/xrhome/src/shared/genai/types:base",
    ],
)

js_library(
    name = "text-to-image",
    srcs = [
        "text-to-image.ts",
    ],
    deps = [
        "//reality/cloud/xrhome/src/shared/genai/types:base",
    ],
)

js_library(
    name = "generate",
    srcs = [
        "generate.ts",
    ],
    deps = [
        "//reality/cloud/xrhome/src/shared/genai/types:image-to-image",
        "//reality/cloud/xrhome/src/shared/genai/types:image-to-mesh",
        "//reality/cloud/xrhome/src/shared/genai/types:mesh-to-animation",
        "//reality/cloud/xrhome/src/shared/genai/types:text-to-image",
    ],
)
