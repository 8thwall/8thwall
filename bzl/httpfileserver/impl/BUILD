load("//bzl/js:js.bzl", "js_cli", "js_library")

js_library(
    name = "cert",
    srcs = [
        "cert.ts",
    ],
)

js_cli(
    name = "http-file-server",
    main = [
        "http-file-server.ts",
    ],
    npm_rule = "@npm-bzl-httpfileserver//:npm-bzl-httpfileserver",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":cert",
    ],
)
