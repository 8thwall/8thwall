package(default_visibility = ["//visibility:public"])

cc_library(
    name = "assimp",
    srcs = glob(
        [
            "**/*.cpp",
        ],
        exclude = [
            "code/Common/Assimp.cpp",
            "contrib/pugixml/pugixml.cpp",
        ],
    ),
    hdrs = glob([
        "include/*.h",
        "include/assimp/*.h",
        "include/assimp/*.hpp",
        "include/assimp/*.inl",
        "include/assimp/Compiler/*.h",
    ]),
    includes = ["include"],
    textual_hdrs = glob([
        "code/**/*.h",
        "code/**/*.hpp",
        "code/**/*.inl",
        "contrib/**/*.h",
        "contrib/**/*.hpp",
    ]),
)
