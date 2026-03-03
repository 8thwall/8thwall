load("//bzl/js:js.bzl", "js_cli")

package(default_visibility = ["//visibility:public"])

js_cli(
    name = "generate-xrmodule-types",
    main = [
        "generate-xrmodule-types.ts",
    ],
    deps = [
        "//bzl/js/codegen:generate-ts-from-cpp",
    ],
)

js_cli(
    name = "generate-xrcc-types",
    main = [
        "generate-xrcc-types.ts",
    ],
    deps = [
        "//bzl/js/codegen:generate-ts-from-cpp",
    ],
)

js_cli(
    name = "generate-xrchunk-types",
    main = [
        "generate-xrchunk-types.ts",
    ],
    deps = [
        "//bzl/js/codegen:generate-ts-from-cpp",
    ],
)

js_cli(
    name = "blob-a-script",
    main = [
        "blob-a-script.ts",
    ],
)
