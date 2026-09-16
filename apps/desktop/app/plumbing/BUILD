load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "errors",
    srcs = [
        "errors.ts",
    ],
    deps = [
        "//apps/desktop/app/plumbing:json-response",
    ],
)

js_library(
    name = "json-response",
    srcs = [
        "json-response.ts",
    ],
)

js_library(
    name = "query-params",
    srcs = [
        "query-params.ts",
    ],
)

js_library(
    name = "requests",
    srcs = [
        "requests.ts",
    ],
    deps = [
        "//apps/desktop/app/plumbing:errors",
    ],
)

js_library(
    name = "stream-file-response",
    srcs = [
        "stream-file-response.ts",
    ],
    deps = [
        "//apps/desktop/app/plumbing:errors",
    ],
)
