cc_library(
    name = "ximgproc",
    srcs = glob([
        "modules/ximgproc/src/*.cpp",
        "modules/ximgproc/src/*.h",
        "modules/ximgproc/src/*.hpp",
        "modules/ximgproc/include/**/*.hpp",
        "modules/ximgproc/include/**/*.h",
    ]),
    hdrs = [
        "modules/ximgproc/include/opencv2/ximgproc.hpp",
    ],
    copts = [
        "-std=c++14",
        "-fexceptions",
    ],
    includes = [
        "modules/ximgproc/include",
    ],
    local_defines = ["__OPENCV_BUILD"],
    visibility = ["//visibility:public"],
    deps = [
        "@opencv//:calib3d",
        "@opencv//:core",
        "@opencv//:highgui",
        "@opencv//:imgproc",
    ],
)
