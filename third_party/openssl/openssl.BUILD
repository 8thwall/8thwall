load("@the8thwall//third_party/openssl:openssl.bzl", "openssl_build")
load("@android-sdk//:defines.bzl", "ANDROID_NDK")
load("@bazel_skylib//rules:select_file.bzl", "select_file")

licenses(["permissive"])  # BSD 2-clause

package(default_visibility = ["//visibility:public"])

openssl_build(
    name = "openssl-build",
    srcs = glob([
        "*",
        "**/*",
    ]),
    configure_flags = select({
        "@the8thwall//bzl/conditions:osx-x86_64": "darwin64-x86_64-cc no-tests no-ssl2 no-ssl3 shared -DCMAKE_BUILD_TYPE=Release",
        "@the8thwall//bzl/conditions:osx-arm64": "enable-rc5 no-tests zlib darwin64-arm64-cc no-asm -DCMAKE_BUILD_TYPE=Release",
        "@the8thwall//bzl/conditions:android64": "linux-generic64 no-tests no-shared -DCMAKE_BUILD_TYPE=Release",  # Use linux-generic, so OpenSSL doesn't override Bazel's android toolchain info.
        "@the8thwall//bzl/conditions:android32": "linux-generic32 no-tests no-shared -DCMAKE_BUILD_TYPE=Release",  # Use linux-generic, so OpenSSL doesn't override Bazel's android toolchain info.
        "@the8thwall//bzl/conditions:ios": "darwin64-arm64-cc no-tests no-async no-shared -DCMAKE_BUILD_TYPE=Release",  # Use darwin64-arm64-cc, so OpenSSL doesn't override Bazel's ios toolchain info.
        "@the8thwall//bzl/conditions:v1-linux": "linux-generic64 no-tests no-shared -DCMAKE_BUILD_TYPE=Release",  # Use linux-generic, so OpenSSL doesn't override Bazel's android toolchain info.
        "@the8thwall//bzl/conditions:v2-linux": "linux-generic64 no-tests no-shared -DCMAKE_BUILD_TYPE=Release",  # Use linux-generic, so OpenSSL doesn't override Bazel's android toolchain info.
        "//conditions:default": "no-tests no-ssl2 no-ssl3 shared -DCMAKE_BUILD_TYPE=Release",
    }),
    copts = [
        "-Wno-unused-but-set-variable",
    ],
    # TODO(sxian): This won't work for building Android on Linux, we need to figure out how to get the hostOS then pick
    # the right path for the toolchain.
    toolchain = select({
        "@the8thwall//bzl/conditions:android": ANDROID_NDK + "/toolchains/llvm/prebuilt/darwin-x86_64/bin",
        "//conditions:default": "",
    }),
)

cc_library(
    name = "crypto",
    hdrs = glob(["include/openssl/*.h"]),
    includes = ["include"],
    linkopts = select({
        "@the8thwall//bzl/conditions:android": [],
        "@the8thwall//bzl/conditions:apple": [],
        "//conditions:default": [
            "-lpthread",
            "-ldl",
        ],
    }),
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:apple": [],
        "@the8thwall//bzl/conditions:android": [],
        "@the8thwall//bzl/conditions:v1-linux": [],
        "@the8thwall//bzl/conditions:v2-linux": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:public"],
    deps = [":openssl-build"],
)

cc_library(
    name = "ssl",
    hdrs = glob(["include/openssl/*.h"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = [":crypto"],
)

# Pick libcrypto.a out of the outputs of openssl-build
select_file(
    name = "libcrypto",
    srcs = ":openssl-build",
    subpath = "libcrypto.a",
)

# Pick libssl.a out of the outputs of openssl-build
select_file(
    name = "libssl",
    srcs = ":openssl-build",
    subpath = "libssl.a",
)

# Pick opensslconf.h out of the outputs of openssl-build
select_file(
    name = "opensslconf_header",
    srcs = ":openssl-build",
    subpath = "include/openssl/opensslconf.h",
)

# Pick opensslv.h out of the outputs of openssl-build
select_file(
    name = "opensslv_header",
    srcs = ":openssl-build",
    subpath = "include/openssl/opensslv.h",
)

# Create a directory structure expected by rust-openssl build script
# This needs lib/ and include/ subdirectories in the proper layout
genrule(
    name = "openssl-rust-dir",
    srcs = [
        ":libcrypto",
        ":libssl",
        ":opensslconf_header",
        ":opensslv_header",
    ] + glob(["include/**"]),
    outs = ["openssl-rust-complete"],
    cmd = """
        mkdir -p $@/lib $@/include/openssl
        cp $(location :libcrypto) $@/lib/
        cp $(location :libssl) $@/lib/
        cp $(location :opensslconf_header) $@/include/openssl/
        cp $(location :opensslv_header) $@/include/openssl/
    """,
    visibility = ["//visibility:public"],
)
