load("//bzl/js:js.bzl", "js_binary", "js_library")

package(default_visibility = ["//c8/ecs:__subpackages__"])

exports_files(["index.html"])

js_binary(
    name = "demo",
    esnext = 1,
    main = [
        "demo.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/demo:scene",
        "//c8/ecs/src/runtime:application",
        "//c8/ecs/src/runtime:index",
    ],
)

js_library(
    name = "scene",
    srcs = [
        "scene.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)
