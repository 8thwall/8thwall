load("@the8thwall//bzl/capnproto:capnproto.bzl", "cc_capnp_library")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "kj",
    srcs = [
        "c++/src/kj/arena.c++",
        "c++/src/kj/array.c++",
        "c++/src/kj/cidr.c++",
        "c++/src/kj/common.c++",
        "c++/src/kj/debug.c++",
        "c++/src/kj/encoding.c++",
        "c++/src/kj/exception.c++",
        "c++/src/kj/filesystem.c++",
        "c++/src/kj/filesystem-disk-unix.c++",
        "c++/src/kj/filesystem-disk-win32.c++",
        "c++/src/kj/hash.c++",
        "c++/src/kj/io.c++",
        "c++/src/kj/list.c++",
        "c++/src/kj/main.c++",
        "c++/src/kj/memory.c++",
        "c++/src/kj/mutex.c++",
        "c++/src/kj/parse/char.c++",
        "c++/src/kj/refcount.c++",
        "c++/src/kj/source-location.c++",
        "c++/src/kj/string.c++",
        "c++/src/kj/string-tree.c++",
        "c++/src/kj/table.c++",
        "c++/src/kj/test-helpers.c++",
        "c++/src/kj/thread.c++",
        "c++/src/kj/time.c++",
        "c++/src/kj/units.c++",
        # kj-async
        "c++/src/kj/async.c++",
        "c++/src/kj/async-io.c++",
        "c++/src/kj/async-io-unix.c++",
        "c++/src/kj/async-io-win32.c++",
        "c++/src/kj/async-unix.c++",
        "c++/src/kj/async-win32.c++",
        "c++/src/kj/timer.c++",
    ],
    hdrs = [
        "c++/src/kj/arena.h",
        "c++/src/kj/array.h",
        "c++/src/kj/cidr.h",
        "c++/src/kj/common.h",
        "c++/src/kj/debug.h",
        "c++/src/kj/encoding.h",
        "c++/src/kj/exception.h",
        "c++/src/kj/filesystem.h",
        "c++/src/kj/function.h",
        "c++/src/kj/hash.h",
        "c++/src/kj/io.h",
        "c++/src/kj/list.h",
        "c++/src/kj/main.h",
        "c++/src/kj/map.h",
        "c++/src/kj/memory.h",
        "c++/src/kj/miniposix.h",
        "c++/src/kj/mutex.h",
        "c++/src/kj/one-of.h",
        "c++/src/kj/parse/char.h",
        "c++/src/kj/parse/common.h",
        "c++/src/kj/refcount.h",
        "c++/src/kj/source-location.h",
        "c++/src/kj/std/iostream.h",
        "c++/src/kj/string.h",
        "c++/src/kj/string-tree.h",
        "c++/src/kj/table.h",
        "c++/src/kj/test.h",
        "c++/src/kj/thread.h",
        "c++/src/kj/threadlocal.h",
        "c++/src/kj/time.h",
        "c++/src/kj/tuple.h",
        "c++/src/kj/units.h",
        "c++/src/kj/vector.h",
        "c++/src/kj/win32-api-version.h",
        "c++/src/kj/windows-sanity.h",
        # kj-async
        "c++/src/kj/async.h",
        "c++/src/kj/async-inl.h",
        "c++/src/kj/async-io.h",
        "c++/src/kj/async-io-internal.h",
        "c++/src/kj/async-prelude.h",
        "c++/src/kj/async-queue.h",
        "c++/src/kj/async-unix.h",
        "c++/src/kj/async-win32.h",
        "c++/src/kj/timer.h",
    ],
    copts = [
        "-Wno-sign-compare",
        "-Wno-strict-aliasing",
        "-Wno-unused-const-variable",
        "-Wno-unused-function",
        "-Wno-user-defined-warnings",
        "-Wno-deprecated-declarations",
        "-D_WINSOCK_DEPRECATED_NO_WARNINGS",
    ],
    defines = select({
        "@the8thwall//bzl/conditions:wasm": [
            "KJ_USE_FUTEX=0",
            "KJ_USE_EPOLL=0",
        ],
        # NOTE(dat): Might need to also link Ws2_32
        "@the8thwall//bzl/conditions:windows": ["-lAdvAPI32"],
        "//conditions:default": ["KJ_USE_FUTEX=0"],
    }),
    includes = ["c++/src"],
)

