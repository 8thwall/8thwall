load(":llvm.bzl", "llvm_tool_wrapper")
load(":toolchains.bzl", "llvm_toolchain")
load("//bzl/js:js.bzl", "js_cli", "js_test")

package(default_visibility = ["//visibility:public"])

toolchain_type(name = "toolchain_type")

llvm_toolchain(
    name = "llvm-macosx-arm64-toolchain",
    llvm = "@llvm-macosx-arm64//:stub",
    runtime = "Toolchains/LLVM16.0.6.xctoolchain/usr/lib/clang/16",
    usr = "Toolchains/LLVM16.0.6.xctoolchain/usr",
    xcode_toolchain_id = "org.llvm.16.0.6",
    xcode_xctoolchain = "LLVM16.0.6.xctoolchain",
)

toolchain(
    name = "llvm-macosx-arm64",
    exec_compatible_with = [
        "@platforms//os:osx",
        "@platforms//cpu:arm64",
    ],
    toolchain = ":llvm-macosx-arm64-toolchain",
    toolchain_type = ":toolchain_type",
)

llvm_toolchain(
    name = "llvm-macosx-x86_64-toolchain",
    llvm = "@llvm-macosx-x86_64//:stub",
    runtime = "Toolchains/LLVM16.0.6.xctoolchain/usr/lib/clang/16",
    usr = "Toolchains/LLVM16.0.6.xctoolchain/usr",
    xcode_toolchain_id = "org.llvm.16.0.6",
    xcode_xctoolchain = "LLVM16.0.6.xctoolchain",
)

toolchain(
    name = "llvm-macosx-x86_64",
    exec_compatible_with = [
        "@platforms//os:osx",
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":llvm-macosx-x86_64-toolchain",
    toolchain_type = ":toolchain_type",
)

llvm_toolchain(
    name = "llvm-linux-arm64-toolchain",
    llvm = "@llvm-linux-arm64//:stub",
    runtime = "lib/clang/16",
)

toolchain(
    name = "llvm-linux-arm64",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:arm64",
    ],
    toolchain = ":llvm-linux-arm64-toolchain",
    toolchain_type = ":toolchain_type",
)

llvm_toolchain(
    name = "llvm-windows-toolchain",
    llvm = "@llvm-windows//:stub",
    runtime = "lib/clang/16",
)

toolchain(
    name = "llvm-windows",
    exec_compatible_with = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":llvm-windows-toolchain",
    toolchain_type = ":toolchain_type",
)

llvm_toolchain(
    name = "llvm-linux-toolchain",
    llvm = "@llvm-linux//:stub",
    runtime = "lib/clang/16",
)

toolchain(
    name = "llvm-linux",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":llvm-linux-toolchain",
    toolchain_type = ":toolchain_type",
)

filegroup(
    name = "clang-tidy-config",
    srcs = [
        "clang-tidy-config.yaml",
    ],
)

js_cli(
    name = "merge-compile-commands",
    main = [
        "merge-compile-commands.js",
    ],
    visibility = ["//visibility:private"],
)

# Include-what-you-use.
filegroup(
    name = "iwyu",
    srcs = ["iwyu.sh"],
)

llvm_tool_wrapper(name = "clang-apply-replacements")

llvm_tool_wrapper(name = "clang-check")

llvm_tool_wrapper(name = "clang-doc")

llvm_tool_wrapper(name = "clang-extdef-mapping")

llvm_tool_wrapper(name = "clang-format")

llvm_tool_wrapper(name = "clang-include-fixer")

llvm_tool_wrapper(name = "clang-move")

llvm_tool_wrapper(name = "clang-offload-bundler")

llvm_tool_wrapper(name = "clang-offload-wrapper")

llvm_tool_wrapper(name = "clang-query")

llvm_tool_wrapper(name = "clang-refactor")

llvm_tool_wrapper(name = "clang-rename")

llvm_tool_wrapper(name = "clang-reorder-fields")

llvm_tool_wrapper(name = "clang-scan-deps")

llvm_tool_wrapper(name = "clang-tidy")

llvm_tool_wrapper(name = "llvm-dis")

llvm_tool_wrapper(name = "llvm-lipo")

llvm_tool_wrapper(name = "llvm-nm")

llvm_tool_wrapper(name = "llvm-readelf")

llvm_tool_wrapper(name = "llvm-objdump")

llvm_tool_wrapper(name = "llvm-dwarfdump")

llvm_tool_wrapper(name = "llvm-symbolizer")

llvm_tool_wrapper(name = "llvm-install-name-tool")

llvm_tool_wrapper(name = "modularize")

llvm_tool_wrapper(name = "pp-trace")

llvm_tool_wrapper(name = "dsymutil")
