load("//bzl/js:js.bzl", "js_cli", "js_library", "js_test")

js_cli(
    name = "inlinerjs",
    esnext = True,
    main = [
        "inlinerjs.ts",
    ],
    npm_rule = "@npm-webpack-build//:npm-webpack-build",
    deps = [
        "//bzl/inlinerjs:attribute-sort-order",
        "//bzl/inlinerjs:sort-string-list",
        "//c8/cli:args",
    ],
)

js_library(
    name = "attribute-sort-order",
    srcs = [
        "attribute-sort-order.ts",
    ],
)

js_library(
    name = "sort-string-list",
    srcs = [
        "sort-string-list.ts",
    ],
)

js_test(
    name = "sort-string-list-test",
    size = "small",
    main = [
        "sort-string-list-test.ts",
    ],
    deps = [
        "//bzl/inlinerjs:sort-string-list",
        "//bzl/js:chai-js",
    ],
)
