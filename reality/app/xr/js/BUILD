load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//bzl/crosstool:wasm-pthread.bzl", "emoptsAllowMemoryGrowth")
load("//bzl/httpfileserver:http-file-server.bzl", "http_file_server")
load("//bzl/js:js.bzl", "js_binary", "js_test")
load("//bzl/npmpackage:typecheck.bzl", "npm_typecheck")
load("//bzl/wasm:wasm.bzl", "wasm_binary")
load(":xr-js.bzl", "xr_js_config")
load("@rules_pkg//:pkg.bzl", "pkg_zip")

package(default_visibility = ["//visibility:public"])

npm_typecheck(name = "npm-jsxr")

exports_files([
    "package.json",
    "package-lock.json",
])

filegroup(
    name = "node_modules",
    srcs = glob(["node_modules/**/*"]),
)

ARTIFACTS = [
    ":xr",
    ":xr-face",
    ":xr-tracking",
    "//reality/app/xr/js/resources",
]

http_file_server(
    name = "serve-xr",
    data = ARTIFACTS + select({
        "@the8thwall//bzl/conditions:wasm-pthread": [":xr-js-asm"],
        "//conditions:default": [],
    }),
)

pkg_zip(
    name = "bundle-with-extra-files",
    srcs = ARTIFACTS,
)

# NOTE(christoph) The "excludes" attribute of pkg_zip doesn't support wildcards and doesn't
# support excluding specific generated files, so filter ourselves.
genrule(
    name = "bundle",
    srcs = [":bundle-with-extra-files"],
    outs = ["bundle.zip"],
    cmd = """
cp $(location :bundle-with-extra-files) $@
zip -dq $@ '*.d.ts' '*.map'
""",
)

wasm_binary(
    name = "semantics-worker-wasm",
    cc_binary = ":semantics-worker-cc",
    embind = 1,
    emopts = emoptsAllowMemoryGrowth() + [
        "EXPORTED_RUNTIME_METHODS=[writeArrayToMemory,stringToUTF8]",
        "EXPORT_ES6=1",
        "EXPORT_NAME=SEMANTICSMODULE",
        "FETCH=1",
        "MODULARIZE=1",
        "TOTAL_MEMORY=96mb",  # Must be a multiple of 16MB.
        "USE_ES6_IMPORT_META=0",
    ],
    html = 0,
    single_file = 1,
)

# Jenkins expects the target to be named "xr-js", with an output of "xr.js"
alias(
    name = "xr-js",
    actual = ":xr",
)

js_binary(
    name = "xr",
    srcs = [
        "index.ts",
        ":xr-js-asm",
        ":xr-js-asm.blob-worker",
    ],
    bundle_header = "config/build-header-js",
    main = ["index.ts"],
    npm_rule = "@npm-jsxr//:npm-jsxr",
    polyfill = "none",
    #NOTE(datchu): We can switch to a loader attribute that allows any list of loaders
    string_replacements = ":buildif",
    webpack_analyze = 0,
    deps = [
        "//c8/protolog/api:log-request.capnp-ts",
        "//reality/app/xr/js/src:globals",
        "//reality/app/xr/js/src:jsxr",
        "//reality/engine/api:semantics.capnp-ts",
        "//reality/engine/api/base:camera-environments.capnp-ts",
    ],
)

genrule(
    name = "xr-js-asm.blob-worker",
    srcs = [
        ":xr-js-asm",
    ],
    outs = [
        "xr-js-asm.blob-worker.js",
    ],
    cmd = "$(location //reality/app/xr/js/tools:blob-a-script) $(locations //reality/app/xr/js:xr-js-asm) xr-js-asm.worker.js $(location xr-js-asm.blob-worker.js)",
    tools = ["//reality/app/xr/js/tools:blob-a-script"],
)

xr_js_config(
    name = "build-constants",
    simd = select({
        "@the8thwall//bzl/conditions:simd": True,
        "//conditions:default": False,
    }),
    template = "build-constants.js",
    version = "//bzl/stamp:release-build-id-dynamic",
)

js_binary(
    name = "buildif",
    commonjs = 1,
    export_library = 1,
    main = [
        "buildif.ts",
    ],
)

