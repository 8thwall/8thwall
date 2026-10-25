licenses(["permissive"])  # MPL 1.1

cc_library(
    name = "mp4v2",
    srcs = glob(
        [
            "src/enum.inl",
            "src/*.h",
            "src/*.cpp",
            "src/**/*.h",
            "src/**/*.cpp",
            "libplatform/*.h",
            "libplatform/*.cpp",
            "libplatform/**/*.h",
            "libplatform/**/*.cpp",
        ],
        exclude = [
            "libplatform/*_posix.cpp",
            "libplatform/**/*_posix.cpp",
            "libplatform/*_win32.cpp",
            "libplatform/**/*_win32.cpp",
        ],
    ) + select({
        "@the8thwall//bzl/conditions:windows": glob([
            "libplatform/*_win32.cpp",
            "libplatform/**/*_win32.cpp",
        ]),
        "//conditions:default": glob([
            "libplatform/*_posix.cpp",
            "libplatform/**/*_posix.cpp",
        ]),
    }),
    hdrs = glob([
        "include/*.h",
        "include/**/*.h",
    ]),
    copts = [
        "-fexceptions",
    ],
    includes = ["include"],
    visibility = ["//visibility:public"],
)
