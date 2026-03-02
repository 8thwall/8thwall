load("@the8thwall//bzl/apple:objc.bzl", "nia_objc_library")

cc_library(
    name = "inc",
    srcs = [],
    hdrs = [
        "include/opencv2/opencv.hpp",
        ":opencv_modules",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        "-std=c++14",
        "-fexceptions",
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":calib3d",
        ":core",
        ":features2d",
        ":flann",
        ":highgui",
        ":imgcodecs",
        ":imgproc",
        ":ml",
    ],
)

cc_library(
    name = "calib3d",
    srcs = glob([
        "modules/calib3d/src/**/*.cpp",
        "modules/calib3d/src/**/*.h",
        "modules/calib3d/src/**/*.hpp",
        "modules/calib3d/include/**/*.hpp",
        "modules/calib3d/include/**/*.h",
    ]) + [
        ":opencl_kernels_calib3d",
    ],
    hdrs = [
        "modules/calib3d/include/opencv2/calib3d/calib3d.hpp",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        "-fexceptions",
    ],
    includes = [
        "modules/calib3d/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":features2d",
        ":imgproc",
    ],
)

cc_library(
    name = "core",
    srcs = glob([
        "modules/core/src/**/*.cpp",
        "modules/core/src/**/*.hpp",
        "modules/core/include/**/*.h",
        "modules/core/include/**/*.hpp",
    ]) + [
        ":custom_hal",
        ":cvconfig",
        ":opencl_kernels_core",
        ":version_string",
    ],
    hdrs = [
        "modules/core/include/opencv2/core/core.hpp",
    ] + [
        ":opencv_modules",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        "-fexceptions",
    ],
    includes = [
        "",
        "modules/core/include",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:linux": [],
        "//conditions:default": ["-lz"],
    }) + select({
        "@platforms//os:android": ["-llog"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "@zlib",
    ],
)

cc_library(
    name = "features2d",
    srcs = glob([
        "modules/features2d/src/**/*.cpp",
        "modules/features2d/src/**/*.h",
        "modules/features2d/src/**/*.hpp",
        "modules/features2d/include/**/*.hpp",
        "modules/features2d/include/**/*.h",
    ]) + [
        ":opencl_kernels_features2d",
    ],
    hdrs = [
        "modules/features2d/include/opencv2/features2d/features2d.hpp",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    includes = [
        "modules/features2d/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":flann",
        ":imgproc",
        ":ml",
    ],
)

cc_library(
    name = "flann",
    srcs = glob([
        "modules/flann/src/**/*.cpp",
        "modules/flann/src/**/*.hpp",
        "modules/flann/include/**/*.h",
        "modules/flann/include/**/*.hpp",
    ]),
    hdrs = [
        "modules/flann/include/opencv2/flann/flann.hpp",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        # Build flann with older C++, since it relies on removed method random_shuffle.
        "-std=c++14",
        "-fexceptions",
    ],
    includes = [
        "modules/flann/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
    ],
)

cc_library(
    name = "highgui",
    srcs = glob(
        [
            "modules/highgui/src/**/*.cpp",
            "modules/highgui/src/**/*.hpp",
            "modules/highgui/include/**/*.hpp",
            "modules/highgui/include/**/*.h",
        ],
        exclude = [
            "modules/highgui/src/window_winrt_bridge.cpp",
            "modules/highgui/src/window_carbon.cpp",
            "modules/highgui/src/window_winrt.cpp",
            "modules/highgui/src/window_w32.cpp",
            "modules/highgui/src/window_QT.cpp",
            "modules/highgui/src/window_gtk.cpp",
        ],
    ) + select({
        "@the8thwall//bzl/conditions:linux": [],
        "//conditions:default": [":cvwindowcocoa"],
    }),
    hdrs = [
        "modules/highgui/include/opencv2/highgui.hpp",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        # Currently we only support highgui visualization through OSX Mac
        # Libraries. If we want to support more platforms, we can consider doing
        # it with crosstool flags and platform conditional dependencies.
        "-DC8_OPENCV_OSX_ONLY",
    ],
    includes = [
        "modules/highgui/include",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:linux": [],
        # This is required, otherwise use of this target on macOS
        # leads to the following runtime error: "dyld[...]: symbol not found in flat namespace '_NSAppKitVersionNumber'"
        #
        # TODO(peter) This linkopt would ideally be added to the ":cvwindowcocoa"
        # target rather than here, because the Obj-C++ files in that target
        # directly make use of 'NSAppKitVersionNumber'. However, adding this
        # linkopt to the 'nia_objc_library()' did not address this issue. Figure
        # out in a future task why this linkopt doesn't work there.
        "//conditions:default": ["-framework AppKit"],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":imgcodecs",
        ":videoio",
    ],
)

cc_library(
    name = "imgcodecs",
    srcs = glob([
        "modules/imgcodecs/src/**/*.cpp",
        "modules/imgcodecs/src/**/*.hpp",
        "modules/imgcodecs/include/**/*.hpp",
        "modules/imgcodecs/include/**/*.h",
    ]),
    hdrs = [
        "modules/imgcodecs/include/opencv2/imgcodecs/imgcodecs.hpp",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        # Currently we only support the Image Codecs library through OSX Mac
        # Libraries. If we want to support more platforms, we can consider doing
        # it with crosstool flags and platform conditional dependencies.
        "-DC8_OPENCV_OSX_ONLY",
        "-DHAVE_PNG",
    ],
    includes = [
        "modules/imgcodecs/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":imgproc",
        "@png",
        "@zlib",
    ],
)

cc_library(
    name = "imgproc",
    srcs = glob([
        "modules/imgproc/src/**/*.cpp",
        "modules/imgproc/src/**/*.hpp",
        "modules/imgproc/src/**/*.h",
        "modules/imgproc/include/**/*.hpp",
        "modules/imgproc/include/**/*.h",
    ]) + [
        ":opencl_kernels_imgproc",
    ],
    hdrs = [
        "modules/imgproc/include/opencv2/imgproc/imgproc.hpp",
    ],
    copts = [
        "-D__OPENCV_BUILD",
        # Build imgproc with older C++, since it relies on removed method mem_fun_ref.
        "-std=c++14",
        "-fexceptions",
    ],
    includes = [
        "modules/imgproc/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
    ],
)

cc_library(
    name = "ml",
    srcs = glob([
        "modules/ml/src/**/*.cpp",
        "modules/ml/src/**/*.hpp",
        "modules/ml/include/**/*.h",
        "modules/ml/include/**/*.hpp",
    ]),
    hdrs = [
        "modules/ml/include/opencv2/ml/ml.hpp",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    includes = [
        "modules/ml/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
    ],
)

cc_library(
    name = "videoio",
    srcs = glob(
        [
            "modules/videoio/src/**/*.cpp",
            "modules/videoio/src/**/*.hpp",
            "modules/videoio/include/**/*.hpp",
            "modules/videoio/include/**/*.h",
        ],
        exclude = [
            "modules/videoio/src/cap_giganetix.cpp",
            "modules/videoio/src/cap_gstreamer.cpp",
            "modules/videoio/src/cap_qt.cpp",
            "modules/videoio/src/cap_unicap.cpp",
            "modules/videoio/src/cap_vfw.cpp",
            "modules/videoio/src/cap_winrt/**/*",
            "modules/videoio/src/cap_winrt_bridge.cpp",
            "modules/videoio/src/cap_winrt_capture.cpp",
            "modules/videoio/src/cap_winrt_video.cpp",
            "modules/videoio/src/cap_ximea.cpp",
            "modules/videoio/src/cap_xine.cpp",
        ],
    ) + select({
        "@the8thwall//bzl/conditions:linux": [],
        "//conditions:default": [":cvcapavfoundation"],
    }),
    copts = [
        "-D__OPENCV_BUILD",
        # Currently we only support VideoIO Camera Capture through OSX Mac
        # Libraries. If we want to support more platforms, we can consider doing
        # it with crosstool flags and platform conditional dependencies.
        "-DC8_OPENCV_OSX_ONLY",
    ],
    includes = [
        "modules/videoio/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":imgcodecs",
        ":imgproc",
    ],
)

#NOTE(dat): zlibconf.h.cmakein is expanded into zlibconf.h manually already
cc_library(
    name = "3rdparty_zlib",
    srcs = glob(["3rdparty/zlib/*.c"]),
    hdrs = glob(["3rdparty/zlib/*.h"]),
    includes = ["3rdparty/zlib"],
    visibility = ["//visibility:private"],
)

nia_objc_library(
    name = "cvcapavfoundation",
    srcs = [
        "modules/videoio/src/cap_avfoundation_mac.mm",
        "modules/videoio/src/precomp.hpp",
    ] + glob([
        "modules/videoio/include/**/*.h",
        "modules/videoio/include/**/*.hpp",
    ]),
    copts = [
        "-Iexternal/opencv/modules/videoio/include",
        "-D__OPENCV_BUILD",
        "-fno-objc-arc",
    ],
    deps = [
        ":core",
        ":imgcodecs",
        ":imgproc",
    ],
)

nia_objc_library(
    name = "cvwindowcocoa",
    srcs = [
        "modules/highgui/src/window_cocoa.mm",
        "modules/highgui/src/precomp.hpp",
    ] + glob([
        "modules/highgui/include/**/*.h",
        "modules/highgui/include/**/*.hpp",
    ]),
    copts = [
        "-Iexternal/opencv/modules/highgui/include",
        "-D__OPENCV_BUILD",
        "-fno-objc-arc",
    ],
    deps = [
        ":core",
        ":imgcodecs",
        ":imgproc",
    ],
)

genrule(
    name = "cvvizcocoainteractor",
    srcs = [
        "modules/viz/src/vtk/vtkCocoaInteractorFix.mm",
    ],
    outs = [
        "libcvvizcocoainteractor.o",
    ],
    cmd = """
SDKROOT=`xcrun --sdk macosx --show-sdk-path` \
xcrun clang -c $(location modules/viz/src/vtk/vtkCocoaInteractorFix.mm) \
-o $(@) \
-I "/usr/local/opt/vtk/include/vtk-8.0" \
-I "/usr/local/opt/vtk/include/vtk-8.1"
""",
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
    name = "opencl_kernels_calib3d",
    outs = ["opencl_kernels_calib3d.hpp"],
    cmd = """
      echo '#include "opencv2/core/ocl.hpp"' >> $@
      echo '#include "opencv2/core/ocl_genbase.hpp"' >> $@
      echo '#include "opencv2/core/opencl/ocl_defs.hpp"' >> $@
    """,
)

genrule(
    name = "opencl_kernels_core",
    outs = ["opencl_kernels_core.hpp"],
    cmd = """
      echo '#include "opencv2/core/ocl.hpp"' >> $@
      echo '#include "opencv2/core/ocl_genbase.hpp"' >> $@
      echo '#include "opencv2/core/opencl/ocl_defs.hpp"' >> $@
    """,
)

genrule(
    name = "opencl_kernels_imgproc",
    outs = ["opencl_kernels_imgproc.hpp"],
    cmd = """
      echo '#include "opencv2/core/ocl.hpp"' >> $@
      echo '#include "opencv2/core/ocl_genbase.hpp"' >> $@
      echo '#include "opencv2/core/opencl/ocl_defs.hpp"' >> $@
    """,
)

genrule(
    name = "opencl_kernels_features2d",
    outs = ["opencl_kernels_features2d.hpp"],
    cmd = """
      echo '#include "opencv2/core/ocl.hpp"' >> $@
      echo '#include "opencv2/core/ocl_genbase.hpp"' >> $@
      echo '#include "opencv2/core/opencl/ocl_defs.hpp"' >> $@
    """,
)

genrule(
    name = "opencv_modules",
    outs = ["opencv2/opencv_modules.hpp"],
    cmd = """
        echo '#define HAVE_OPENCV_CALIB3D' >> $@
        echo '#define HAVE_OPENCV_CORE' >> $@
        echo '#ifndef __MAPPING_BAZEL_BUILD__' >> $@
        echo '#define HAVE_OPENCV_FLANN' >> $@
        echo '#endif // __MAPPING_BAZEL_BUILD__' >> $@
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