wasm_binary(
    name = "xr-js-asm",
    cc_binary = ":xr-js-cc",
    emopts = emoptsAllowMemoryGrowth() + [
        "EXPORTED_FUNCTIONS=[_malloc,_free]",
        "EXPORTED_RUNTIME_METHODS=[GL,stringToUTF8,writeArrayToMemory]",
        "EXPORT_ES6=1",
        "EXPORT_NAME=XRCC",
        "FETCH=1",
        "MODULARIZE=1",
        "SPLIT_MEMORY=0",
        "USE_ES6_IMPORT_META=0",
        "WEBGL2_BACKWARDS_COMPATIBILITY_EMULATION=1",
        # The following are useful for debugging but commented out for production:
        #"ASSERTIONS=2",
        #"DISABLE_EXCEPTION_CATCHING=0",
        #"SAFE_HEAP=1", # Add additional memory access checks.
        #"SAFE_HEAP_LOG=1", # Log SAFE_HEAP operations.
        #"STACK_OVERFLOW_CHECK=2", # Detailed stack guard checks.
        #"DEMANGLE_SUPPORT=1", # Automatically demangle stack traces
    ] + select({
        "@the8thwall//bzl/conditions:wasm-pthread": [
            "TOTAL_MEMORY=128mb",  # Must be a multiple of 16MB.
        ],
        "//conditions:default": [
            "TOTAL_MEMORY=96mb",  # Must be a multiple of 16MB.
        ],
    }),
    html = 0,
    single_file = select({
        # NOTE(dat): We can potentially default to not loading a single file
        "@the8thwall//bzl/conditions:wasm-pthread": 0,
        "//conditions:default": 1,
    }),
)

filegroup(
    name = "xr-module-cc-srcs",
    srcs = [
        "facecontroller.cc",
        "layers-controller.cc",
        "tracking-controller.cc",
        "xr-js.cc",
    ],
)

filegroup(
    name = "xr-js-cc-srcs",
    srcs = [
        "layers-controller.cc",
        "xr-js.cc",
    ],
)

cc_binary(
    name = "xr-js-cc",
    srcs = [
        ":xr-js-cc-srcs",
    ],
    deps = [
        ":xrweb",
        "//c8:string",
        "//c8:string-view",
        "//c8:symbol-visibility",
        "//c8/geometry:device-pose",
        "//c8/geometry:homography",
        "//c8/geometry:intrinsics",
        "//c8/pixels:pixel-transforms",
        "//c8/pixels/pipeline:gl-texture-pipeline",
        "//c8/protolog:xr-requests",
        "//c8/string:format",
        "//reality/engine/api:semantics.capnp-cc",
        "//reality/engine/api/base:camera-environments.capnp-cc",
        "//reality/engine/geometry:orientation",
        "//reality/engine/semantics:semantics-classifier",
        "//reality/engine/semantics:semantics-cubemap-renderer",
    ],
)

js_binary(
    name = "xr-face",
    bundle_header = "config/build-header-js",
    export_library = 1,
    main = ["face-index.ts"],
    npm_rule = "@npm-jsxr//:npm-jsxr",
    polyfill = "none",
    #NOTE(datchu): We can switch to a loader attribute that allows any list of loaders
    string_replacements = ":buildif",
    webpack_analyze = 0,
    deps = [
        "//reality/app/xr/js/src:face-controller",
        "//reality/app/xr/js/src:globals",
    ],
)

wasm_binary(
    name = "xr-face-wasm",
    cc_binary = ":xr-face-cc",
    emopts = emoptsAllowMemoryGrowth() + [
        "EXPORT_ES6=1",
        "EXPORT_NAME=XRFACECC",
        "EXPORTED_FUNCTIONS=[_malloc,_free]",
        "EXPORTED_RUNTIME_METHODS=[GL,stringToUTF8,writeArrayToMemory]",
        "MODULARIZE=1",
        "SPLIT_MEMORY=0",
        "USE_ES6_IMPORT_META=0",
        "WEBGL2_BACKWARDS_COMPATIBILITY_EMULATION=1",
        # The following are useful for debugging but commented out for production:
        #"ASSERTIONS=2",
        #"DISABLE_EXCEPTION_CATCHING=0",
        #"SAFE_HEAP=1", # Add additional memory access checks.
        #"SAFE_HEAP_LOG=1", # Log SAFE_HEAP operations.
        #"STACK_OVERFLOW_CHECK=2", # Detailed stack guard checks.
        #"DEMANGLE_SUPPORT=1", # Automatically demangle stack traces
    ] + select({
        "@the8thwall//bzl/conditions:wasm-pthread": [
            "TOTAL_MEMORY=16mb",  # Must be a multiple of 16MB.
        ],
        "//conditions:default": [
            "TOTAL_MEMORY=16mb",  # Must be a multiple of 16MB.
        ],
    }),
    html = 0,
    single_file = select({
        # NOTE(dat): We can potentially default to not loading a single file
        "@the8thwall//bzl/conditions:wasm-pthread": 0,
        "//conditions:default": 1,
    }),
)

filegroup(
    name = "xrcc-face-srcs",
    srcs = [
        "facecontroller.cc",
    ],
)

