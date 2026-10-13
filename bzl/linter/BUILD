load("//bzl/js:js.bzl", "js_cli", "js_library")
load("//bzl/node:node-tools.bzl", "node_tool_wrapper")

node_tool_wrapper(
    name = "eslint",
    data = [
        "@npm-eslint//:modules",
    ],
    env = {
        "NODE_PATH": "${RUNFILES_DIR}/npm-eslint/node_modules",
    },
    node = "//bzl/node:node",
    node_tool = "@npm-eslint//:node_modules/eslint/bin/eslint.js",
    visibility = ["//visibility:public"],
)

js_cli(
    name = "eslint-here",
    externalize_npm = 1,
    main = [
        "eslint-here.ts",
    ],
    npm_rule = "@npm-eslint//:npm-eslint",
    deps = [
        "//bzl/linter:eslint-diff",
    ],
)

js_library(
    name = "eslint-diff",
    srcs = [
        "eslint-diff.ts",
    ],
)
