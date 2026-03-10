load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

proto_library(
    name = "greeter",
    srcs = [
        "greeter.proto",
    ],
    visibility = [
        "//visibility:private",
    ],
)

cc_proto_library(
    name = "greeter-cc",
    visibility = [
        "//visibility:private",
    ],
    deps = [
        ":greeter",
    ],
)

cc_grpc_library(
    name = "greeter-cc-grpc",
    srcs = [
        ":greeter",
    ],
    grpc_only = True,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":greeter-cc",
    ],
)

cc_binary(
    name = "greeter-client",
    srcs = ["greeter-client.cc"],
    deps = [
        ":greeter-cc-grpc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_binary(
    name = "greeter-server",
    srcs = ["greeter-server.cc"],
    deps = [
        ":greeter-cc-grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
    ],
)
