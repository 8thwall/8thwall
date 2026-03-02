# Example rule to build a Windows DLL.
cc_binary(
    name = "dll-example",
    linkshared = 1,  # Link in all exported methods in deps.
    linkstatic = 1,  # Build a shared lib (DLL).
    target_compatible_with = [
        "@platforms//os:windows",  # Only build this rule for windows.
    ],
    visibility = ["//visibility:private"],
    deps = [
        "//bzl/examples:export-methods",  # An example alwayslink=1 library with symbols.
    ],
)

# Unit test that uses 'objdump -p' to confirm that the Windows DLL has the
# expected public symbols.
sh_test(
    name = "dll-symbol-test",
    size = "small",
    srcs = ["dll-symbol-test.sh"],
    data = [
        ":dll-example",
        "//bzl/llvm:llvm-objdump",
    ],
    target_compatible_with = [
        "@platforms//os:windows",  # Only build this rule for windows.
    ],
)