cc_library(
    name = "capnp-lib",
    srcs = [
        "c++/src/capnp/any.c++",
        "c++/src/capnp/arena.c++",
        "c++/src/capnp/blob.c++",
        "c++/src/capnp/c++.capnp.c++",
        "c++/src/capnp/dynamic.c++",
        "c++/src/capnp/layout.c++",
        "c++/src/capnp/list.c++",
        "c++/src/capnp/message.c++",
        "c++/src/capnp/schema.c++",
        "c++/src/capnp/schema.capnp.c++",
        "c++/src/capnp/schema-loader.c++",
        "c++/src/capnp/serialize.c++",
        "c++/src/capnp/serialize-packed.c++",
        "c++/src/capnp/stream.capnp.c++",
        "c++/src/capnp/stringify.c++",
        # capnp-rpc
        "c++/src/capnp/capability.c++",
        "c++/src/capnp/dynamic-capability.c++",
        "c++/src/capnp/ez-rpc.c++",
        "c++/src/capnp/membrane.c++",
        "c++/src/capnp/persistent.capnp.c++",
        "c++/src/capnp/reconnect.c++",
        "c++/src/capnp/rpc.c++",
        "c++/src/capnp/rpc.capnp.c++",
        "c++/src/capnp/rpc-twoparty.c++",
        "c++/src/capnp/rpc-twoparty.capnp.c++",
        "c++/src/capnp/serialize-async.c++",
    ],
    hdrs = [
        "c++/src/capnp/any.h",
        "c++/src/capnp/arena.h",
        "c++/src/capnp/blob.h",
        "c++/src/capnp/c++.capnp.h",
        "c++/src/capnp/capability.h",
        "c++/src/capnp/common.h",
        "c++/src/capnp/dynamic.h",
        "c++/src/capnp/endian.h",
        "c++/src/capnp/generated-header-support.h",
        "c++/src/capnp/layout.h",
        "c++/src/capnp/list.h",
        "c++/src/capnp/membrane.h",
        "c++/src/capnp/message.h",
        "c++/src/capnp/orphan.h",
        "c++/src/capnp/pointer-helpers.h",
        "c++/src/capnp/pretty-print.h",
        "c++/src/capnp/raw-schema.h",
        "c++/src/capnp/schema.capnp.h",
        "c++/src/capnp/schema.h",
        "c++/src/capnp/schema-lite.h",
        "c++/src/capnp/schema-loader.h",
        "c++/src/capnp/schema-parser.h",
        "c++/src/capnp/serialize.h",
        "c++/src/capnp/serialize-async.h",
        "c++/src/capnp/serialize-packed.h",
        "c++/src/capnp/serialize-text.h",
        "c++/src/capnp/stream.capnp.h",
        # capnp-rpc
        "c++/src/capnp/ez-rpc.h",
        "c++/src/capnp/persistent.capnp.h",
        "c++/src/capnp/reconnect.h",
        "c++/src/capnp/rpc.capnp.h",
        "c++/src/capnp/rpc.h",
        "c++/src/capnp/rpc-prelude.h",
        "c++/src/capnp/rpc-twoparty.capnp.h",
        "c++/src/capnp/rpc-twoparty.h",
    ],
    copts = [
        "-Wno-sign-compare",
    ] + select({
        "@the8thwall//bzl/conditions:android": [],
        "//conditions:default": [
            "-Wno-deprecated-builtins",  # Doesn't exist in NDK v24.
        ],
    }),
    includes = ["c++/src"],
    linkopts = select({
        "@the8thwall//bzl/conditions:windows": ["-lAdvAPI32"],
        "//conditions:default": [],
    }),
    deps = [":kj"],
)

cc_library(
    name = "json",
    srcs = [
        "c++/src/capnp/compat/json.c++",
        "c++/src/capnp/compat/json.capnp.c++",
    ],
    hdrs = [
        "c++/src/capnp/compat/json.capnp.h",
        "c++/src/capnp/compat/json.h",
    ],
    includes = ["c++/src"],
    deps = [
        ":capnp-lib",
    ],
)

cc_library(
    name = "capnpc-lib",
    srcs = [
        "c++/src/capnp/compiler/compiler.c++",
        "c++/src/capnp/compiler/error-reporter.c++",
        "c++/src/capnp/compiler/generics.c++",
        "c++/src/capnp/compiler/grammar.capnp.c++",
        "c++/src/capnp/compiler/lexer.c++",
        "c++/src/capnp/compiler/lexer.capnp.c++",
        "c++/src/capnp/compiler/node-translator.c++",
        "c++/src/capnp/compiler/parser.c++",
        "c++/src/capnp/compiler/type-id.c++",
        "c++/src/capnp/schema-parser.c++",
        "c++/src/capnp/serialize-text.c++",
    ],
    hdrs = [
        "c++/src/capnp/compiler/compiler.h",
        "c++/src/capnp/compiler/error-reporter.h",
        "c++/src/capnp/compiler/generics.h",
        "c++/src/capnp/compiler/grammar.capnp.h",
        "c++/src/capnp/compiler/lexer.capnp.h",
        "c++/src/capnp/compiler/lexer.h",
        "c++/src/capnp/compiler/module-loader.h",
        "c++/src/capnp/compiler/node-translator.h",
        "c++/src/capnp/compiler/parser.h",
        "c++/src/capnp/compiler/resolver.h",
        "c++/src/capnp/compiler/type-id.h",
    ],
    include_prefix = "capnp",
    deps = [
        ":json",
    ],
)

cc_binary(
    name = "capnp",
    srcs = [
        "c++/src/capnp/compiler/capnp.c++",
        "c++/src/capnp/compiler/module-loader.c++",
    ],
    deps = [":capnpc-lib"],
)

genrule(
    name = "capnpc-bin",
    srcs = [":capnp"],
    outs = ["capnpc"],
    cmd = "ln -s $$(basename $(location :capnp)) $@",
)

cc_binary(
    name = "capnpc-c++",
    srcs = ["c++/src/capnp/compiler/capnpc-c++.c++"],
    copts = [
        "-Wno-enum-compare-switch",
    ],
    deps = [":capnpc-lib"],
)

cc_binary(
    name = "capnpc-capnp",
    srcs = ["c++/src/capnp/compiler/capnpc-capnp.c++"],
    deps = [":capnpc-lib"],
)

filegroup(
    name = "capnp-capnp",
    srcs = glob([
        "c++/src/capnp/**/*.capnp",
    ]),
)

cc_capnp_library(
    name = "test",
    srcs = ["c++/src/capnp/test.capnp"],
    data = glob(["c++/src/capnp/testdata/**"]),
)

cc_capnp_library(
    name = "test-import",
    srcs = ["c++/src/capnp/test-import.capnp"],
    include = "c++/src/capnp",
    deps = [":test"],
)
