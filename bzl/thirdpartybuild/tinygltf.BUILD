licenses(["permissive"])  # MIT

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tinygltf",
    srcs = [
        "tiny_gltf.cc",
    ],
    hdrs = [
        "stb_image.h",
        "stb_image_write.h",
        "tiny_gltf.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@json",
    ],
)
