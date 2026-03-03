load("//bzl/httpfileserver:http-file-server.bzl", "http_file_server")
load("//bzl/js:js.bzl", "js_binary", "js_library")

http_file_server(
    name = "serve",
    data = [
        "index.html",
        ":app-min",
        ":worker-min",
    ],
    root_redirect = "bzl/examples/web/app/index.html",
)

js_binary(
    name = "app-min",
    main = [
        "app.tsx",
    ],
    npm_rule = "@npm-bzl-examples//:npm-bzl-examples",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        ":localized-clock-view",
        ":redux-store",
        ":theme",
    ],
)

js_binary(
    name = "worker-min",
    srcs = [
    ],
    main = [
        "worker.ts",
    ],
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

js_library(
    name = "localized-clock-view",
    srcs = [
        "localized-clock-view.tsx",
    ],
    deps = [
        "time-actions",
    ],
)

js_library(
    name = "redux-store",
    srcs = [
        "redux-store.ts",
    ],
    deps = [
        "time-reducer",
    ],
)

js_library(
    name = "theme",
    srcs = [
        "theme.ts",
    ],
)

js_library(
    name = "time-actions",
    srcs = [
        "time-actions.ts",
    ],
    deps = [
        "//bzl/examples/web/lib:dispatchify",
        "//bzl/examples/web/lib:dispatchify-types",
    ],
)

js_library(
    name = "time-reducer",
    srcs = [
        "time-reducer.ts",
    ],
)
