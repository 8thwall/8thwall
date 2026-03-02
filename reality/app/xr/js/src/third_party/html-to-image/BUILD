load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "html-to-image",
    # NOTE(paris): Could be re-written to make each file a js_library.
    srcs = glob(
        [
            "src/**/*.js",
            "src/**/*.ts",
        ],
    ),
    deps = [
        "//reality/app/xr/js/src:canvas",
    ],
)
