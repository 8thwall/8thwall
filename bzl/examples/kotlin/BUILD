package(default_visibility = ["//visibility:private"])

load("@rules_kotlin//kotlin:jvm.bzl", "kt_jvm_library", "kt_jvm_test")

kt_jvm_library(
    name = "hellobuild",
    srcs = [
        "HelloBuild.kt",
    ],
)

kt_jvm_test(
    name = "hellobuild-test",
    srcs = [
        "HelloBuildTest.kt",
    ],
    target_compatible_with = ["@platforms//os:osx"],
    test_class = "com.examples.kotlin.hellobuild.HelloBuildTest",
    deps = [
        ":hellobuild",
        "@junit4//jar",
        "@rules_kotlin//kotlin/compiler:kotlin-test",
    ],
)
