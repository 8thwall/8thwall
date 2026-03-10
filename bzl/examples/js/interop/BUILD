load("//bzl/js:js.bzl", "js_binary", "js_cli", "js_library")

js_library(
    name = "capnp-style",
    srcs = ["export/capnp-style.js"],
)

js_library(
    name = "export",
    srcs = [
        "export/commonjs-default.js",
        "export/commonjs-named.js",
        "export/js-es6-default.js",
        "export/js-es6-named.js",
        "export/ts-es6-default.ts",
        "export/ts-es6-named.ts",
    ],
)

js_cli(
    name = "commonjs",
    commonjs = True,
    main = [
        "import/commonjs.js",
    ],
    deps = [
        ":capnp-style",
        ":export",
    ],
)

js_cli(
    name = "js-es6",
    commonjs = True,
    main = [
        "import/js-es6.js",
    ],
    deps = [
        ":capnp-style",
        ":export",
    ],
)

js_cli(
    name = "ts-es6",
    commonjs = True,
    main = [
        "import/ts-es6.ts",
    ],
    deps = [
        ":capnp-style",
        ":export",
    ],
)

js_binary(
    name = "node-library",
    export_library = True,
    main = [
        "export/ts-es6-named.ts",
    ],
    target = "node",
)

js_binary(
    name = "node-library-commonjs",
    commonjs = True,
    export_library = True,
    main = [
        "export/ts-es6-named.ts",
    ],
    target = "node",
)

js_cli(
    name = "ts-es6-nia",
    commonjs = True,
    main = [
        "import/ts-es6-nia.ts",
    ],
    deps = [
        ":capnp-style",
        ":export",
    ],
)

js_cli(
    name = "js-es6-nia",
    commonjs = True,
    main = [
        "import/js-es6-nia.js",
    ],
    deps = [
        ":capnp-style",
        ":export",
    ],
)
