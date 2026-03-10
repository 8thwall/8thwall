load("//bzl/js:js.bzl", "js_binary", "js_library", "js_test")
load("//bzl/js:ts-declarations.bzl", "ts_declarations")

package(
    default_visibility = [
        "//c8/ecs:__pkg__",
        "//c8/ecs/demo:__subpackages__",
        "//c8/ecs/src/runtime:__subpackages__",
        "//c8/ecs/src/shared:__subpackages__",
        "//c8/ecs/tools:__subpackages__",
    ],
)

js_binary(
    name = "runtime",
    esnext = 1,
    main = [
        "runtime-entry.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/src/runtime:application",
        "//c8/ecs/src/runtime:index",
        "//c8/ecs/src/runtime:set-three",
        "//c8/ecs/src/shared:resources",
    ],
)

js_library(
    name = "3d-ui",
    srcs = [
        "3d-ui.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:3d-ui-translate",
        "//c8/ecs/src/runtime:8mesh",
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:dispose",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:ui",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:fonts",
        "//c8/ecs/src/shared:msdf-font-type",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:three-layers",
        "//c8/ecs/src/shared:ui-constants",
        "//c8/ecs/src/shared:ui-nine-slice",
    ],
)

js_library(
    name = "animation",
    srcs = [
        "animation.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/shared:angle-conversion",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "api",
    srcs = [
        "api.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:animation",
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:audio",
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:core-api",
        "//c8/ecs/src/runtime:disabled",
        "//c8/ecs/src/runtime:entity-types",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:fly-controller",
        "//c8/ecs/src/runtime:init-ready",
        "//c8/ecs/src/runtime:map-types",
        "//c8/ecs/src/runtime:orbit-controls",
        "//c8/ecs/src/runtime:particles",
        "//c8/ecs/src/runtime:physics",
        "//c8/ecs/src/runtime:video",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/runtime:xr",
    ],
)

js_library(
    name = "application",
    srcs = [
        "application.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:effects-manager-types",
        "//c8/ecs/src/runtime:init-ready",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:motion-render-pass",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-create",
        "//c8/ecs/src/shared:application-type",
        "//c8/ecs/src/shared:get-camera-from-scene-graph",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:three-layers",
    ],
)

js_library(
    name = "asm",
    srcs = [
        "asm.ts",
        ":ecs-asm",
    ],
    deps = [
        "//c8/ecs:ecs-asm",
        "//c8/ecs/gen:asm-types",
    ],
)

js_library(
    name = "asset-manager",
    srcs = [
        "asset-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:asset-manifest",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "assets",
    srcs = [
        "assets.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asset-manager",
    ],
)

js_library(
    name = "attribute",
    srcs = [
        "attribute.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:cursor",
        "//c8/ecs/src/runtime:memory",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:extended-schema",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "audio-manager",
    srcs = [
        "audio-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:audio-constants",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "audio",
    srcs = [
        "audio.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:audio-manager",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "built-in-systems",
    srcs = [
        "built-in-systems.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:audio-system",
        "//c8/ecs/src/runtime/systems:camera-system",
        "//c8/ecs/src/runtime/systems:face-system",
        "//c8/ecs/src/runtime/systems:geometry-system",
        "//c8/ecs/src/runtime/systems:gltf-system",
        "//c8/ecs/src/runtime/systems:hidden-system",
        "//c8/ecs/src/runtime/systems:hider-material-system",
        "//c8/ecs/src/runtime/systems:image-target-system",
        "//c8/ecs/src/runtime/systems:light-systems",
        "//c8/ecs/src/runtime/systems:material-system",
        "//c8/ecs/src/runtime/systems:shadow-material-system",
        "//c8/ecs/src/runtime/systems:splat-system",
        "//c8/ecs/src/runtime/systems:transform-system",
        "//c8/ecs/src/runtime/systems:ui-system",
        "//c8/ecs/src/runtime/systems:unlit-material-system",
        "//c8/ecs/src/runtime/systems:video-material-system",
        "//c8/ecs/src/runtime/systems:video-system",
        "//c8/ecs/src/shared:geometry-constants",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "camera-events",
    srcs = [
        "camera-events.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:event-ids",
    ],
)

js_library(
    name = "camera-manager",
    srcs = [
        "camera-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "camera",
    srcs = [
        "camera.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:three-layers",
    ],
)

js_library(
    name = "components",
    srcs = [
        "components.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/components:audio-component",
        "//c8/ecs/src/runtime/components:camera-component",
        "//c8/ecs/src/runtime/components:face-component",
        "//c8/ecs/src/runtime/components:geometry-components",
        "//c8/ecs/src/runtime/components:gltf-component",
        "//c8/ecs/src/runtime/components:hidden-component",
        "//c8/ecs/src/runtime/components:image-target-component",
        "//c8/ecs/src/runtime/components:light-component",
        "//c8/ecs/src/runtime/components:material-components",
        "//c8/ecs/src/runtime/components:persistent-component",
        "//c8/ecs/src/runtime/components:shadow-component",
        "//c8/ecs/src/runtime/components:splat-component",
        "//c8/ecs/src/runtime/components:transform-components",
        "//c8/ecs/src/runtime/components:ui-component",
        "//c8/ecs/src/runtime/components:video-component",
    ],
)

js_library(
    name = "constants",
    srcs = [
        "constants.ts",
    ],
)

js_library(
    name = "create-scene-background",
    srcs = [
        "create-scene-background.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:effects-manager-types",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/shared:sky-constants",
    ],
)

js_library(
    name = "cursor",
    srcs = [
        "cursor.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:string-storage",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "disabled",
    srcs = [
        "disabled.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:cursor",
        "//c8/ecs/src/runtime:parent",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "entity",
    srcs = [
        "entity.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:dispose",
        "//c8/ecs/src/runtime:global-callbacks",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:object-hierarchy",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "event-ids",
    srcs = [
        "event-ids.ts",
    ],
)

js_library(
    name = "events",
    srcs = [
        "events.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "fly-controller",
    srcs = [
        "fly-controller.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime/math:quat",
        "//c8/ecs/src/runtime/math:vec3",
    ],
)

js_library(
    name = "get-gltf-vertices",
    srcs = [
        "get-gltf-vertices.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three-merge-vertices",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/shared:mesh-optimizer-constants",
    ],
)

js_library(
    name = "global-callbacks",
    srcs = [
        "global-callbacks.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "index",
    srcs = [
        "index.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:api",
    ],
)

js_library(
    name = "init-ready",
    srcs = [
        "init-ready.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:yoga",
    ],
)

js_library(
    name = "input-events",
    srcs = [
        "input-events.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:input-types",
    ],
)

js_library(
    name = "input-manager",
    srcs = [
        "input-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:input-types",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "input",
    srcs = [
        "input.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:input-events",
        "//c8/ecs/src/runtime:pointer-events",
        "//c8/ecs/src/runtime:ui-events",
    ],
)

js_library(
    name = "lights",
    srcs = [
        "lights.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:lights-types",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "list-data",
    srcs = [
        "list-data.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:constants",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "loaders",
    srcs = [
        "loaders.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/shared:resources",
    ],
)

js_library(
    name = "memory",
    srcs = [
        "memory.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:validate",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "orbit-controls",
    srcs = [
        "orbit-controls.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:core-api",
    ],
)

js_library(
    name = "parent",
    srcs = [
        "parent.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:constants",
        "//c8/ecs/src/runtime:list-data",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "particles",
    srcs = [
        "particles.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:loaders",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/runtime/math:mat4",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "persistent",
    srcs = [
        "persistent.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "physics-events",
    srcs = [
        "physics-events.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:constants",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:list-data",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "physics",
    srcs = [
        "physics.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:attribute",
        "//c8/ecs/src/runtime:constants",
        "//c8/ecs/src/runtime:memory",
        "//c8/ecs/src/runtime:physics-events",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:physic-constants",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "plugin-entry",
    srcs = [
        "plugin-entry.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:index",
    ],
)

js_library(
    name = "pointer-events",
    srcs = [
        "pointer-events.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:pointer-events-types",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime/math:vec3",
        "//c8/ecs/src/shared:custom-sorting",
        "//c8/ecs/src/shared:eid-operations",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "query",
    srcs = [
        "query.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:constants",
        "//c8/ecs/src/runtime:list-data",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "registry",
    srcs = [
        "registry.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:attribute",
        "//c8/ecs/src/runtime:disabled",
        "//c8/ecs/src/runtime:physics",
        "//c8/ecs/src/runtime:query",
        "//c8/ecs/src/runtime:state-machine",
        "//c8/ecs/src/runtime:state-machine-types",
        "//c8/ecs/src/runtime:system",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:extended-schema",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "set-three",
    srcs = [
        "set-three.ts",
    ],
)

js_library(
    name = "state-definer",
    srcs = [
        "state-definer.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:state-machine-types",
    ],
)

js_library(
    name = "state-machine-types",
    srcs = [
        "state-machine-types.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:entity-types",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "state-machine",
    srcs = [
        "state-machine.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:state-definer",
        "//c8/ecs/src/runtime:state-machine-types",
        "//c8/ecs/src/runtime:stategroup-definer",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "stategroup-definer",
    srcs = [
        "stategroup-definer.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:state-definer",
        "//c8/ecs/src/runtime:state-machine-types",
    ],
)

js_library(
    name = "string-storage",
    srcs = [
        "string-storage.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:instanced",
    ],
)

js_library(
    name = "system",
    srcs = [
        "system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:constants",
        "//c8/ecs/src/runtime:cursor",
        "//c8/ecs/src/runtime:list-data",
        "//c8/ecs/src/runtime:memory",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "three-merge-vertices",
    srcs = [
        "three-merge-vertices.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
    ],
)

js_library(
    name = "three-side",
    srcs = [
        "three-side.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
    ],
)

js_library(
    name = "three-wrapping",
    srcs = [
        "three-wrapping.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
    ],
)

js_library(
    name = "three",
    srcs = [
        "three.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//c8/ecs/src/runtime:three-types",
    ],
)

js_library(
    name = "time",
    srcs = [
        "time.ts",
    ],
)

js_library(
    name = "transform",
    srcs = [
        "transform.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "types",
    srcs = [
        "types.ts",
    ],
)

js_library(
    name = "ui-types",
    srcs = [
        "ui-types.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "ui",
    srcs = [
        "ui.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//c8/ecs/src/runtime:ui-types",
        "//c8/ecs/src/runtime:yoga",
        "//c8/ecs/src/shared:flex-styles",
    ],
)

js_library(
    name = "validate",
    srcs = [
        "validate.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "xr",
    srcs = [
        "xr.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/xr:xr-face-components",
    ],
)

js_library(
    name = "yoga",
    srcs = [
        "yoga.ts",
    ],
)

js_library(
    name = "8mesh",
    srcs = [
        "8mesh.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh:three-mesh-ui",
    ],
)

genrule(
    name = "ecs-asm",
    outs = ["ecs-asm.ts"],
    cmd = "echo \"import ECS from '@repo/c8/ecs/ecs-asm';export default ECS;\" > $@",
)

js_library(
    name = "events-types",
    srcs = [
        "events-types.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "pointer-events-types",
    srcs = [
        "pointer-events-types.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:three-types",
    ],
)

js_library(
    name = "scene-types",
    srcs = [
        "scene-types.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "camera-manager-types",
    srcs = [],
    deps = [
        "three-types",
    ],
)

js_library(
    name = "three-types",
    srcs = [
        "camera-manager-types.ts",
        "lights-types.ts",
        "object-3d-extended.ts",
        "three-types.ts",
        "user-data.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "world-create",
    srcs = [
        "world-create.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:audio-manager",
        "//c8/ecs/src/runtime:built-in-systems",
        "//c8/ecs/src/runtime:camera-manager",
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:effects-manager",
        "//c8/ecs/src/runtime:entity",
        "//c8/ecs/src/runtime:entity-impl",
        "//c8/ecs/src/runtime:entity-types",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:input-events",
        "//c8/ecs/src/runtime:input-manager",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:media-manager",
        "//c8/ecs/src/runtime:parent",
        "//c8/ecs/src/runtime:physics-events",
        "//c8/ecs/src/runtime:pointer-events",
        "//c8/ecs/src/runtime:prefab-entity",
        "//c8/ecs/src/runtime:raycast",
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:scene-handle",
        "//c8/ecs/src/runtime:scene-types",
        "//c8/ecs/src/runtime:spawn-into-object",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:time",
        "//c8/ecs/src/runtime:transform",
        "//c8/ecs/src/runtime:transform-manager",
        "//c8/ecs/src/runtime:ui-events-raycaster",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/xr:xr-manager",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "world",
    srcs = [
        "entity-types.ts",
        "world.ts",
        "world-attribute.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:audio-controls-types",
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:effects-manager-types",
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:input-types",
        "//c8/ecs/src/runtime:pointer-events-types",
        "//c8/ecs/src/runtime:raycast-types",
        "//c8/ecs/src/runtime:scene-types",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:time",
        "//c8/ecs/src/runtime:transform-manager-types",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/runtime/xr:xr-manager-types",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "input-types",
    srcs = [
        "input-types.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "motion-render-pass",
    srcs = [
        "motion-render-pass.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:motion-pass-shaders",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
    ],
)

js_library(
    name = "motion-pass-shaders",
    srcs = [
        "motion-pass-shaders.ts",
    ],
)

js_library(
    name = "matrix-refresh",
    srcs = [
        "matrix-refresh.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
    ],
)

js_library(
    name = "spawn-into-object",
    srcs = [
        "spawn-into-object.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:apply-update",
        "//c8/ecs/src/runtime:prefab-entity",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "apply-update",
    srcs = [
        "apply-update.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:disabled",
        "//c8/ecs/src/runtime:physics",
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:audio-constants",
        "//c8/ecs/src/shared:get-ui-attributes",
        "//c8/ecs/src/shared:material-constants",
        "//c8/ecs/src/shared:physic-constants",
        "//c8/ecs/src/shared:resource",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "scene-handle",
    srcs = [
        "scene-handle.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:entity",
        "//c8/ecs/src/runtime:persistent",
        "//c8/ecs/src/runtime:prefab-entity",
        "//c8/ecs/src/runtime:scene-environment",
        "//c8/ecs/src/runtime:scene-fog",
        "//c8/ecs/src/runtime:scene-sky",
        "//c8/ecs/src/runtime:scene-types",
        "//c8/ecs/src/runtime:spawn-into-object",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:get-camera-from-scene-graph",
        "//c8/ecs/src/shared:object-hierarchy",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "test-env",
    testonly = 1,
    srcs = [
        "test-env.ts",
    ],
)

ts_declarations(
    name = "test-runtime-types",
    binary = ":test-runtime-bin",
)

js_binary(
    name = "test-runtime-bin",
    commonjs = 1,
    esnext = 1,
    export_library = 1,
    full_dts = 1,
    main = [
        "test-runtime-bin.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/src/runtime:api",
        "//c8/ecs/src/runtime:set-three",
    ],
)

js_library(
    name = "test-runtime-lib",
    testonly = 1,
    srcs = [
        "test-runtime-lib.ts",
        "test-runtime-lib-inner.d.ts",
        "test-runtime-lib-inner.js",
    ],
    data = [
        ":test-runtime-bin",
    ],
    deps = [
        ":test-runtime-types",
    ],
)

js_test(
    name = "world-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "world-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
    ],
)

js_test(
    name = "memory-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "memory-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:memory",
    ],
)

js_test(
    name = "behavior-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "behavior-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "cursor-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "cursor-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "delete-entity-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "delete-entity-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "disabled-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "disabled-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "independent-observer-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "independent-observer-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "persistent-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "persistent-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/shared:schema",
    ],
)

js_test(
    name = "component-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "component-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "lifecycle-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "lifecycle-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/shared:schema",
    ],
)

js_test(
    name = "system-observers-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "system-observers-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "systems-observers-multi-terms-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "systems-observers-multi-terms-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_library(
    name = "scene-environment",
    srcs = [
        "scene-environment.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:loaders",
        "//c8/ecs/src/runtime:scene-types",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:resource",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "lights-types",
    srcs = [],
    deps = [
        ":three-types",
    ],
)

js_test(
    name = "world-import-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "world-import-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/src/runtime:world",
    ],
)

js_library(
    name = "object-3d-extended",
    srcs = [],
    deps = [
        "three-types",
    ],
)

js_library(
    name = "user-data",
    srcs = [],
    deps = [
        "three-types",
    ],
)

js_test(
    name = "transform-manager-get-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "transform-manager-get-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:transform-manager-types",
        "//c8/ecs/src/runtime:transform-test-helpers",
        "//c8/ecs/src/runtime:world",
    ],
)

js_test(
    name = "transform-manager-set-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "transform-manager-set-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:transform-manager-types",
        "//c8/ecs/src/runtime:transform-test-helpers",
        "//c8/ecs/src/runtime:world",
    ],
)

js_library(
    name = "transform-manager-types",
    srcs = [
        "transform-manager-types.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "transform-manager",
    srcs = [
        "transform-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:transform-manager-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "transform-test-helpers",
    testonly = 1,
    srcs = [
        "transform-test-helpers.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/runtime/math:quat",
        "//c8/ecs/src/shared:angle-conversion",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "raycast-types",
    srcs = [
        "raycast-types.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime/math:vec3",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "raycast",
    srcs = [
        "raycast.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:raycast-types",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math:mat4",
        "//c8/ecs/src/runtime/math:quat",
        "//c8/ecs/src/runtime/math:vec3",
        "//c8/ecs/src/shared:custom-sorting",
        "//c8/ecs/src/shared:eid-operations",
        "//c8/ecs/src/shared:schema",
    ],
)

js_test(
    name = "transform-manager-translate-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "transform-manager-translate-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:transform-manager-types",
        "//c8/ecs/src/runtime:transform-test-helpers",
        "//c8/ecs/src/runtime:world",
    ],
)

js_test(
    name = "transform-manager-temp-variable-test",
    size = "small",
    data = [
        "transform-manager.ts",
    ],
    esnext = 1,
    externalize_npm = 1,
    main = [
        "transform-manager-temp-variable-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
    ],
)

js_test(
    name = "transform-manager-rotate-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "transform-manager-rotate-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:transform-manager-types",
        "//c8/ecs/src/runtime:transform-test-helpers",
        "//c8/ecs/src/runtime:world",
    ],
)

js_test(
    name = "external-three-test",
    size = "small",
    data = [
        "//c8/ecs/src/runtime",
        "//c8/ecs/src/runtime:plugin",
    ],
    main = [
        "external-three-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
    ],
)

js_binary(
    name = "plugin",
    esnext = 1,
    main = [
        "plugin-entry.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/src/runtime:index",
        "//c8/ecs/src/shared:resources",
    ],
)

js_library(
    name = "3d-ui-translate",
    srcs = [
        "3d-ui-translate.ts",
    ],
)

js_library(
    name = "glb-cache-pool",
    srcs = [
        "glb-cache-pool.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/shared:schema",
    ],
)

js_test(
    name = "transform-manager-look-at-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "transform-manager-look-at-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:transform-manager-types",
        "//c8/ecs/src/runtime:transform-test-helpers",
        "//c8/ecs/src/runtime:world",
    ],
)

js_library(
    name = "ui-events",
    srcs = [
        "ui-events.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_test(
    name = "scene-handle-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "scene-handle-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "core-api",
    srcs = [
        "core-api.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-payloads",
        "//c8/ecs/src/runtime:input",
        "//c8/ecs/src/runtime:query",
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:state-machine",
        "//c8/ecs/src/runtime:system",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-create",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "video-texture",
    srcs = [
        "video-texture.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
    ],
)

js_test(
    name = "flecs-prefab-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "flecs-prefab-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:cursor",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "dispose",
    srcs = [
        "dispose.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:video-texture",
    ],
)

js_test(
    name = "entity-impl-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "entity-impl-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
    ],
)

js_library(
    name = "entity-impl",
    srcs = [
        "entity-impl.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:disabled",
        "//c8/ecs/src/runtime:entity-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/runtime/components:hidden-component",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "entity-types",
    srcs = [],
    deps = [
        ":world",
    ],
)

js_library(
    name = "world-attribute",
    srcs = [],
    deps = [
        ":world",
    ],
)

js_library(
    name = "prefab-entity",
    srcs = [
        "prefab-entity.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:asm",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_test(
    name = "scene-prefab-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "scene-prefab-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:transform-test-helpers",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "video-manager",
    srcs = [
        "video-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:audio-manager",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:texture-types",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:video-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:video-constants",
    ],
)

js_test(
    name = "entity-impl-transforms-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "entity-impl-transforms-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:transform-test-helpers",
        "//c8/ecs/src/runtime:world",
    ],
)

js_test(
    name = "world-prefab-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "world-prefab-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "video-types",
    srcs = [
        "video-types.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:texture-types",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
    ],
)

js_test(
    name = "state-machine-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "state-machine-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:state-machine",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
    ],
)

js_library(
    name = "ui-events-raycaster",
    srcs = [
        "ui-events-raycaster.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:pointer-events",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:ui-events",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:custom-sorting",
        "//c8/ecs/src/shared:eid-operations",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:three-layers",
    ],
)

js_test(
    name = "string-storage-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "string-storage-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:string-storage",
    ],
)

js_library(
    name = "media-manager",
    srcs = [
        "media-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:audio-controls-types",
        "//c8/ecs/src/runtime:audio-manager",
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:instanced",
    ],
)

js_library(
    name = "audio-controls-types",
    srcs = [
        "audio-controls-types.ts",
    ],
)

js_test(
    name = "bitflags-test",
    size = "small",
    main = [
        "bitflags-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:bitflags",
    ],
)

js_library(
    name = "bitflags",
    srcs = [
        "bitflags.ts",
    ],
)

js_test(
    name = "component-schema-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "component-schema-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
    ],
)

js_test(
    name = "events-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "events-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_library(
    name = "video",
    srcs = [
        "video.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:video-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "texture-types",
    srcs = [
        "texture-types.ts",
    ],
)

js_library(
    name = "scene-fog",
    srcs = [
        "scene-fog.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:get-fog-from-scene-graph",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_test(
    name = "get-children-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "get-children-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "geometry-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "geometry-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_library(
    name = "texture-constants",
    srcs = [
        "texture-constants.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:texture-types",
    ],
)

js_library(
    name = "effects-manager-types",
    srcs = [
        "effects-manager-types.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "effects-manager",
    srcs = [
        "effects-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:create-scene-background",
        "//c8/ecs/src/runtime:dispose",
        "//c8/ecs/src/runtime:effects-manager-types",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_test(
    name = "effects-manager-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "effects-manager-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_test(
    name = "registry-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "registry-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "component-visibility-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "component-visibility-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "collider-schema-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "collider-schema-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
    ],
)

js_test(
    name = "physics-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "physics-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
    ],
)

js_test(
    name = "events-types-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "events-types-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
    ],
)

js_library(
    name = "event-payloads",
    srcs = [
        "event-payloads.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:camera-manager-types",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:input-events",
        "//c8/ecs/src/runtime:map-types",
        "//c8/ecs/src/runtime:physics-events",
        "//c8/ecs/src/runtime:pointer-events",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:ui-events",
        "//c8/ecs/src/runtime/xr:xr-manager-types",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "map-types",
    srcs = [
        "map-types.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "scene-sky",
    srcs = [
        "scene-sky.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:effects-manager-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:get-sky-from-scene-graph",
        "//c8/ecs/src/shared:resource",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_test(
    name = "scene-sky-test",
    size = "small",
    commonjs = 1,
    esnext = 1,
    externalize_npm = 0,
    main = [
        "scene-sky-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:scene-sky",
    ],
)

js_test(
    name = "component-callbacks-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "component-callbacks-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)
