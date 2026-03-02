load("//bzl/js:js.bzl", "js_library", "js_test")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "globals",
    srcs = [
        "globals.d.ts",
    ],
    deps = [
        "//reality/app/xr/js:buildif",
    ],
)

js_library(
    name = "animation-frame-helper",
    srcs = [
        "animation-frame-helper.ts",
    ],
)

js_library(
    name = "callback-manager",
    srcs = [
        "callback-manager.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:callbacks",
        "//reality/app/xr/js/src/types:module",
        "//reality/app/xr/js/src/types:pipeline",
    ],
)

js_library(
    name = "camera-loop",
    srcs = [
        "camera-loop.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:callback-manager",
        "//reality/app/xr/js/src:canvas",
        "//reality/app/xr/js/src:config",
        "//reality/app/xr/js/src:context-wrapper",
        "//reality/app/xr/js/src:document-visibility",
        "//reality/app/xr/js/src:gl-renderer",
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src:lifecycle",
        "//reality/app/xr/js/src:loop",
        "//reality/app/xr/js/src:offscreen-canvas",
        "//reality/app/xr/js/src:permissions",
        "//reality/app/xr/js/src:session-manager-desktop3d",
        "//reality/app/xr/js/src:session-manager-getusermedia",
        "//reality/app/xr/js/src:set-manager",
        "//reality/app/xr/js/src:write-emscripten",
        "//reality/app/xr/js/src/types:callbacks",
        "//reality/app/xr/js/src/types:module",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/types:xrcc",
    ],
)

js_library(
    name = "camera-providers",
    srcs = [
        "camera-providers.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:gl-renderer",
    ],
)

js_library(
    name = "canvas-screenshot",
    srcs = [
        "canvas-screenshot.js",
    ],
    deps = [
        "//reality/app/xr/js/src:canvas",
        "//reality/app/xr/js/src:factory",
    ],
)

js_library(
    name = "canvas",
    srcs = [
        "canvas.ts",
    ],
)

js_library(
    name = "offscreen-canvas",
    srcs = [
        "offscreen-canvas.ts",
    ],
)

js_library(
    name = "config",
    srcs = [
        "config.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:factory",
    ],
)

js_library(
    name = "constraints-helper",
    srcs = [
        "constraints-helper.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:config",
        "//reality/app/xr/js/src:js-device",
    ],
)

js_library(
    name = "context-wrapper",
    srcs = [
        "context-wrapper.js",
    ],
)

js_library(
    name = "deprecate",
    srcs = [
        "deprecate.ts",
    ],
)

js_library(
    name = "document-visibility",
    srcs = [
        "document-visibility.js",
    ],
)

js_library(
    name = "face-controller",
    srcs = [
        "face-controller.ts",
    ],
    deps = [
        "//reality/app/xr/js:xr-face-wasm",
        "//reality/app/xr/js/src:factory",
        "//reality/app/xr/js/src:quaternion",
        "//reality/app/xr/js/src/faces:load-models",
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:environment-update",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/types:xrfacecc",
        "//reality/engine/api:face.capnp-ts",
        "//reality/engine/api:mesh.capnp-ts",
        "//reality/engine/api/base:camera-environments.capnp-ts",
        "//reality/engine/api/base:camera-intrinsics.capnp-ts",
        "//reality/engine/api/base:geo-types.capnp-ts",
    ],
)

js_library(
    name = "factory",
    srcs = [
        "factory.ts",
    ],
)

js_library(
    name = "full-window-canvas",
    srcs = [
        "full-window-canvas.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:factory",
    ],
)

js_library(
    name = "geometry-conversions",
    srcs = [
        "geometry-conversions.ts",
    ],
)

js_library(
    name = "gl-renderer",
    srcs = [
        "gl-renderer.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:module",
        "//reality/app/xr/js/src/types:pipeline",
    ],
)

js_library(
    name = "js-device",
    srcs = [
        "js-device.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:config",
        "//reality/app/xr/js/src:factory",
        "//reality/app/xr/js/src:webxr-compatibility",
        "//reality/app/xr/js/src/devices:compatibility",
        "//reality/app/xr/js/src/devices:fingerprint",
        "//reality/app/xr/js/src/devices:override",
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:device",
        "//reality/engine/api/device:info.capnp-ts",
    ],
)

