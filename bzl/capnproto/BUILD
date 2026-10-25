package(default_visibility = ["//visibility:public"])

sh_binary(
    name = "capnpc-js-plugin-unix",
    srcs = [
        "capnpc-js-plugin.sh",
    ],
    data = [
        "@npm-capnpc-js//:modules",
    ],
)

sh_binary(
    name = "capnpc-ts-plugin-unix",
    srcs = [
        "capnpc-ts-plugin.sh",
    ],
    data = [
        "@npm-capnpc-ts//:modules",
    ],
)

py_binary(
    name = "capnpc-js-plugin-windows",
    srcs = [
        "capnpc-js-plugin-windows.py",
    ],
    data = [
        "@npm-capnpc-js//:modules",
    ],
    main = "capnpc-js-plugin-windows.py",
)

py_binary(
    name = "capnpc-ts-plugin-windows",
    srcs = [
        "capnpc-ts-plugin-windows.py",
    ],
    data = [
        "@npm-capnpc-ts//:modules",
    ],
    main = "capnpc-ts-plugin-windows.py",
)

alias(
    name = "capnpc-js-plugin",
    actual = select({
        "//bzl/conditions:windows": ":capnpc-js-plugin-windows",
        "//conditions:default": ":capnpc-js-plugin-unix",
    }),
)

alias(
    name = "capnpc-ts-plugin",
    actual = select({
        "//bzl/conditions:windows": ":capnpc-ts-plugin-windows",
        "//conditions:default": ":capnpc-ts-plugin-unix",
    }),
)
