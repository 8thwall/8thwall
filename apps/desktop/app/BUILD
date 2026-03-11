load("//bzl/js:js.bzl", "js_binary", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_binary(
    name = "start",
    esnext = 1,
    externals = "electron",
    main = [
        "start.ts",
    ],
    npm_rule = "@npm-desktop//:npm-desktop",
    target = "node",
    deps = [
        "//apps/desktop:application-state",
        "//apps/desktop/app:desktop-protocol",
        "//apps/desktop/app:menu",
        "//apps/desktop/app/desktop-app:deep-link",
        "//apps/desktop/app/desktop-app:permissions",
        "//apps/desktop/app/desktop-app:protocol",
        "//apps/desktop/app/desktop-app:register-on-open-url-handler",
        "//apps/desktop/app/desktop-app:register-second-instance-handler",
        "//apps/desktop/app/dev8-socket:dev8-socket-server",
        "//apps/desktop/app/file-sync:protocol",
        "//apps/desktop/app/file-watch:ports",
        "//apps/desktop/app/preferences:protocol",
    ],
)

js_binary(
    name = "preload",
    esnext = 1,
    externals = "electron",
    main = [
        "preload.ts",
    ],
    npm_rule = "@npm-desktop//:npm-desktop",
    deps = [
        "//apps/desktop/app:desktop-protocol",
        "//apps/desktop/app/file-watch:api",
        "//reality/shared/desktop:electron-api",
    ],
)

js_library(
    name = "desktop-protocol",
    srcs = [
        "desktop-protocol.ts",
    ],
    deps = [
        "//reality/shared/desktop:desktop-protocol-types",
    ],
)

js_library(
    name = "menu",
    srcs = [
        "menu.ts",
    ],
)

js_library(
    name = "resources",
    srcs = [
        "resources.ts",
    ],
)
