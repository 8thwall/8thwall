# Run a specific select when built with --features=simd.
def onSimd(items, default = []):
    return select({
        "@the8thwall//bzl/conditions:simd": items,
        "//conditions:default": default,
    })

# Specify non-portable SIMD instructions in your build rules with the following.
#
# load("//bzl/crosstool:simd.bzl", "SIMD")
# cc_library(
#   ...,
#   copts = SIMD.SSE3,
# )
SIMD = struct(
    SSE = onSimd(["-msse"]),
    SSE2 = onSimd(["-msse2"]),
    SSE3 = onSimd(["-msse3"]),
    SSSE3 = onSimd(["-mssse3"]),
    SSE4_1 = onSimd(["-msse4.1"]),
    SSE4_2 = onSimd(["-msse4.2"]),
    AVX = onSimd(["-mavx"]),
    NEON = onSimd(["-mfpu=neon"]),
)
