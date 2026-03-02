load("//bzl/js:js.bzl", "js_binary", "js_cli", "js_library", "js_test")
load("//bzl/js:ts-declarations.bzl", "ts_declarations")

js_binary(
    name = "pre-built-binary",
    commonjs = 1,
    export_library = 1,
    externals = "eslint",
    main = [
        "pre-built-binary.ts",
    ],
    npm_rule = "@npm-eslint//:npm-eslint",
    target = "node",
)

js_cli(
    name = "cli-on-top",
    main = [
        "cli-on-top.ts",
    ],
    npm_rule = "@npm-eslint//:npm-eslint",
    deps = [
        "//bzl/examples/js/import:pre-built-binary",
    ],
)

js_binary(
    name = "wildcard-external",
    externals = "webpack, @babel/*",
    main = [
        "wildcard-external.ts",
    ],
    npm_rule = "@npm-webpack-build//:npm-webpack-build",
    target = "node",
)

js_test(
    name = "wildcard-external-test",
    size = "small",
    data = [
        ":wildcard-external",
    ],
    main = [
        "wildcard-external-test.ts",
    ],
)

js_test(
    name = "externalized-npm-test",
    size = "small",
    externalize_npm = True,
    main = [
        "externalized-npm-test.ts",
    ],
    npm_rule = "@npm-webpack-build//:npm-webpack-build",
    deps = [
        "//bzl/js:chai-js",
    ],
)

js_binary(
    name = "type-only-lib",
    export_library = 1,
    main = [
        "type-only-lib.ts",
    ],
)

ts_declarations(
    name = "type-only-lib-declarations",
    binary = ":type-only-lib",
)

js_binary(
    name = "type-only",
    main = [
        "type-only.ts",
    ],
    target = "node",
    deps = [
        "//bzl/examples/js/import:type-only-lib-declarations",
    ],
)

js_test(
    name = "type-only-validate-test",
    size = "small",
    data = [
        ":type-only",
    ],
    main = [
        "type-only-validate-test.ts",
    ],
    deps = [
        "//bzl/examples/js/import:type-only-lib",
        "//bzl/js:chai-js",
    ],
)

js_library(
    name = "transitive-definitions-dep",
    srcs = [
        "transitive-definitions-dep.ts",
    ],
)

js_binary(
    name = "transitive-definitions",
    export_library = 1,
    full_dts = 1,
    main = [
        "transitive-definitions.ts",
    ],
    deps = [
        "//bzl/examples/js/import:transitive-definitions-dep",
    ],
)

js_test(
    name = "transitive-definitions-test",
    size = "small",
    data = [
        ":transitive-definitions",
    ],
    main = [
        "transitive-definitions-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
    ],
)

filegroup(
    name = "circular-srcs",
    srcs = [
        "circular-dep-1.ts",
        "circular-dep-2.ts",
    ],
)

js_cli(
    name = "circular-cli",
    main = [
        "circular-cli.ts",
    ],
    deps = [
        "//bzl/examples/js/import:circular-dep-1",
    ],
)

js_library(
    name = "circular-lib",
    srcs = [
        "circular-dep-1.ts",
        "circular-dep-2.ts",
    ],
)

js_library(
    name = "circular-dep-1",
    srcs = [],
    deps = [
        ":circular-lib",
    ],
)

js_library(
    name = "circular-dep-2",
    srcs = [],
    deps = [
        ":circular-lib",
    ],
)
