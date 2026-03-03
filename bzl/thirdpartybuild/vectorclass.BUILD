licenses(["permissive"])  # Apache 2

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "vectorclass",
    srcs = [
        "instrset.h",
        "instrset_detect.cpp",
        "vector_convert.h",
        "vectorf128.h",
        "vectorf256.h",
        "vectorf256e.h",
        "vectorf512.h",
        "vectorf512e.h",
        "vectorfp16.h",
        "vectorfp16e.h",
        "vectori128.h",
        "vectori256.h",
        "vectori256e.h",
        "vectori512.h",
        "vectori512e.h",
        "vectori512s.h",
        "vectori512se.h",
        "vectormath_common.h",
        "vectormath_exp.h",
        "vectormath_hyp.h",
        "vectormath_lib.h",
        "vectormath_trig.h",
    ],
    hdrs = [
        "vectorclass.h",
    ],
    includes = [
        ".",
    ],
)

# Currently this does not build correctly and should be investigated.
# cc_binary (
#     name = "dispatch_example1",
#     srcs = ["dispatch_example1.cpp",],
#     copts = [
#     	"-march=native",
#     	"-O2",
# 	"-m64",
# 	"-msse2",
# 	"-std=c++17",
#     ],
#     deps = [":vectorclass",],
# )
