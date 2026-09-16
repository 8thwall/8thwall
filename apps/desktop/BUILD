load("//bzl/js:js.bzl", "js_binary")
load("//bzl/npmpackage:typecheck.bzl", "npm_typecheck")

package(default_visibility = ["//apps/desktop:__subpackages__"])

exports_files([
    "package.json",
    "package-lock.json",
])

npm_typecheck(name = "npm-desktop")

js_binary(
    name = "builder",
    commonjs = 1,
    esnext = 1,
    export_library = 1,
    externals = "*",
    main = [
        "builder.config.ts",
    ],
    npm_rule = "@npm-desktop//:npm-desktop",
    target = "node",
    deps = [
        "//apps/desktop/src/core:desktop-protocol",
    ],
)
