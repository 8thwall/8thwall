load("//bzl/node:node-tools.bzl", "node_tool_wrapper")
load("//bzl/js:js.bzl", "js_binary", "js_library")
load("//bzl/js:bootstrap.bzl", "ts_bootstrap")
load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

config_setting(
    name = "release",
    values = {"compilation_mode": "opt"},
    visibility = ["//visibility:public"],
)

# Whether to perform webpack compilation for 'web' or 'node' runtime.
string_flag(
    name = "target",
    build_setting_default = "default",
    values = [
        "default",  # Choose the default for the target.
        "web",  # Always build for the 'web' target.
        "node",  # Always build for the 'node' target.
    ],
    visibility = ["//visibility:public"],
)

string_flag(
    name = "inspect",
    build_setting_default = "no",
    values = [
        "no",  # has no effect
        "yes",  # allow inspect but doesn't break on start
        "brk",  # allow inspect and break on start
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "target-default",
    flag_values = {
        ":target": "default",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "target-node",
    flag_values = {
        ":target": "node",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "target-web",
    flag_values = {
        ":target": "web",
    },
    visibility = ["//visibility:public"],
)

ts_bootstrap(
    name = "webpack-build-js",
    src = "webpack-build.ts",
)

node_tool_wrapper(
    name = "webpack-build",
    data = [
        "@npm-webpack-build//:npm-webpack-build",
    ],
    node = "//bzl/node:node",
    node_tool = ":webpack-build-js",
    visibility = ["//visibility:public"],
)

filegroup(
    name = "mocha-reporters",
    srcs = ["mocha-reporters.json"],
    visibility = ["//visibility:public"],
)

node_tool_wrapper(
    name = "mocha",
    data = [
        ":mocha-reporters",
        "@npm-mocha//:modules",
    ],
    node = "//bzl/node:node",
    node_tool = "@npm-mocha//:node_modules/mocha/bin/mocha",
    visibility = ["//visibility:public"],
)

js_binary(
    name = "chai-js-bin",
    testonly = 1,
    commonjs = 1,
    export_library = 1,
    main = [
        "chai-js-bin.ts",
    ],
    npm_rule = "@npm-mocha//:npm-mocha",
    target = "node",
    visibility = [
        "//visibility:public",
    ],
)

js_library(
    name = "chai-js",
    testonly = 1,
    srcs = [
        "chai-js.d.ts",
        "chai-js.js",
    ],
    data = [
        ":chai-js-bin",
    ],
    visibility = [
        "//visibility:public",
    ],
)

js_library(
    name = "no-experimental-warnings",
    srcs = [],
    data = [
        "no-experimental-warnings.js",
    ],
    node_opts = [
        # Future versions of node have the following flag
        # "--disable-warning=ExperimentWarning",
        "--require",
        "${RUNFILES_DIR/#[^\\/]*/./$RUNFILES_DIR}/_main/$(location :no-experimental-warnings.js)",
    ],
    visibility = ["//visibility:public"],
)

js_library(
    name = "vm-modules",
    srcs = [],
    node_opts = [
        "--experimental-vm-modules",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":no-experimental-warnings",
    ],
)

js_library(
    name = "fetch",
    srcs = [],
    node_opts = [
        "--experimental-fetch",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":no-experimental-warnings",
    ],
)

node_tool_wrapper(
    name = "fileloader",
    data = [],
    node = "//bzl/node:node",
    node_tool = "fileloader.js",
    visibility = ["//visibility:public"],
)
