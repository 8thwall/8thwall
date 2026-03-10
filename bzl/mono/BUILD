load("//bzl/mono:toolchains.bzl", "mono_toolchain")

package(default_visibility = ["//visibility:public"])

toolchain_type(name = "toolchain_type")

mono_toolchain(
    name = "mono-osx-toolchain",
    mono = "@mono-macosx//:stub",
)

toolchain(
    name = "mono-macosx",
    exec_compatible_with = [
        "@platforms//os:osx",
    ],
    toolchain = ":mono-osx-toolchain",
    toolchain_type = ":toolchain_type",
)
