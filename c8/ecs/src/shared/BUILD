load("//bzl/js:js.bzl", "js_library", "js_test")

package(default_visibility = [
    "//c8/ecs:__subpackages__",
    "//reality/cloud:shared",
])

js_library(
    name = "parse-component-ast",
    srcs = [
        "parse-component-ast.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:asset-constants",
        "//c8/ecs/src/shared:component-constants",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:studio-component",
    ],
)

js_library(
    name = "validate-space-names",
    srcs = [
        "validate-space-names.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "audio-constants",
    srcs = [
        "audio-constants.ts",
    ],
)

js_library(
    name = "component-constants",
    srcs = [
        "component-constants.ts",
    ],
)

js_library(
    name = "crdt",
    srcs = [
        "crdt.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:automerge",
        "//c8/ecs/src/shared:data",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "data",
    srcs = [
        "data.ts",
    ],
)

js_library(
    name = "debug-messaging",
    srcs = [
        "debug-messaging.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:transform-map",
    ],
)

js_library(
    name = "geometry-constants",
    srcs = [
        "geometry-constants.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "instanced",
    srcs = [
        "instanced.ts",
    ],
)

js_library(
    name = "light-constants",
    srcs = [
        "light-constants.ts",
    ],
)

js_library(
    name = "physic-constants",
    srcs = [
        "physic-constants.ts",
    ],
)

js_library(
    name = "scene-graph",
    srcs = [
        "scene-graph.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:flex-style-types",
        "//c8/ecs/src/shared:runtime-version",
    ],
)

js_library(
    name = "schema-utility",
    srcs = [
        "schema-utility.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:component-constants",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "schema",
    srcs = [
        "schema.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:asset-constants",
        "//c8/ecs/src/shared:component-constants",
    ],
)

js_library(
    name = "studio-component",
    srcs = [
        "studio-component.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "flex-style-types",
    srcs = [
        "flex-style-types.ts",
    ],
)

js_library(
    name = "asset-constants",
    srcs = [
        "asset-constants.ts",
    ],
)

js_library(
    name = "transform-map",
    srcs = [
        "transform-map.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "msdf-font-type",
    srcs = [
        "msdf-font-type.ts",
    ],
    visibility = [
        "//c8/ecs:__subpackages__",
        "//reality/app/nae/packager:__subpackages__",
        "//reality/cloud:shared",
    ],
)

js_library(
    name = "map-constants",
    srcs = [
        "map-constants.ts",
    ],
    visibility = [
        "//c8/ecs:__subpackages__",
        "//reality/app/nae/packager:__subpackages__",
        "//reality/cloud:shared",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "application-type",
    srcs = [
        "application-type.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:scene-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "color",
    srcs = [
        "color.ts",
    ],
)

js_library(
    name = "default-action-maps",
    srcs = [
        "default-action-maps.ts",
    ],
)

js_library(
    name = "face-mesh-data",
    srcs = [
        "face-mesh-data.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "find-root-component",
    srcs = [
        "find-root-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "flex-enum",
    srcs = [
        "flex-enum.ts",
    ],
)

js_library(
    name = "flex-styles",
    srcs = [
        "flex-styles.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:ui-types",
        "//c8/ecs/src/shared:flex-enum",
    ],
)

js_library(
    name = "fonts",
    srcs = [
        "fonts.ts",
    ],
    visibility = [
        "//c8/ecs:__subpackages__",
        "//reality/app/nae/packager:__subpackages__",
        "//reality/cloud:shared",
    ],
    deps = [
        "//c8/ecs/src/shared:resources",
    ],
)

js_library(
    name = "get-sky-from-scene-graph",
    srcs = [
        "get-sky-from-scene-graph.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:sky-constants",
    ],
)

js_library(
    name = "get-ui-attributes",
    srcs = [
        "get-ui-attributes.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:ui",
        "//c8/ecs/src/shared:resource",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:ui-constants",
    ],
)

js_library(
    name = "gltf-constants",
    srcs = [
        "gltf-constants.ts",
    ],
)

js_library(
    name = "load-script",
    srcs = [
        "load-script.ts",
    ],
)

js_library(
    name = "material-constants",
    srcs = [
        "material-constants.ts",
    ],
)

js_library(
    name = "mesh-optimizer-constants",
    srcs = [
        "mesh-optimizer-constants.ts",
    ],
)

js_library(
    name = "object-hierarchy",
    srcs = [
        "object-hierarchy.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "resource",
    srcs = [
        "resource.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:fonts",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "sky-constants",
    srcs = [
        "sky-constants.ts",
    ],
)

js_library(
    name = "splat-model",
    srcs = [
        "splat-model.ts",
    ],
)

js_library(
    name = "sync-session",
    srcs = [
        "sync-session.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:index",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:application-type",
        "//c8/ecs/src/shared:data",
        "//c8/ecs/src/shared:debug-messaging",
        "//c8/ecs/src/shared:object-hierarchy",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:transform-map",
        "//c8/ecs/src/shared:world-to-scene-graph",
    ],
)

js_library(
    name = "ui-constants",
    srcs = [
        "ui-constants.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:fonts",
    ],
)

js_library(
    name = "validate-number",
    srcs = [
        "validate-number.ts",
    ],
)

js_library(
    name = "world8",
    srcs = [
        "world8.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:scene-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "xr-config",
    srcs = [
        "xr-config.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "get-camera-from-scene-graph",
    srcs = [
        "get-camera-from-scene-graph.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:object-hierarchy",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "map-controller",
    srcs = [
        "map-controller.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:map-constants",
        "//c8/ecs/src/shared:material-constants",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "ui-nine-slice",
    srcs = [
        "ui-nine-slice.ts",
    ],
)

js_library(
    name = "world-to-scene-graph",
    srcs = [
        "world-to-scene-graph.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:index",
        "//c8/ecs/src/runtime:scene-types",
        "//c8/ecs/src/runtime:ui-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:application-type",
        "//c8/ecs/src/shared:object-hierarchy",
        "//c8/ecs/src/shared:resource",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:ui-constants",
    ],
)

js_library(
    name = "asset-manifest",
    srcs = [
        "asset-manifest.ts",
    ],
    visibility = ["//visibility:public"],
)

js_library(
    name = "find-component",
    srcs = [
        "find-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "session-stats",
    srcs = [
        "session-stats.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/shared:application-type",
    ],
)

js_library(
    name = "angle-conversion",
    srcs = [
        "angle-conversion.ts",
    ],
)

js_test(
    name = "parse-component-ast-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "parse-component-ast-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:parse-component-ast",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:studio-component",
    ],
)

js_test(
    name = "schema-utility-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "schema-utility-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:component-constants",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:schema-utility",
    ],
)

js_test(
    name = "crdt-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "crdt-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:automerge",
        "//c8/ecs/src/shared:crdt",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_test(
    name = "crdt-undo-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "crdt-undo-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:crdt",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

genrule(
    name = "automerge-gen",
    outs = [
        "automerge.js",
    ],
    cmd = "echo \"module.exports = require('@automerge/automerge/next')\" > $@",
)

js_library(
    name = "automerge",
    srcs = [
        ":automerge-gen",
    ],
)

js_library(
    name = "get-environment-from-scene-graph",
    srcs = [
        "get-environment-from-scene-graph.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:resource",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "transform-constants",
    srcs = [
        "transform-constants.ts",
    ],
)

js_library(
    name = "environment-maps",
    srcs = [
        "environment-maps.ts",
    ],
    visibility = [
        "//c8/ecs:__subpackages__",
        "//reality/app/nae/packager:__subpackages__",
        "//reality/cloud:shared",
    ],
)

js_library(
    name = "eid-operations",
    srcs = [
        "eid-operations.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "base-sync-session",
    srcs = [
        "base-sync-session.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:application-type",
        "//c8/ecs/src/shared:crdt",
        "//c8/ecs/src/shared:data",
        "//c8/ecs/src/shared:debug-messaging",
    ],
)

js_test(
    name = "object-hierarchy-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "object-hierarchy-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:object-hierarchy",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "camera-constants",
    srcs = [
        "camera-constants.ts",
    ],
)

js_library(
    name = "video-constants",
    srcs = [
        "video-constants.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:audio-constants",
    ],
)

js_test(
    name = "crdt-prefab-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "crdt-prefab-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:crdt",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "validate-prefab-names",
    srcs = [
        "validate-prefab-names.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:object-hierarchy",
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_test(
    name = "three-layers-test",
    size = "small",
    externalize_npm = 1,
    main = [
        "three-layers-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:three-layers",
    ],
)

js_library(
    name = "three-layers",
    srcs = [
        "three-layers.ts",
    ],
)

js_library(
    name = "extended-schema",
    srcs = [
        "extended-schema.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "fog-constants",
    srcs = [
        "fog-constants.ts",
    ],
)

js_library(
    name = "get-fog-from-scene-graph",
    srcs = [
        "get-fog-from-scene-graph.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:scene-graph",
    ],
)

js_library(
    name = "custom-sorting",
    srcs = [
        "custom-sorting.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:three-types",
    ],
)

js_test(
    name = "determine-ui-order-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "determine-ui-order-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared:determine-ui-order",
        "//c8/ecs/src/shared:flex-styles",
    ],
)

js_library(
    name = "determine-ui-order",
    srcs = [
        "determine-ui-order.ts",
    ],
    deps = [
        "//c8/ecs/src/shared:flex-styles",
    ],
)

js_library(
    name = "release-notes-types",
    srcs = [
        "release-notes-types.ts",
    ],
)

js_library(
    name = "runtime-version",
    srcs = [
        "runtime-version.ts",
    ],
)

js_library(
    name = "tooltip-types",
    srcs = [
        "tooltip-types.ts",
    ],
)

js_test(
    name = "tooltip-completion-test",
    size = "small",
    data = [
        "//c8/ecs/gen:runtime-tooltip-data.json",
    ],
    esnext = 1,
    externalize_npm = 1,
    main = [
        "tooltip-completion-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    tags = [
        "manual",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:tooltip-types",
    ],
)

js_test(
    name = "tooltip-defaults-test",
    size = "small",
    data = [
        "//c8/ecs/gen:runtime-tooltip-data.json",
    ],
    esnext = 1,
    externalize_npm = 1,
    main = [
        "tooltip-defaults-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:tooltip-types",
    ],
)

js_test(
    name = "tooltip-types-test",
    size = "small",
    data = [
        "//c8/ecs/gen:runtime-tooltip-data.json",
    ],
    esnext = 1,
    externalize_npm = 1,
    main = [
        "tooltip-types-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:tooltip-types",
    ],
)

js_library(
    name = "resources",
    srcs = [
        "resources.ts",
    ],
)
