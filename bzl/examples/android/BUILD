package(default_visibility = ["//visibility:public"])

load("@build_bazel_rules_android//android:rules.bzl", "android_binary")

android_binary(
    name = "hello-android",
    srcs = glob([
        "*.java",
    ]),
    custom_package = "com.nianticlabs.bzl.examples.android",
    manifest = "AndroidManifest.xml",
    resource_files = glob(["res/**"]),
    deps = [
        ":example-methods-jni",
        "@androidsdk//com.android.support:appcompat-v7-25.0.1",
    ],
)

cc_library(
    name = "example-methods-jni",
    srcs = [
        "example-methods-jni.cc",
    ],
    deps = [
        "//bzl/examples:example-methods",
    ],
    # Add alwayslink to ensure that the Android .so dynamic library includes
    # all of the symbols in this cc_library.
    alwayslink = 1,
)