js_library(
    name = "jsxr",
    srcs = [
        "jsxr.ts",
    ],
    deps = [
        "//reality/app/xr/js:build-constants",
        "//reality/app/xr/js:xr-js-asm",
        "//reality/app/xr/js/src:camera-loop",
        "//reality/app/xr/js/src:camera-providers",
        "//reality/app/xr/js/src:canvas-screenshot",
        "//reality/app/xr/js/src:chunk-loader",
        "//reality/app/xr/js/src:cloudstudio-threejs-renderer",
        "//reality/app/xr/js/src:config",
        "//reality/app/xr/js/src:deprecate",
        "//reality/app/xr/js/src:full-window-canvas",
        "//reality/app/xr/js/src:gl-renderer",
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src:layers-controller",
        "//reality/app/xr/js/src:media-recorder",
        "//reality/app/xr/js/src:permissions",
        "//reality/app/xr/js/src:threejs-renderer",
        "//reality/app/xr/js/src:worldpoint-renderer",
        "//reality/app/xr/js/src:xr-aframe",
        "//reality/app/xr/js/src:xr-babylonjs",
        "//reality/app/xr/js/src:xr-playcanvas",
        "//reality/app/xr/js/src:xr-sumerian",
        "//reality/app/xr/js/src:yuv-pixels-array",
        "//reality/app/xr/js/src/devices:compatibility",
        "//reality/app/xr/js/src/devices:override",
        "//reality/app/xr/js/src/types:api",
        "//reality/app/xr/js/src/types:xrcc",
    ],
)

js_library(
    name = "layers-controller",
    srcs = [
        "layers-controller.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:factory",
        "//reality/app/xr/js/src:gl-renderer",
        "//reality/app/xr/js/src:permissions",
        "//reality/app/xr/js/src:pixel-transforms",
        "//reality/app/xr/js/src:quaternion",
        "//reality/app/xr/js/src:resources",
        "//reality/app/xr/js/src/semantics:options",
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:environment-update",
        "//reality/app/xr/js/src/types:layers",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/types:xrcc",
        "//reality/engine/api:semantics.capnp-ts",
        "//reality/engine/api/base:camera-environments.capnp-ts",
    ],
)

js_test(
    name = "layers-controller-test",
    main = [
        "layers-controller-test.ts",
    ],
    npm_rule = "@npm-jsxr//:npm-jsxr",
    target = "web",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "//bzl/js:chai-js",
        "//reality/app/xr/js/src:globals",
        "//reality/app/xr/js/src:layers-controller",
    ],
)

js_library(
    name = "lifecycle",
    srcs = [
        "lifecycle.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:pipeline",
    ],
)

