load("@bazel_skylib//rules:common_settings.bzl", "bool_flag", "string_flag")
load(":node-tools.bzl", "node_tool_wrapper", "node_wrapper")

package(
    default_visibility = ["//visibility:public"],
)

# Whether to use the source-built node for the target headers or whether to use
# headers from the node source package.
bool_flag(
    name = "source-built",
    build_setting_default = False,
    visibility = ["//visibility:public"],
)

config_setting(
    name = "source-built-false",
    flag_values = {
        ":source-built": "False",
    },
)

config_setting(
    name = "source-built-true",
    flag_values = {
        ":source-built": "True",
    },
)

# Build node wrapper script to run node through lldb for debugging.
string_flag(
    name = "lldb",
    build_setting_default = "none",
    values = [
        "none",  # Don't use lldb
        "auto",  # Use lldb in a non-interactive mode, suitable for bazel run or bazel test.
        "interactive",  # Interactive lldb. Can only run by running a binary directly.
    ],
    visibility = ["//visibility:public"],
)

node_wrapper(
    name = "node",
    node = "@nodejs_host//:node_bin",
)

node_tool_wrapper(
    name = "npm",
    node = ":node",
    node_tool = "@nodejs_host//:npm_bin",
)

node_tool_wrapper(
    name = "npx",
    node = ":node",
    node_tool = "@nodejs_host//:npx_bin",
)

genrule(
    name = "node-addons-hdrs",
    srcs = [
        "@nodejs_host//:npm_files",
        "@nodejs_host//:node_bin",
    ],
    outs = [
        "node_api.h",
        "node_api_types.h",
        "js_native_api.h",
        "js_native_api_types.h",
    ],
    cmd = "cp $$(dirname $$(dirname $(location @nodejs_host//:node_bin)))/include/node/*api*.h $(@D)/",
)

cc_library(
    name = "node-addons-lib",
    hdrs = [":node-addons-hdrs"],
    defines = [
        "NAPI_EXTERN=__attribute__((weak))",
    ],
    includes = ["."],
    visibility = ["//visibility:private"],
)

alias(
    name = "node-addons",
    actual = select({
        ":source-built-true": "@node//:node-addons",
        ":source-built-false": "node-addons-lib",
    }),
)

genrule(
    name = "node-addons-extra-hdrs",
    srcs = [
        "@nodejs_host//:npm_files",
        "@nodejs_host//:node_bin",
    ],
    outs = [
        "node.h",
        "node_buffer.h",
        "node_object_wrap.h",
        "node_version.h",
        "cppgc/common.h",
        "v8-array-buffer.h",
        "v8-callbacks.h",
        "v8-container.h",
        "v8-context.h",
        "v8-data.h",
        "v8-date.h",
        "v8-debug.h",
        "v8-embedder-heap.h",
        "v8-embedder-state-scope.h",
        "v8-exception.h",
        "v8-extension.h",
        "v8-external.h",
        "v8-function-callback.h",
        "v8-function.h",
        "v8-initialization.h",
        "v8-internal.h",
        "v8-isolate.h",
        "v8-json.h",
        "v8-local-handle.h",
        "v8-locker.h",
        "v8-maybe.h",
        "v8-memory-span.h",
        "v8-message.h",
        "v8-microtask-queue.h",
        "v8-microtask.h",
        "v8-object.h",
        "v8-persistent-handle.h",
        "v8-platform.h",
        "v8-primitive-object.h",
        "v8-primitive.h",
        "v8-promise.h",
        "v8-proxy.h",
        "v8-regexp.h",
        "v8-script.h",
        "v8-snapshot.h",
        "v8-statistics.h",
        "v8-template.h",
        "v8-traced-handle.h",
        "v8-typed-array.h",
        "v8-unwinder.h",
        "v8-value-serializer.h",
        "v8-value.h",
        "v8-version.h",
        "v8-wasm.h",
        "v8-weak-callback-info.h",
        "v8.h",
        "v8config.h",
        "uv.h",
        "uv/aix.h",
        "uv/bsd.h",
        "uv/darwin.h",
        "uv/errno.h",
        "uv/linux.h",
        "uv/os390.h",
        "uv/posix.h",
        "uv/stdint-msvc2008.h",
        "uv/sunos.h",
        "uv/threadpool.h",
        "uv/tree.h",
        "uv/unix.h",
        "uv/version.h",
        "uv/win.h",
    ],
    cmd = "rsync -Lr $$(dirname $$(dirname $(location @nodejs_host//:node_bin)))/include/node/* $(@D)/",
)

cc_library(
    name = "node-addons-extra-lib",
    hdrs = [":node-addons-extra-hdrs"],
    visibility = ["//visibility:private"],
    deps = [
        ":node-addons",
    ],
)

alias(
    name = "node-addons-extra",
    actual = select({
        ":source-built-true": "@node//:node-addons-extra",
        ":source-built-false": "node-addons-extra-lib",
    }),
)
