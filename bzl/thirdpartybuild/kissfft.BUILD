licenses(["permissive"])  # BSD 3-clause

package(default_visibility = ["//visibility:public"])

# NOTE(dat): Previously tried to enable -DUSE_SIMD and -msse when we are building in SIMD mode
# .          Tried test-real and SIMD version is even slower
cc_library(
    name = "kissfft",
    srcs = [
        "_kiss_fft_guts.h",
        "kfc.c",
        "kfc.h",
        "kiss_fft.c",
        "kiss_fft_log.h",
        "kiss_fftnd.c",
        "kiss_fftndr.c",
        "kiss_fftr.c",
    ],
    hdrs = [
        "kiss_fft.h",
        "kiss_fftnd.h",
        "kiss_fftndr.h",
        "kiss_fftr.h",
        "kissfft.hh",
    ],
    copts = [
        "-Wno-newline-eof",
    ],
    includes = [
        ".",
    ],
)
