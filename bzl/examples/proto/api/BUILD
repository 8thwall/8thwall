load("@rules_proto//proto:defs.bzl", "proto_library")
load("//bzl/js:js.bzl", "js_library")
load("//bzl/js:proto.bzl", "ts_proto_library")

cc_proto_library(
    name = "hello-cc",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":hello",
    ],
)

js_library(
    name = "hello-ts",
    srcs = [
        ":hello-ts-proto",
    ],
    npm_rule = "@npm-bzl-examples//:npm-bzl-examples",
    visibility = [
        "//visibility:public",
    ],
)

ts_proto_library(
    name = "hello-ts-proto",
    visibility = [
        "//visibility:private",
    ],
    deps = [
        # See https://github.com/thesayyn/protoc-gen-ts/issues/151
        ":details",  # TODO(nbutko): Fix ts_proto_library transitive deps and remove.
        ":hello",
    ],
)

proto_library(
    name = "details",
    srcs = [
        "details.proto",
    ],
    visibility = [
        "//visibility:private",
    ],
)

proto_library(
    name = "hello",
    srcs = [
        "hello.proto",
    ],
    visibility = [
        "//visibility:private",
    ],
    deps = [
        ":details",
    ],
)
