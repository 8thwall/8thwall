load("//bzl/js:js.bzl", "js_binary")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
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
        "//apps/desktop/app/dev8-socket:ports",
        "//apps/desktop/app/file-sync:protocol",
        "//apps/desktop/app/file-watch:ports",
        "//apps/desktop/app/image-targets:protocol",
        "//apps/desktop/app/native:deep-link",
        "//apps/desktop/app/native:desktop-protocol",
        "//apps/desktop/app/native:menu",
        "//apps/desktop/app/native:permissions",
        "//apps/desktop/app/native:protocol",
        "//apps/desktop/app/native:register-on-open-url-handler",
        "//apps/desktop/app/native:register-second-instance-handler",
        "//apps/desktop/app/preferences:protocol",
        "//apps/desktop/app/storage:application-state",
        "//apps/desktop/app/system-log:ports",
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
        "//apps/desktop/app/dev8-socket:api",
        "//apps/desktop/app/file-watch:api",
        "//apps/desktop/app/native:desktop-protocol",
        "//apps/desktop/app/system-log:api",
        "//reality/shared/desktop:electron-api",
    ],
)
