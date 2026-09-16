load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "protocol",
    srcs = [
        "protocol.ts",
    ],
    deps = [
        "//apps/desktop/app/plumbing:errors",
        "//reality/shared/desktop:create-electron-url",
    ],
)

js_library(
    name = "register-on-open-url-handler",
    srcs = [
        "register-on-open-url-handler.ts",
    ],
    deps = [
        "//apps/desktop/app/native:deep-link",
    ],
)

js_library(
    name = "register-second-instance-handler",
    srcs = [
        "register-second-instance-handler.ts",
    ],
    deps = [
        "//apps/desktop/app/native:deep-link",
    ],
)

js_library(
    name = "deep-link",
    srcs = [
        "deep-link.ts",
    ],
    deps = [
        "//apps/desktop/app/native:desktop-protocol",
        "//apps/desktop/app/native:window-visibility",
    ],
)

js_library(
    name = "window-visibility",
    srcs = [
        "window-visibility.ts",
    ],
)

js_library(
    name = "permissions",
    srcs = [
        "permissions.ts",
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
