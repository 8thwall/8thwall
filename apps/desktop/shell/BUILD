load("//bzl/js:js.bzl", "js_binary", "js_library")

package(default_visibility = [
    "//apps/desktop/shell:__subpackages__",
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
        "//apps/desktop/core:application-state",
        "//apps/desktop/core:desktop-protocol",
        "//apps/desktop/dev8-socket:ports",
        "//apps/desktop/file-sync:protocol",
        "//apps/desktop/file-watch:ports",
        "//apps/desktop/image-targets:protocol",
        "//apps/desktop/preferences:protocol",
        "//apps/desktop/shell:deep-link",
        "//apps/desktop/shell:menu",
        "//apps/desktop/shell:permissions",
        "//apps/desktop/shell:protocol",
        "//apps/desktop/shell:register-on-open-url-handler",
        "//apps/desktop/shell:register-second-instance-handler",
        "//apps/desktop/system-log:ports",
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
        "//apps/desktop/core:desktop-protocol",
        "//apps/desktop/dev8-socket:api",
        "//apps/desktop/file-watch:api",
        "//apps/desktop/system-log:api",
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
        "//apps/desktop/core:desktop-protocol",
        "//apps/desktop/shell:window-visibility",
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
        "//apps/desktop/transport:errors",
        "//reality/shared/desktop:create-electron-url",
    ],
)

js_library(
    name = "register-on-open-url-handler",
    srcs = [
        "register-on-open-url-handler.ts",
    ],
    deps = [
        "//apps/desktop/shell:deep-link",
    ],
)

js_library(
    name = "register-second-instance-handler",
    srcs = [
        "register-second-instance-handler.ts",
    ],
    deps = [
        "//apps/desktop/shell:deep-link",
    ],
)

js_library(
    name = "window-visibility",
    srcs = [
        "window-visibility.ts",
    ],
)
