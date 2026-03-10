cc_binary(
    name = "websocket_test",
    srcs = [
        "client.c",
        "client.h",
        "run-client-server.cpp",
        "server.c",
        "server.h",
    ],
    target_compatible_with = select({
        "//bzl/conditions:wasm": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "@libwebsockets",
    ],
)
