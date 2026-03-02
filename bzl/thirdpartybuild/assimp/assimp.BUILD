load("@the8thwall//bzl/thirdpartybuild/assimp:assimp.bzl", "assimp_cc_library")

licenses(["notice"])  # Ref: https://github.com/assimp/assimp/blob/v5.2.2/LICENSE

package(default_visibility = ["//visibility:public"])

REQUESTED_ASSIMP_FORMAT_TYPES = [
    "FBX",
    "GLTF",
]

assimp_cc_library(
    name = "assimp",
    formats = REQUESTED_ASSIMP_FORMAT_TYPES,
    other_defines = select({
        "@platforms//os:windows": [
            "WIN32_LEAN_AND_MEAN",
            # NOTE: Defining this pre-compilation variable is a bit of an hack
            #       Looking at the original CMakeLists.txt, https://bit.ly/3R06NJz
            #       this is an internal option for compiling only the OpenDDL
            #       parser as a static library. But it seems that we are compiling a single assimp
            #       library with all the thing slapped together in it Which seems to work but
            #       it might causes in the long run, if we have to have further
            #       windows support from the assimp library
            "OPENDDL_STATIC_LIBARY",
        ],
        "//conditions:default": [],
    }),
)
