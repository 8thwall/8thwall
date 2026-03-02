licenses(["permissive"])

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "neon-sse",
    srcs = [
    ],
    hdrs = [
        "NEON_2_SSE.h",
    ],
    copts = [
        "-Wno-deprecated-declarations",
    ],
)

cc_library(
    name = "sse-neon",
    srcs = [
        # TODO(dat): include the alignment when we find posix_memalign
        #      "SSE2NEONBinding.cpp"
    ],
    hdrs = [
        "SSE2NEON.h",
        #      "SSE2NEONBinding.h"
    ],
)