cc_binary(
    name = "xr-face-cc",
    srcs = [
        ":xrcc-face-srcs",
    ],
    deps = [
        "//c8:symbol-visibility",
        "//c8/geometry:facemesh-data",
        "//c8/geometry:intrinsics",
        "//reality/engine/api:face.capnp-cc",
        "//reality/engine/api/base:camera-environments.capnp-cc",
        "//reality/engine/ears:ear-roi-renderer",
        "//reality/engine/faces:face-geometry",
        "//reality/engine/faces:face-messages",
        "//reality/engine/faces:face-roi-renderer",
        "//reality/engine/faces:face-roi-shader",
        "//reality/engine/faces:face-tracker",
        "//reality/engine/geometry:orientation",
    ],
)

js_binary(
    name = "xr-tracking",
    bundle_header = "config/build-header-js",
    export_library = 1,
    main = ["tracking-index.ts"],
    npm_rule = "@npm-jsxr//:npm-jsxr",
    polyfill = "none",
    string_replacements = ":buildif",
    webpack_analyze = 0,
    deps = [
        "//reality/app/xr/js/src:globals",
        "//reality/app/xr/js/src:resources",
        "//reality/app/xr/js/src:tracking-controller",
    ],
)

wasm_binary(
    name = "xr-tracking-wasm",
    cc_binary = ":xr-tracking-cc",
    emopts = emoptsAllowMemoryGrowth() + [
        "EXPORT_ES6=1",
        "EXPORT_NAME=XRTRACKINGCC",
        "EXPORTED_FUNCTIONS=[_malloc,_free]",
        "EXPORTED_RUNTIME_METHODS=[GL,writeArrayToMemory,lengthBytesUTF8,stringToUTF8]",
        "FETCH=1",
        "MODULARIZE=1",
        "SPLIT_MEMORY=0",
        "USE_ES6_IMPORT_META=0",
        "WEBGL2_BACKWARDS_COMPATIBILITY_EMULATION=1",
        # The following are useful for debugging but commented out for production:
        #"ASSERTIONS=2",
        #"DISABLE_EXCEPTION_CATCHING=0",
        #"SAFE_HEAP=1", # Add additional memory access checks.
        #"SAFE_HEAP_LOG=1", # Log SAFE_HEAP operations.
        #"STACK_OVERFLOW_CHECK=2", # Detailed stack guard checks.
        #"DEMANGLE_SUPPORT=1", # Automatically demangle stack traces
    ] + select({
        "@the8thwall//bzl/conditions:wasm-pthread": [
            "TOTAL_MEMORY=16mb",  # Must be a multiple of 16MB.
        ],
        "//conditions:default": [
            "TOTAL_MEMORY=16mb",  # Must be a multiple of 16MB.
        ],
    }),
    html = 0,
    single_file = select({
        # NOTE(dat): We can potentially default to not loading a single file
        "@the8thwall//bzl/conditions:wasm-pthread": 0,
        "//conditions:default": 1,
    }),
)

filegroup(
    name = "xrcc-tracking-srcs",
    srcs = [
        "tracking-controller.cc",
    ],
)

cc_binary(
    name = "xr-tracking-cc",
    srcs = [
        ":xrcc-tracking-srcs",
    ],
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:wasm": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//c8:symbol-visibility",
        "//c8/geometry:intrinsics",
        "//c8/io:image-io",
        "//c8/pixels:pixel-buffer",
        "//c8/protolog/api:remote-request.capnp-cc",
        "//reality/engine/executor:xr-engine",
        "//reality/engine/features:gl-reality-frame",
        "//reality/engine/imagedetection:detection-image",
        "//reality/engine/imagedetection:detection-image-loader",
    ],
)

# START_AUTOGENERATED_RULES
# Do not edit these rules manually, use inliner.
# file: semantics-worker.cc
cc_binary(
    name = "semantics-worker-cc",
    srcs = [
        "semantics-worker.cc",
    ],
    deps = [
        "//bzl/inliner:rules2",
        "//c8:c8-log",
        "//c8:string",
        "//c8:symbol-visibility",
        "//c8:vector",
        "//c8/protolog:xr-requests",
        "//c8/string:format",
        "//reality/engine/executor:xr-engine",
        "//reality/engine/semantics:semantics-classifier",
        "@json",
    ],
)
# cc_end(0x0a11c6c2)

# file: xrweb.cc
cc_library(
    name = "xrweb",
    srcs = [
        "xrweb.cc",
    ],
    hdrs = [
        "xrweb.h",
    ],
    deps = [
        "//bzl/inliner:rules2",
        "//c8:string",
    ],
)
# cc_end(0x08637cca)
# END_AUTOGENERATED_RULES
