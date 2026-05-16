load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "dev8-socket-server",
    srcs = [
        "dev8-socket-server.ts",
    ],
    deps = [
        "//apps/desktop/app/dev8-socket:listeners",
        "//c8/ecs/src/shared:debug-messaging",
    ],
)

js_library(
    name = "api",
    srcs = [
        "api.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:debug-messaging",
        "//reality/shared:listener-pool",
        "//reality/shared/desktop:electron-api",
    ],
)

js_library(
    name = "listeners",
    srcs = [
        "listeners.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:debug-messaging",
        "//reality/shared:listener-pool",
    ],
)

js_library(
    name = "ports",
    srcs = [
        "ports.ts",
    ],
    deps = [
        "//apps/desktop/app/dev8-socket:listeners",
        "//c8/ecs/src/shared:debug-messaging",
    ],
)
