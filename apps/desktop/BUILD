load("//bzl/js:js.bzl", "js_binary", "js_library")
load("//bzl/npmpackage:typecheck.bzl", "npm_typecheck")

package(default_visibility = ["//apps/desktop:__subpackages__"])

exports_files([
    "package.json",
    "package-lock.json",
])

npm_typecheck(name = "npm-desktop")

js_library(
    name = "constants",
    srcs = [
        "constants.ts",
    ],
)

js_library(
    name = "project-helpers",
    srcs = [
        "project-helpers.ts",
    ],
    deps = [
        "//reality/shared/desktop:unix-path",
    ],
)

js_library(
    name = "local-project-db",
    srcs = [
        "local-project-db.ts",
    ],
    deps = [
        "//apps/desktop:application-state",
        "//reality/shared/desktop:local-sync-types",
    ],
)

js_library(
    name = "errors",
    srcs = [
        "errors.ts",
    ],
    deps = [
        "//apps/desktop:json-response",
    ],
)

js_library(
    name = "json-response",
    srcs = [
        "json-response.ts",
    ],
)

js_library(
    name = "query-params",
    srcs = [
        "query-params.ts",
    ],
)

js_library(
    name = "application-state",
    srcs = [
        "application-state.ts",
    ],
)

js_library(
    name = "local-preferences",
    srcs = [
        "local-preferences.ts",
    ],
    deps = [
        "//apps/desktop:application-state",
    ],
)

js_library(
    name = "local-server",
    srcs = [
        "local-server.ts",
    ],
    deps = [
        "//apps/desktop:constants",
        "//apps/desktop/app/file-sync:install-build-package",
        "//apps/desktop/app/file-sync:run-commands",
        "//c8/cli:ip",
    ],
)

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
        "//apps/desktop/app:desktop-protocol",
    ],
)

js_library(
    name = "requests",
    srcs = [
        "requests.ts",
    ],
    deps = [
        "//apps/desktop:errors",
    ],
)

js_library(
    name = "stream-file-response",
    srcs = [
        "stream-file-response.ts",
    ],
    deps = [
        "//apps/desktop:errors",
    ],
)
