load("//bzl/js:js.bzl", "js_binary", "js_test")

js_binary(
    name = "class-example-es6",
    export_library = 1,
    main = [
        "class-example.ts",
    ],
    mangle = 0,
)

js_binary(
    name = "class-example-esnext",
    esnext = 1,
    export_library = 1,
    main = [
        "class-example.ts",
    ],
)

js_binary(
    name = "class-in-js",
    main = [
        "class-in-js.js",
    ],
    mangle = 0,
)

js_test(
    name = "class-test",
    size = "small",
    data = [
        ":class-example-es6",
        ":class-in-js",
    ],
    main = [
        "class-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
    ],
)
