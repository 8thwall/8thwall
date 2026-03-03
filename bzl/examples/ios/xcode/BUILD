load("//bzl/xcode:xcode.bzl", "xcode_app", "xcode_project")

xcode_project(
    name = "hello-world-ios",
    project = "HelloWorld",
    deps = [
        "//bzl/examples:example-methods",
    ],
)

xcode_app(
    name = "hello-world",
    project = ":hello-world-ios",
    target_compatible_with = select({
        "@apple-developer-team//:niantic": [
            "@platforms//os:ios",
            "@platforms//cpu:arm64",
        ],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)
