load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "inliner",
    srcs = ["inliner.py"],
    data = [
        ":parse-build-rules-sh",
    ],
)

proto_library(
    name = "build-rule",
    srcs = ["build-rule.proto"],
)

cc_proto_library(
    name = "build-rule-cc",
    deps = [
        ":build-rule",
    ],
)

sh_binary(
    name = "parse-build-rules-sh",
    srcs = [
        "parse-build-rules.sh",
    ],
    data = [
        ":parse-build-rules",
    ],
)

cc_binary(
    name = "parse-build-rules",
    srcs = [
        "parse-build-rules.cc",
    ],
    deps = [
        ":build-rule-cc",
        "//c8:c8-log",
        "//c8:string",
        "//third_party/smhasher:murmurhash3",
        "@com_google_protobuf//:protobuf",
        "@llvm-project//clang:ast_matchers",
        "@llvm-project//clang:frontend",
        "@llvm-project//clang:tooling",
    ],
)
