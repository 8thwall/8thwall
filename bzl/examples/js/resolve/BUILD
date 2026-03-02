load("//bzl/js:js.bzl", "js_binary", "js_cli", "js_library", "js_test")
load("//bzl/npmpackage:typecheck.bzl", "npm_typecheck")

exports_files([
    "package.json",
    "package-lock.json",
])

npm_typecheck(name = "npm-examples-js-resolve")

js_cli(
    name = "transitive",
    main = [
        "transitive.ts",
    ],
    npm_rule = "@npm-examples-js-resolve//:npm-examples-js-resolve",
)

js_cli(
    name = "transitive-ts",
    main = ["transitive.ts"],
    npm_rule = "@npm-examples-js-resolve//:npm-examples-js-resolve",
)

js_cli(
    name = "polyfill-cli",
    main = [
        "polyfill.ts",
    ],
)

js_binary(
    name = "polyfill-binary",
    main = ["polyfill.ts"],
    polyfill = "path, buffer",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

js_binary(
    name = "polyfill-binary-none",
    main = ["polyfill.ts"],
    polyfill = "none",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

js_binary(
    name = "polyfill-binary-node",
    main = ["polyfill.ts"],
    target = "node",
)

js_binary(
    name = "external",
    externals = "is-buffer",
    main = ["transitive.ts"],
    npm_rule = "@npm-examples-js-resolve//:npm-examples-js-resolve",
    target = "node",
)

js_library(
    name = "unique",
    srcs = [
        "unique.ts",
    ],
)

js_test(
    name = "unique-test",
    size = "small",
    main = [
        "unique-test.ts",
    ],
    deps = [
        "//bzl/examples/js/resolve:unique",
    ],
)
