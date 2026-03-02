load("//bzl/node:node-source-transition.bzl", "node_source_transition")
load("//bzl/runfiles:materialize-node-runfiles.bzl", "materialize_node_runfiles")
load("@build_bazel_rules_android//android:rules.bzl", "android_binary")
load("//bzl/android:android.bzl", "android_deploy_aar")

def _android_node_cc_deps_impl(ctx):
    native_deps = depset(transitive = [deps[DefaultInfo].files for deps in ctx.attr.deps])

    cc_infos = [deps[CcInfo] for deps in ctx.attr.deps if CcInfo in deps]
    java_infos = [deps[JavaInfo] for deps in ctx.attr.deps if JavaInfo in deps]

    if java_infos:
        fail("Use java_deps for android and java deps")

    providers = [
        DefaultInfo(
            files = native_deps,
        ),
    ]

    if cc_infos:
        providers.append(cc_common.merge_cc_infos(direct_cc_infos = cc_infos))

    return providers

_android_node_cc_deps = rule(
    implementation = _android_node_cc_deps_impl,
    cfg = node_source_transition,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "deps": attr.label_list(
            default = [],
        ),
    },
)

# Similar to android_binary, but builds all 'assets' with source-built node JS
# transition and adds a 'wasm_assets' attribute which builds deps with a
# webassembly transition. Use java_deps for android and java deps.
## TODO(alvinp): Decide whether android_node_binary can be removed in favor of android_node_aar.
def android_node_binary(
        name,
        deps = [],
        java_deps = [],
        assets = [],
        wasm_assets = [],
        assets_dir = "",
        **kwargs):
    assets_name = "{}-assets-impl".format(name)
    deps_name = "{}-cc-deps-impl".format(name)

    materialize_node_runfiles(
        name = assets_name,
        deps = assets,
        wasm_deps = wasm_assets,
    )

    _android_node_cc_deps(
        name = deps_name,
        deps = deps,
    )

    android_binary(
        name = name,
        assets_dir = assets_dir,
        assets = [
            ":{}".format(assets_name),
        ],
        deps = java_deps + [
            ":{}".format(deps_name),
        ],
        **kwargs
    )

def android_node_aar(
        name,
        deps = [],
        java_deps = [],
        assets = [],
        wasm_assets = [],
        assets_dir = "",
        **kwargs):
    assets_name = "{}-assets-impl".format(name)
    deps_name = "{}-cc-deps-impl".format(name)

    materialize_node_runfiles(
        name = assets_name,
        deps = assets,
        wasm_deps = wasm_assets,
    )

    _android_node_cc_deps(
        name = deps_name,
        deps = deps,
    )

    android_deploy_aar(
        name = name,
        assets_dir = assets_dir,
        assets = [
            ":{}".format(assets_name),
        ],
        deps = java_deps + [
            ":{}".format(deps_name),
        ],
        **kwargs
    )
