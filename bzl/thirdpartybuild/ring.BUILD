"""
This module builds the `ring` crate using bazel as opposed to cc-rs.

It adapts the published version of `ring` from crates.io, which already has pregenerated assembly files.

How to update:
1. Download the latest version of `ring` from crates.io
    - i.e. https://static.crates.io/crates/ring/ring-0.17.14.crate
2. Rename the file to have the `.tar.gz` extension
3. Upload the renamed file to the `8w-cargo-crate` bucket
4. Update the version numbers found in the http_archive target in WORKSPACE
5. Run `bazel build @ring --platforms=//bzl:ios_arm64` to make sure the library builds

How to add or change dependencies:
1. Update the `Cargo.toml` file found in `bzl/cargo/ring` with new dependencies
2. Run `CARGO_BAZEL_REPIN=true bazel sync --only=ring-deps` to regenerate BUILD files
3. Add the new dependencies to the `rust_library` rule in the `ring.BUILD` file
4. Verify your changes by running `bazel build @ring --platforms=//bzl:ios_arm64`
"""

load("@rules_rust//rust:defs.bzl", "rust_library")

licenses(["permissive"])  # ISC

package(default_visibility = ["//visibility:public"])

core_srcs = [
    "crypto/curve25519/curve25519.c",
    "crypto/fipsmodule/aes/aes_nohw.c",
    "crypto/fipsmodule/bn/montgomery.c",
    "crypto/fipsmodule/bn/montgomery_inv.c",
    "crypto/fipsmodule/ec/ecp_nistz.c",
    "crypto/fipsmodule/ec/gfp_p256.c",
    "crypto/fipsmodule/ec/gfp_p384.c",
    "crypto/fipsmodule/ec/p256.c",
    "crypto/limbs/limbs.c",
    "crypto/mem.c",
    "crypto/poly1305/poly1305.c",
    "crypto/fipsmodule/ec/p256-nistz.c",
]

osx_universal_asm_srcs = [
    "pregenerated/aes-gcm-avx2-x86_64-macosx.S",
    "pregenerated/aesni-gcm-x86_64-macosx.S",
    "pregenerated/aesni-x86_64-macosx.S",
    "pregenerated/chacha-x86_64-macosx.S",
    "pregenerated/chacha20_poly1305_x86_64-macosx.S",
    "pregenerated/ghash-x86_64-macosx.S",
    "pregenerated/p256-x86_64-asm-macosx.S",
    "pregenerated/sha256-x86_64-macosx.S",
    "pregenerated/sha512-x86_64-macosx.S",
    "pregenerated/vpaes-x86_64-macosx.S",
    "pregenerated/x86_64-mont-macosx.S",
    "pregenerated/x86_64-mont5-macosx.S",
]

ios_asm_srcs = [
    "pregenerated/aesv8-armx-ios64.S",
    "pregenerated/aesv8-gcm-armv8-ios64.S",
    "pregenerated/armv8-mont-ios64.S",
    "pregenerated/chacha-armv8-ios64.S",
    "pregenerated/chacha20_poly1305_armv8-ios64.S",
    "pregenerated/ghash-neon-armv8-ios64.S",
    "pregenerated/ghashv8-armx-ios64.S",
    "pregenerated/vpaes-armv8-ios64.S",
    "pregenerated/p256-armv8-asm-ios64.S",
    "pregenerated/sha512-armv8-ios64.S",
    "pregenerated/sha256-armv8-ios64.S",
]

android_arm64_asm_srcs = [
    "pregenerated/aesv8-armx-linux64.S",
    "pregenerated/aesv8-gcm-armv8-linux64.S",
    "pregenerated/armv8-mont-linux64.S",
    "pregenerated/chacha-armv8-linux64.S",
    "pregenerated/chacha20_poly1305_armv8-linux64.S",
    "pregenerated/ghash-neon-armv8-linux64.S",
    "pregenerated/ghashv8-armx-linux64.S",
    "pregenerated/p256-armv8-asm-linux64.S",
    "pregenerated/sha256-armv8-linux64.S",
    "pregenerated/sha512-armv8-linux64.S",
    "pregenerated/vpaes-armv8-linux64.S",
]

cc_library(
    name = "ring-core",
    srcs = select({
        # TODO(lreyna): Add support for other platforms
        "@the8thwall//bzl/conditions:osx-x86_64": osx_universal_asm_srcs,
        "@the8thwall//bzl/conditions:osx-arm64": ios_asm_srcs,
        "@the8thwall//bzl/conditions:ios": ios_asm_srcs,
        "@the8thwall//bzl/conditions:android-arm64": android_arm64_asm_srcs,
        "//conditions:default": ["@platforms//:incompatible"],
    }) + core_srcs,
    hdrs = glob([
        "crypto/**/*.inl",
        "crypto/**/*.h",
        "include/**/*.h",
        "pregenerated/ring_core_generated/**/*.h",
        "third_party/**/*.h",
    ]),
    includes = [
        "include",
        "pregenerated",
        "third_party",
    ],
)

# NOTE: These are embedded into the binary
filegroup(
    name = "key-templates",
    srcs = [
        "src/data/alg-rsa-encryption.der",
        "src/ec/curve25519/ed25519/ed25519_pkcs8_v2_template.der",
        "src/ec/suite_b/ecdsa/ecPublicKey_p256_pkcs8_v1_template.der",
        "src/ec/suite_b/ecdsa/ecPublicKey_p384_pkcs8_v1_template.der",
    ],
)

rust_library(
    name = "ring",
    srcs = glob([
        "src/**/*.rs",
    ]),
    compile_data = [
        ":key-templates",
    ],
    crate_features = ["alloc"],
    crate_name = "ring",
    crate_root = "src/lib.rs",
    edition = "2021",
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:osx": [],
        "@the8thwall//bzl/conditions:ios": [],
        "@the8thwall//bzl/conditions:android-arm64": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    version = "0.17.14",
    visibility = ["//visibility:public"],
    deps = [
        ":ring-core",
        "@ring-deps//:cfg-if",
        "@ring-deps//:getrandom",
        "@ring-deps//:libc",
        "@ring-deps//:untrusted",
    ],
)
