load("//bzl/js:js.bzl", "js_binary", "js_library")

package(default_visibility = [
    "//apps/desktop/src/shell:__subpackages__",
])

js_binary(
    name = "start",
    esnext = 1,
    externals = "electron, sharp",
    main = [
        "start.ts",
    ],
    npm_rule = "@npm-desktop//:npm-desktop",
    target = "node",
    deps = [
        "//apps/desktop/src/core:application-state",
        "//apps/desktop/src/core:desktop-protocol",
        "//apps/desktop/src/core:resources",
        "//apps/desktop/src/dev8-socket:ports",
        "//apps/desktop/src/file-sync:protocol",
        "//apps/desktop/src/file-watch:ports",
        "//apps/desktop/src/image-targets:protocol",
        "//apps/desktop/src/preferences:protocol",
        "//apps/desktop/src/shell:deep-link",
        "//apps/desktop/src/shell:menu",
        "//apps/desktop/src/shell:permissions",
        "//apps/desktop/src/shell:protocol",
        "//apps/desktop/src/shell:register-on-open-url-handler",
        "//apps/desktop/src/shell:register-second-instance-handler",
        "//apps/desktop/src/system-log:ports",
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
        "//apps/desktop/src/core:desktop-protocol",
        "//apps/desktop/src/dev8-socket:api",
        "//apps/desktop/src/file-watch:api",
        "//apps/desktop/src/system-log:api",
        "//reality/shared/desktop:electron-api",
    ],
)

js_library(
    name = "menu",
    srcs = [
        "menu.ts",
    ],
)

js_library(
    name = "deep-link",
    srcs = [
        "deep-link.ts",
    ],
    deps = [
        "//apps/desktop/src/core:desktop-protocol",
        "//apps/desktop/src/shell:window-visibility",
    ],
)

js_library(
    name = "permissions",
    srcs = [
        "permissions.ts",
    ],
)

js_library(
    name = "protocol",
    srcs = [
        "protocol.ts",
    ],
    deps = [
        "//apps/desktop/src/core:resources",
        "//apps/desktop/src/transport:errors",
        "//reality/shared/desktop:create-electron-url",
    ],
)

js_library(
    name = "register-on-open-url-handler",
    srcs = [
        "register-on-open-url-handler.ts",
    ],
    deps = [
        "//apps/desktop/src/shell:deep-link",
    ],
)

js_library(
    name = "register-second-instance-handler",
    srcs = [
        "register-second-instance-handler.ts",
    ],
    deps = [
        "//apps/desktop/src/shell:deep-link",
    ],
)

js_library(
    name = "window-visibility",
    srcs = [
        "window-visibility.ts",
    ],
)
