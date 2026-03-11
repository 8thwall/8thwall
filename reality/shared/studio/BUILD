load("//bzl/js:js.bzl", "js_library", "js_test")

package(default_visibility = ["//reality/cloud:shared"])

js_library(
    name = "ecs-config",
    srcs = [
        "ecs-config.ts",
    ],
)

js_library(
    name = "ecs-manifest",
    srcs = [
        "ecs-manifest.ts",
    ],
    deps = [
        "//reality/shared/studio:ecs-config",
    ],
)

js_library(
    name = "validate-ecs-manifest",
    srcs = [
        "validate-ecs-manifest.ts",
    ],
    deps = [
        "//reality/shared/studio:ecs-config",
        "//reality/shared/studio:ecs-manifest",
    ],
)

js_test(
    name = "validate-ecs-manifest-test",
    size = "small",
    main = [
        "validate-ecs-manifest-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//reality/shared/studio:ecs-manifest",
        "//reality/shared/studio:validate-ecs-manifest",
    ],
)

js_library(
    name = "load-ecs-manifest",
    srcs = [
        "load-ecs-manifest.ts",
    ],
    deps = [
        "//reality/shared/studio:ecs-manifest",
        "//reality/shared/studio:validate-ecs-manifest",
    ],
)

js_library(
    name = "fetch-asset-manifest",
    srcs = [
        "fetch-asset-manifest.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//c8/ecs/src/shared:asset-manifest",
        "//reality/shared:s3",
        "//reality/shared/studio:fetch-studio-global-entry",
    ],
)

js_library(
    name = "parse-script-srcs",
    srcs = [
        "parse-script-srcs.ts",
    ],
    esnext = 1,
    visibility = ["//visibility:public"],
    deps = [
        "//reality/shared/studio:build-info-comment",
    ],
)

js_test(
    name = "parse-script-srcs-test",
    size = "small",
    esnext = 1,
    main = [
        "parse-script-srcs-test.ts",
    ],
    npm_rule = "@npm-html-app-packager//:npm-html-app-packager",
    target = "node",
    webpack_mode = "development",
    deps = [
        "//bzl/js:chai-js",
        "//reality/shared/studio:parse-script-srcs",
    ],
)

js_library(
    name = "build-info-comment",
    srcs = [
        "build-info-comment.ts",
    ],
    visibility = ["//visibility:public"],
)

js_library(
    name = "fetch-studio-global-entry",
    srcs = [
        "fetch-studio-global-entry.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//reality/shared:dynamodb",
        "//reality/shared:typed-attributes",
    ],
)

js_test(
    name = "escape-asset-path-test",
    size = "small",
    main = [
        "escape-asset-path-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//reality/shared/studio:escape-asset-path",
    ],
)

js_library(
    name = "escape-asset-path",
    srcs = [
        "escape-asset-path.ts",
    ],
)

js_library(
    name = "runtime-version-constants",
    srcs = [
        "runtime-version-constants.ts",
    ],
)

js_library(
    name = "generate-url",
    srcs = [
        "generate-url.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:runtime-version",
        "//reality/shared/studio:ecs-manifest",
        "//reality/shared/studio:runtime-version-constants",
    ],
)

js_library(
    name = "splash-type",
    srcs = [
        "splash-type.ts",
    ],
)

js_library(
    name = "build-messaging",
    srcs = [
        "build-messaging.ts",
    ],
    visibility = [
        "//reality/app/nae:__subpackages__",
        "//reality/cloud:shared",
    ],
)

js_test(
    name = "parse-hosting-domain-test",
    size = "small",
    main = [
        "parse-hosting-domain-test.ts",
    ],
    npm_rule = "@npm-lambda//:npm-lambda",
    deps = [
        "//bzl/js:chai-js",
        "//reality/shared/studio:parse-hosting-domain",
    ],
)

js_library(
    name = "parse-hosting-domain",
    srcs = [
        "parse-hosting-domain.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
)
