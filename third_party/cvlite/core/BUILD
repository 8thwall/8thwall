licenses(["permissive"])  # BSD 3-clause

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "core",
    srcs = glob([
        "**/*.cpp",
        "**/*.hpp",
        "**/*.h",
        "**/*.hpp",
    ]) + [
        ":custom_hal",
        ":cvconfig",
        ":opencl_kernels_core",
        ":version_string",
    ],
    hdrs = [
        "core.hpp",
    ] + [
        ":opencv_modules",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        "-fexceptions",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:android": [
            "-llog",
            "-lz",
        ],
        "@the8thwall//bzl/conditions:windows": [],
        "@the8thwall//bzl/conditions:v1-linux": [],
        "@the8thwall//bzl/conditions:wasm": ["-s USE_ZLIB=1"],
        "//conditions:default": ["-lz"],
    }),
    visibility = ["//visibility:public"],
    deps = select({
        "@the8thwall//bzl/conditions:wasm": ["@zlib//:zlib"],
        "@the8thwall//bzl/conditions:windows": ["@zlib//:zlib"],
        "@the8thwall//bzl/conditions:v1-linux": ["@zlib//:zlib"],
        "//conditions:default": [],
    }),
)

genrule(
    name = "custom_hal",
    outs = ["custom_hal.hpp"],
    cmd = "touch $@",
)

genrule(
    name = "cvconfig",
    outs = ["cvconfig.h"],
    cmd = """
        echo '#ifdef C8_OPENCV_OSX_ONLY' >> $@
        echo '  #define HAVE_AVFOUNDATION' >> $@
        echo '  #define HAVE_COCOA' >> $@
        echo '#endif // ifdef C8_OPENCV_OSX_ONLY' >> $@
    """,
)

genrule(
    name = "opencl_kernels_core",
    outs = ["opencl_kernels_core.hpp"],
    cmd = """
      echo '#include "third_party/cvlite/core/ocl.hpp"' >> $@
      echo '#include "third_party/cvlite/core/ocl_genbase.hpp"' >> $@
      echo '#include "third_party/cvlite/core/opencl/ocl_defs.hpp"' >> $@
    """,
)

genrule(
    name = "opencv_modules",
    outs = ["opencv_modules.hpp"],
    cmd = """
        echo '#define HAVE_OPENCV_CALIB3D' >> $@
        echo '#define HAVE_OPENCV_CORE' >> $@
        echo '#define HAVE_OPENCV_FLANN' >> $@
        echo '#define HAVE_OPENCV_FEATURES2D' >> $@
        echo '#define HAVE_OPENCV_IMGCODECS' >> $@
        echo '#define HAVE_OPENCV_IMGPROC' >> $@
        echo '#define HAVE_OPENCV_ML' >> $@
        echo '#ifdef C8_OPENCV_OSX_ONLY' >> $@
        echo '  #define HAVE_OPENCV_HIGHGUI' >> $@
        echo '  #define HAVE_OPENCV_VIDEOIO' >> $@
        echo '#endif  // C8_OPENCV_OSX_ONLY' >> $@
    """,
)

genrule(
    name = "version_string",
    outs = ["version_string.inc"],
    cmd = """
        echo '\"OpenCV 3.2.0\"' >> $@
    """,
)
