load("//bzl/js:js.bzl", "js_library", "js_test")
load("//bzl/capnproto:capnproto.bzl", "capnp_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "xrapi-addon",
    srcs = [
        "xrapi-addon.cc",
    ],
    hdrs = [
        "xrapi-addon.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":xrapi-manager",
        "//bzl/android:native-app-glue",
        "//bzl/node:node-addons",
        "//bzl/node:node-addons-extra",
        "//c8:c8-log",
        "//c8:map",
        "//c8:string",
    ],
)

js_library(
    name = "xrapi-native",
    srcs = [
        "xrapi-native.ts",
    ],
    deps = [
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "xrapi-types",
    srcs = [
        "xrapi-types.ts",
    ],
)

js_library(
    name = "xrapi-bindings",
    srcs = [
        "xrapi-bindings.ts",
    ],
    deps = [
        "//c8/dom",
        "//c8/xrapi:callbacks",
        "//c8/xrapi:session",
        "//c8/xrapi:space",
        "//c8/xrapi:transform",
        "//c8/xrapi:view",
        "//c8/xrapi:webgl-binding",
        "//c8/xrapi:xr-input-source-event",
        "//c8/xrapi:xr-input-sources-change-event",
        "//c8/xrapi:xrapi-install",
        "//c8/xrapi:xrapi-layers",
        "//c8/xrapi:xrapi-proto.capnp-ts",
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "xrapi-emulated",
    srcs = [
        "xrapi-emulated.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/math",
        "//c8/xrapi:callbacks",
        "//c8/xrapi:pose",
        "//c8/xrapi:session",
        "//c8/xrapi:space",
        "//c8/xrapi:transform",
        "//c8/xrapi:webgl-binding",
        "//c8/xrapi:xr-input-source-event",
        "//c8/xrapi:xr-input-sources-change-event",
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "xrapi-install",
    srcs = [
        "xrapi-install.ts",
    ],
    deps = [
        "//c8/xrapi:xrapi-types",
    ],
)

cc_library(
    name = "xrapi-foveation",
    srcs = [
        "xrapi-foveation.cc",
    ],
    hdrs = [
        "xrapi-foveation.h",
    ],
    deps = [
        "//c8:c8-log",
        "//c8:vector",
        "//c8/xrapi/openxr:meta-foveation",
    ],
)

cc_library(
    name = "xrapi-space-warp",
    srcs = [
        "xrapi-space-warp.cc",
    ],
    hdrs = [
        "xrapi-space-warp.h",
    ],
    deps = [
        "//c8:c8-log",
        "//c8:vector",
        "//c8/pixels/opengl:gl-framebuffer",
        "//c8/pixels/opengl:gl-headers",
        "//c8/pixels/opengl:gl-texture",
        "//c8/xrapi/openxr",
        "//c8/xrapi/openxr:gl-graphics",
        "//c8/xrapi/openxr:gl-motion-estimation",
        "//c8/xrapi/openxr:helper",
    ],
)

cc_library(
    name = "xrapi-manager",
    srcs = [
        "xrapi-manager.cc",
    ],
    hdrs = [
        "xrapi-manager.h",
    ],
    deps = [
        ":xrapi-proto.capnp-cc",
        "//bzl/android:native-app-glue",
        "//bzl/node:node-addons-extra",
        "//c8:c8-log",
        "//c8:hmatrix",
        "//c8:quaternion",
        "//c8:string",
        "//c8/io:capnp-messages",
        "//c8/xrapi:xrapi-foveation",
        "//c8/xrapi:xrapi-space-warp",
        "//c8/xrapi/openxr",
        "//c8/xrapi/openxr:display-refresh-rate-fb",
        "//c8/xrapi/openxr:ext-performance-settings",
        "//c8/xrapi/openxr:gl-contexts",
        "//c8/xrapi/openxr:gl-graphics",
        "//c8/xrapi/openxr:gl-impl",
        "//c8/xrapi/openxr:gl-texture-auto-restore",
        "//c8/xrapi/openxr:helper",
    ],
)

capnp_library(
    name = "xrapi-proto",
    srcs = [
        "xrapi-proto.capnp",
    ],
)

js_library(
    name = "xr-input-sources-change-event",
    srcs = [
        "xr-input-sources-change-event.ts",
    ],
    deps = [
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "xr-input-source-event",
    srcs = [
        "xr-input-source-event.ts",
    ],
    deps = [
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "transform",
    srcs = [
        "transform.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/runtime/math:algorithms",
        "//c8/xrapi:xrapi-types",
    ],
)

js_test(
    name = "transform-test",
    size = "small",
    main = [
        "transform-test.ts",
    ],
    npm_rule = "@npm-rendering//:npm-rendering",
    deps = [
        "//bzl/js:chai-js",
        "//c8/xrapi:transform",
    ],
)

js_library(
    name = "pose",
    srcs = [
        "pose.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/math",
        "//c8/xrapi:transform",
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "space",
    srcs = [
        "space.ts",
    ],
    deps = [
        "//c8/xrapi:transform",
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "callbacks",
    srcs = [
        "callbacks.ts",
    ],
)

js_library(
    name = "webgl-binding",
    srcs = [
        "webgl-binding.ts",
    ],
    deps = [
        "//c8/xrapi:session",
        "//c8/xrapi:view",
        "//c8/xrapi:xrapi-layers",
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "xrapi-layers",
    srcs = [
        "xrapi-layers.ts",
    ],
    deps = [
        "//c8/xrapi:session",
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "session",
    srcs = [
        "session.ts",
    ],
    deps = [
        "//c8/xrapi:view",
        "//c8/xrapi:xrapi-types",
    ],
)

js_library(
    name = "view",
    srcs = [
        "view.ts",
    ],
    deps = [
        "//c8/xrapi:xrapi-types",
    ],
)