js_test(
    name = "lifecycle-test",
    main = [
        "lifecycle-test.ts",
    ],
    target = "web",
    target_compatible_with = select({
        "//bzl/js:target-node": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "//bzl/js:chai-js",
        "//reality/app/xr/js/src:lifecycle",
    ],
)

js_library(
    name = "loop",
    srcs = [
        "loop.ts",
    ],
)

js_library(
    name = "media-recorder",
    srcs = [
        "media-recorder.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:factory",
        "//reality/app/xr/js/src:mp4-recorder",
        "//reality/app/xr/js/src:permissions",
        "//reality/app/xr/js/src/media:direct-recorder",
        "//reality/app/xr/js/src/media:worker",
    ],
)

js_library(
    name = "chunk-loader",
    srcs = [
        "chunk-loader.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:deprecate",
        "//reality/app/xr/js/src:resources",
        "//reality/app/xr/js/src/types:api",
        "//reality/app/xr/js/src/types:pipeline",
    ],
)

js_library(
    name = "mouse-to-touch-translator",
    srcs = [
        "mouse-to-touch-translator.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:interaction-provider",
    ],
)

js_library(
    name = "mp4-recorder",
    srcs = [
        "mp4-recorder.js",
    ],
    deps = [
        "//reality/app/xr/js/src/media:audio-mixer",
        "//reality/app/xr/js/src/media:direct-recorder",
        "//reality/app/xr/js/src/media:frame-compositor",
        "//reality/app/xr/js/src/media:transcoded-recorder",
        "//reality/app/xr/js/src/media:wasm-recorder",
    ],
)

js_library(
    name = "permissions-helper",
    srcs = [
        "permissions-helper.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:permissions",
        "//reality/app/xr/js/src:resources",
        "//reality/app/xr/js/src/types:common",
    ],
)

js_library(
    name = "permissions",
    srcs = [
        "permissions.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:factory",
    ],
)

js_test(
    name = "pixel-transforms-test",
    size = "small",
    main = [
        "pixel-transforms-test.ts",
    ],
    npm_rule = "@npm-jsxr//:npm-jsxr",
    deps = [
        "//bzl/js:chai-js",
        "//reality/app/xr/js/src:pixel-transforms",
    ],
)

js_library(
    name = "pixel-transforms",
    srcs = [
        "pixel-transforms.ts",
    ],
)

js_library(
    name = "prevent-default-touch",
    srcs = [
        "prevent-default-touch.js",
    ],
)

js_library(
    name = "quaternion",
    srcs = [
        "quaternion.ts",
    ],
)

# This library is re-used by omniscope.
# TODO(nb): export sensor data as a pipeline module from the main library, and consume it from
# omniscope that way.
js_library(
    name = "sensors",
    srcs = [
        "sensors.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:xrtrackingcc",
        "//reality/engine/api/request:sensor.capnp-ts",
    ],
)

js_library(
    name = "session-manager-desktop3d",
    srcs = [
        "session-manager-desktop3d.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:config",
        "//reality/app/xr/js/src:mouse-to-touch-translator",
    ],
)

js_library(
    name = "session-manager-getusermedia",
    srcs = [
        "session-manager-getusermedia.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:animation-frame-helper",
        "//reality/app/xr/js/src:config",
        "//reality/app/xr/js/src:constraints-helper",
        "//reality/app/xr/js/src:gl-renderer",
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src:permissions",
        "//reality/app/xr/js/src:permissions-helper",
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/types:xrcc",
    ],
)

js_library(
    name = "set-manager",
    srcs = [
        "set-manager.ts",
    ],
)

js_library(
    name = "threejs-renderer-webxr",
    srcs = [
        "threejs-renderer-webxr.js",
    ],
    deps = [
        "//reality/app/xr/js/src:permissions-helper",
    ],
)

js_library(
    name = "tracking-controller",
    srcs = [
        "tracking-controller.ts",
    ],
    deps = [
        "//reality/app/xr/js:xr-tracking-wasm",
        "//reality/app/xr/js/src:config",
        "//reality/app/xr/js/src:factory",
        "//reality/app/xr/js/src:gl-renderer",
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src:permissions",
        "//reality/app/xr/js/src:quaternion",
        "//reality/app/xr/js/src:sensors",
        "//reality/app/xr/js/src:write-emscripten",
        "//reality/app/xr/js/src/types:api",
        "//reality/app/xr/js/src/types:callbacks",
        "//reality/app/xr/js/src/types:common",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/types:recorder",
        "//reality/app/xr/js/src/types:xrcc",
        "//reality/app/xr/js/src/types:xrmodule",
        "//reality/app/xr/js/src/types:xrtrackingcc",
        "//reality/engine/api:reality.capnp-ts",
        "//reality/engine/api/device:info.capnp-ts",
        "//reality/engine/api/response:pose.capnp-ts",
        "//reality/shared/engine:image-targets",
    ],
)

js_library(
    name = "threejs-renderer",
    srcs = [
        "threejs-renderer.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:geometry-conversions",
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src:permissions-helper",
        "//reality/app/xr/js/src:threejs-support",
        "//reality/app/xr/js/src:webxr-compatibility",
        "//reality/app/xr/js/src/types:callbacks",
        "//reality/app/xr/js/src/types:layers",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:desktop-camera-manager",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:void-space",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-input-manager",
    ],
)

js_library(
    name = "webxr-compatibility",
    srcs = [
        "webxr-compatibility.ts",
    ],
)

js_library(
    name = "webxr-controller",
    srcs = [
        "webxr-controller.js",
    ],
    deps = [
        "//reality/app/xr/js/src:canvas",
        "//reality/app/xr/js/src:gl-renderer",
    ],
)

js_library(
    name = "worldpoint-renderer",
    srcs = [
        "worldpoint-renderer.ts",
    ],
)

js_library(
    name = "write-emscripten",
    srcs = [
        "write-emscripten.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:emscripten",
    ],
)

js_library(
    name = "xr-aframe",
    srcs = [
        "xr-aframe.js",
    ],
    deps = [
        "//reality/app/xr/js/src/aframe:layer-scene-primitive",
        "//reality/app/xr/js/src/aframe:xr-config-component",
        "//reality/app/xr/js/src/aframe:xr-layer-scene-component",
        "//reality/app/xr/js/src/aframe:xr-web-component",
    ],
)

js_library(
    name = "xr-babylonjs",
    srcs = [
        "xr-babylonjs.js",
    ],
)

js_library(
    name = "xr-constants",
    srcs = [
        "xr-constants.ts",
    ],
)

js_library(
    name = "xr-playcanvas",
    srcs = [
        "xr-playcanvas.js",
    ],
    deps = [
        "//reality/app/xr/js/src:canvas",
        "//reality/app/xr/js/src:factory",
        "//reality/app/xr/js/src/types:layers",
    ],
)

js_library(
    name = "xr-sumerian",
    srcs = [
        "xr-sumerian.js",
    ],
    deps = [
        "//reality/app/xr/js/src:canvas",
    ],
)

js_library(
    name = "cloudstudio-threejs-renderer",
    srcs = [
        "cloudstudio-threejs-renderer.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/xr:xr-ecs-types",
        "//reality/app/xr/js/src:geometry-conversions",
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src:permissions-helper",
        "//reality/app/xr/js/src:webxr-compatibility",
        "//reality/app/xr/js/src/types:callbacks",
        "//reality/app/xr/js/src/types:layers",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:desktop-camera-manager",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:void-space",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-input-manager",
    ],
)

js_library(
    name = "yuv-pixels-array",
    srcs = [
        "yuv-pixels-array.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:gl-renderer",
    ],
)

js_library(
    name = "threejs-support",
    srcs = [
        "threejs-support.ts",
    ],
)

js_library(
    name = "resources",
    srcs = [
        "resources.ts",
    ],
)
