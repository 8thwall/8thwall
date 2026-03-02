load("//bzl/node:node-tools.bzl", "node_tool_wrapper")
load("//bzl/js:js.bzl", "js_cli")

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

