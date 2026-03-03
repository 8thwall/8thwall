licenses(["permissive"])  # GPL 2.0 with Linking Exception.

load("@the8thwall//bzl/wasm:wasm.bzl", "wasm_binary")

genrule(
    name = "features-js-h",
    outs = ["include/git2_features.h"],
    cmd = """cat << EOF > $@
#pragma once

// Disable generation of exported symbols.
#define GIT_EXTERN_HIDDEN 1

// Some deprecated features (like non-stream filters) appear to be broken.
// Probably best to ensure that we don't use any deprecated features which may
// exist in a half baked state.
#define GIT_DEPRECATE_HARD 1

#define GIT_SHA256_BUILTIN 1
#define GIT_SHA1_COLLISIONDETECT 1
#define GIT_REGEX_REGCOMP 1

// #define GIT_DEBUG_POOL 1
// #define GIT_TRACE 1
// #define GIT_THREADS 1
// #define GIT_MSVC_CRTDBG 1

// #define GIT_ARCH_64 1
#define GIT_ARCH_32 1

// Enables nanosecond percision for index when using stat.
#define GIT_USE_NSEC 1

/*
#define GIT_USE_ICONV 1
#define GIT_USE_STAT_MTIM 1
#define GIT_USE_STAT_MTIMESPEC 1
#define GIT_USE_STAT_MTIME_NSEC 1
#define GIT_USE_FUTIMENS 1
#define GIT_USE_REGCOMP_L 1
*/

// #define GIT_SSH 1
// #define GIT_SSH_MEMORY_CREDENTIALS 1

// #define GIT_GSSAPI 1
// #define GIT_WINHTTP 1

// #define GIT_HTTPS 1
// #define GIT_OPENSSL 1

#ifdef __APPLE__
#define GIT_SECURE_TRANSPORT 1
#endif


// #define GIT_MBEDTLS 1

// #define GIT_SHA1_COLLISIONDETECT 1
// #define GIT_SHA1_WIN32 1
// #define GIT_SHA1_COMMON_CRYPTO 1
// #define GIT_SHA1_OPENSSL 1
// #define GIT_SHA1_MBEDTLS 1
EOF""",
)

cc_library(
    name = "features-js",
    hdrs = [
        ":features-js-h",
    ],
    includes = [
        "include",
    ],
)

genrule(
    name = "experimental-js-h",
    outs = ["experimental.h"],
    cmd = """cat << EOF > $@
#pragma once

// The experimental file was introduced as part of experimental SHA256 support.
// Currently leave it disabled until we actually want it.
// #define GIT_EXPERIMENTAL_SHA256 1

""",
)

genrule(
    name = "git2-experimental-js-h",
    outs = ["git2/experimental.h"],
    cmd = """cat << EOF > $@
#pragma once

// The experimental file was introduced as part of experimental SHA256 support.
// Currently leave it disabled until we actually want it.
// #define GIT_EXPERIMENTAL_SHA256 1

""",
)

cc_library(
    name = "experimental-js",
    hdrs = [
        ":experimental-js-h",
        ":git2-experimental-js-h",
    ],
    includes = [
        "include",
    ],
)

cc_library(
    name = "http-parser",
    srcs = [
        "deps/http-parser/http_parser.c",
    ],
    hdrs = [
        "deps/http-parser/http_parser.h",
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "picosha2",
    srcs = [
        "deps/picosha2/picosha2-c.cc",
    ],
    hdrs = [
        "deps/picosha2/picosha2.h",
        "deps/picosha2/picosha2-c.h",
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "libgit2",
    srcs = glob(
        [
            "src/libgit2/*.h",
            "src/libgit2/*.c",
            "src/util/*.h",
            "src/util/*.c",
            "src/util/allocators/*.h",
            "src/util/allocators/*.c",
            "src/util/hash/*.h",
            "src/util/hash/*.c",
            "src/util/hash/rfc6234/*.h",
            "src/util/hash/rfc6234/*.c",
            "src/util/hash/sha1dc/*.h",
            "src/util/hash/sha1dc/*.c",
            "src/util/unix/*.h",
            "src/util/unix/*.c",
            "src/libgit2/transports/*.h",
            "src/libgit2/transports/*.c",
            "src/libgit2/transports/*.cc",
            "src/libgit2/streams/*.h",
            "src/libgit2/streams/*.c",
            "src/libgit2/xdiff/*.h",
            "src/libgit2/xdiff/*.c",
        ],
        exclude = [
            "src/util/hash/hash_win32.h",
            "src/util/hash/hash_win32.c",
            "src/util/hash/openssl.h",
            "src/util/hash/openssl.c",
            "src/util/hash/common_crypto.h",
            "src/util/hash/common_crypto.c",
            "src/util/hash/hash_mbedtls.h",
            "src/util/hash/hash_mbedtls.c",
            "src/util/hash/win32.h",
            "src/util/hash/win32.c",
        ],
    ),
    hdrs = glob(
        [
            "include/*.h",
            "include/**/*.h",
        ],
        exclude = [
            "include/git2/inttypes.h",
            "include/git2/stdint.h",
        ],
    ),
    copts = [
        "-Iexternal/libgit2/src/libgit2",
        "-Iexternal/libgit2/src/util",
        "-Iexternal/libgit2/src/util/hash",
        "-Iexternal/libgit2/include",
        "-Iexternal/libgit2/include/git2",
        "-Iexternal/libgit2/deps/http-parser",
        "-Iexternal/libgit2/deps/picosha2",
        "-DSONAME=OFF",
        "-DUSE_HTTPS=OFF",
        "-DBUILD_SHARED_LIBS=OFF",
        "-DTHREADSAFE=OFF",
        "-DBUILD_CLAR=OFF",
        "-DUSE_SSH=OFF",
        "-Wno-incompatible-pointer-types",
        "-Wno-invalid-pp-token",
        "-Wno-format",
        "-Wno-deprecated-declarations",
        "-Wno-misleading-indentation",
    ] + select({
        "@the8thwall//bzl/conditions:wasm": [
            "-DNO_MMAP",
            "-DGIT_USE_STAT_MTIM",
            "-DGIT_USE_STAT_MTIM_NSEC",
        ],
        "//conditions:default": [
            "-DGIT_USE_STAT_MTIMESPEC",
        ],
    }),
    includes = [
        "include",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:osx": [
            "-framework CoreFoundation",
            "-framework Security",
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":experimental-js",
        ":features-js",
        ":http-parser",
        ":picosha2",
        "@zlib",
    ],
)
