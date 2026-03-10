load("//third_party/grpc:grpc-wrap-rules.bzl", cpp_grpc_library = "cpp_grpc_library_versionable")

package(
    default_visibility = ["//visibility:private"],
)

cpp_grpc_library(
    name = "test_service_utils",
    protos = [
        "//bzl/examples/grpc/proto:test_service_proto",
    ],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_binary(
    name = "grpc_client",
    srcs = [
        "grpc_client.cpp",
    ],
    deps = [
        ":test_service_utils",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_binary(
    name = "grpc_server",
    srcs = [
        "grpc_server.cpp",
    ],
    deps = [
        ":test_service_utils",
        "@com_github_grpc_grpc//:grpc++",
    ],
)
