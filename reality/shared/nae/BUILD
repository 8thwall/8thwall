load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "nae-types",
    srcs = [
        "nae-types.ts",
    ],
    deps = [
        "//reality/shared/nae:nae-constants",
    ],
)

js_library(
    name = "nae-constants",
    srcs = [
        "nae-constants.ts",
    ],
)

js_library(
    name = "nae-utils",
    srcs = [
        "nae-utils.ts",
    ],
    deps = [
        "//reality/shared:dynamodb",
        "//reality/shared:typed-attributes",
        "//reality/shared/nae:nae-constants",
        "//reality/shared/nae:nae-types",
    ],
)

js_library(
    name = "historical-builds",
    srcs = [
        "historical-builds.ts",
    ],
    deps = [
        "//reality/shared:dynamodb",
        "//reality/shared:typed-attributes",
        "//reality/shared/nae:nae-types",
        "//reality/shared/nae:nae-utils",
    ],
)
