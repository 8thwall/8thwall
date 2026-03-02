load("//bzl/httpfileserver:http-file-server.bzl", "http_file_server")
load("//bzl/js:js.bzl", "js_binary", "js_library")
load("//bzl/wasm:wasm.bzl", "wasm_binary")

http_file_server(
    name = "serve",
    data = [
        "index.html",
        ":chunk1",
        ":chunk2",
        ":core",
    ],
)

js_binary(
    name = "core",
    esnext = 1,
    main = [
        "core.ts",
    ],
    deps = [
        "//bzl/examples/js/split:log",
    ],
)

js_binary(
    name = "chunk1",
    esnext = 1,
    export_library = 1,
    main = [
        "chunk1.ts",
    ],
    deps = [
        "//bzl/examples/js/split:log",
    ],
)

js_binary(
    name = "chunk2",
    esnext = 1,
    export_library = 1,
    main = [
        "chunk2.ts",
    ],
    deps = [
        ":chunk2-wasm",
        "//bzl/examples/js/split:log",
    ],
)

BROWSER_ONLY = select({
    "//bzl/js:target-node": ["@platforms//:incompatible"],
    "//conditions:default": [],
})

js_library(
    name = "log",
    srcs = [
        "log.ts",
    ],
    target_compatible_with = BROWSER_ONLY,
)

wasm_binary(
    name = "chunk2-wasm",
    cc_binary = ":chunk2-cc",
    emopts = [
        "ALLOW_MEMORY_GROWTH=0",
        "ENVIRONMENT=web",
        "EXPORT_ES6=1",
        "EXPORT_NAME=ECS",
        "EXPORTED_FUNCTIONS=[_malloc,_free]",
        "EXPORTED_RUNTIME_METHODS=[writeArrayToMemory,lengthBytesUTF8,stringToUTF8]",
        "FETCH=0",
        "MODULARIZE=1",
        "SPLIT_MEMORY=0",
        "TOTAL_MEMORY=64mb",
        "USE_ES6_IMPORT_META=0",
        "WASM_BIGINT=1",
    ],
    html = 0,
    single_file = 1,
)

cc_binary(
    name = "chunk2-cc",
    srcs = [
        "chunk2.cc",
    ],
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:wasm": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//c8:symbol-visibility",
    ],
)
