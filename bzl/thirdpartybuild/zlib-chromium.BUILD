licenses(["permissive"])

load("@the8thwall//bzl/windows:select.bzl", "onWindows")

cc_library(
    name = "zlib",
    srcs = [
        "adler32.c",
        "chromeconf.h",
        "compress.c",
        "contrib/optimizations/insert_string.h",
        "cpu_features.c",
        "cpu_features.h",
        "crc32.c",
        "crc32.h",
        "deflate.c",
        "deflate.h",
        "gzclose.c",
        "gzguts.h",
        "gzlib.c",
        "gzread.c",
        "gzwrite.c",
        "infback.c",
        "inffast.c",
        "inffast.h",
        "inffixed.h",
        "inflate.h",
        "inftrees.c",
        "inftrees.h",
        "trees.c",
        "trees.h",
        "uncompr.c",
        "zconf.h",
        "zlib.h",
        "zutil.c",
        "zutil.h",
    ] + select({
        "@the8thwall//bzl/conditions:x86-any": [
            # Use SIMD for all x86 cpus.
            "crc32_simd.c",
            "crc32_simd.h",
            "crc_folding.c",
            "adler32_simd.c",
            "adler32_simd.h",
            "slide_hash_simd.h",
            "contrib/optimizations/chunkcopy.h",
            "contrib/optimizations/inffast_chunk.c",
            "contrib/optimizations/inffast_chunk.h",
            "contrib/optimizations/inflate.c",
        ],
        "@the8thwall//bzl/conditions:arm-any": [
            # Use NEON for all x86 cpus.
            "crc32_simd.c",
            "crc32_simd.h",
            "adler32_simd.c",
            "adler32_simd.h",
            "slide_hash_simd.h",
            "contrib/optimizations/chunkcopy.h",
            "contrib/optimizations/inffast_chunk.c",
            "contrib/optimizations/inffast_chunk.h",
            "contrib/optimizations/inflate.c",
        ],
        "@the8thwall//bzl/conditions:simd": [
            # Use SIMD for wasm simd.
            "crc32_simd.c",
            "crc32_simd.h",
            "crc_folding.c",
            "adler32_simd.c",
            "adler32_simd.h",
            "slide_hash_simd.h",
            "contrib/optimizations/chunkcopy.h",
            "contrib/optimizations/inffast_chunk.c",
            "contrib/optimizations/inffast_chunk.h",
            "contrib/optimizations/inflate.c",
        ],
        "//conditions:default": [
            "inflate.c",
        ],
    }),
    hdrs = glob(["*.h"]),
    copts = [
        "-w",
        "-Dverbose=-1",
        "-Wno-deprecated-non-prototype",
        "-Wno-incompatible-pointer-types",
        "-Wunused-variable",
        "-Wno-incompatible-pointer-types-discards-qualifiers",
    ] + select({
        "@the8thwall//bzl/conditions:x86_32": [
            "-DDEFLATE_SLIDE_HASH_SSE2",
            "-DINFLATE_CHUNK_SIMD_SSE2",
            "-DADLER32_SIMD_SSSE3",
        ],
        "@the8thwall//bzl/conditions:x86_64": [
            "-DCRC32_SIMD_SSE42_PCLMUL",
            "-DDEFLATE_SLIDE_HASH_SSE2",
            "-DINFLATE_CHUNK_SIMD_SSE2",
            "-DINFLATE_CHUNK_READ_64LE",
            "-DADLER32_SIMD_SSSE3",
        ],
        "@the8thwall//bzl/conditions:arm": [
            "-DDEFLATE_SLIDE_HASH_NEON",
            "-DADLER32_SIMD_NEON",
            "-DCRC32_ARMV8_CRC32",
            "-DINFLATE_CHUNK_SIMD_NEON",
        ],
        "@the8thwall//bzl/conditions:arm64": [
            "-DDEFLATE_SLIDE_HASH_NEON",
            "-DADLER32_SIMD_NEON",
            "-DCRC32_ARMV8_CRC32",
            "-DINFLATE_CHUNK_SIMD_NEON",
            "-DINFLATE_CHUNK_READ_64LE",
        ],
        "@the8thwall//bzl/conditions:simd": [
            "-DDEFLATE_SLIDE_HASH_SSE2",
            "-DINFLATE_CHUNK_SIMD_SSE2",
            # "-DADLER32_SIMD_SSSE3",  # Currently not compiling for wasmsimd.
        ],
        "//conditions:default": [
        ],
    }) + select({
        "@the8thwall//bzl/conditions:windows": [
            "-msse4.2",
            "-mpclmul",
            "-Wno-implicit-function-declaration",
            "-DX86_WINDOWS",
            "-D_CRT_SECURE_NO_DEPRECATE",
            "-D_CRT_NONSTDC_NO_DEPRECATE",
        ],
        "@the8thwall//bzl/conditions:android-arm": [
            "-DARMV8_OS_ANDROID",
            "-DZ_HAVE_UNISTD_H=1",
        ],
        "@the8thwall//bzl/conditions:android-arm64": [
            "-DARMV8_OS_ANDROID",
            "-DZ_HAVE_UNISTD_H=1",
        ],
        "@the8thwall//bzl/conditions:ios": [
            "-DARMV8_OS_IOS",
            "-DZ_HAVE_UNISTD_H=1",
        ],
        "@the8thwall//bzl/conditions:osx-arm64": [
            "-DARMV8_OS_MACOS",
            "-DZ_HAVE_UNISTD_H=1",
        ],
        "@the8thwall//bzl/conditions:osx-x86_64": [
            "-msse4.2",
            "-mpclmul",
            "-DX86_NOT_WINDOWS",
            "-DZ_HAVE_UNISTD_H=1",
        ],
        "@the8thwall//bzl/conditions:v1-linux": [
            "-msse4.2",
            "-mpclmul",
            "-DX86_NOT_WINDOWS",
            "-DZ_HAVE_UNISTD_H=1",
        ],
        "@the8thwall//bzl/conditions:v2-linux": [
            "-msse4.2",
            "-mpclmul",
            "-DX86_NOT_WINDOWS",
            "-DZ_HAVE_UNISTD_H=1",
        ],
        "@the8thwall//bzl/conditions:wasm": [
            "-Wno-implicit-function-declaration",
            "-DZ_HAVE_UNISTD_H=1",
        ],
    }) + select({
        "@the8thwall//bzl/conditions:simd": [
            "-D__SSE__",
            "-D__SSE2__",
            "-msse",
        ],
        "//conditions:default": [],
    }),
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = select({
        "@the8thwall//bzl/conditions:android": [
            "@the8thwall//bzl/android:cpu-features",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "compression-utils-portable",
    srcs = [
        "google/compression_utils_portable.cc",
    ],
    hdrs = [
        "google/compression_utils_portable.h",
    ],
    includes = ["google"],
    visibility = ["//visibility:public"],
    deps = [
        ":zlib",
    ],
)
