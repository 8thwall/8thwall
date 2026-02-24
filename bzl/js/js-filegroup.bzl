def _js_target_transition_impl(settings, attr):
    if attr.force_target not in ["web", "node"]:
        return None
    return {"//bzl/js:target": attr.force_target}

js_target_transition = transition(
    implementation = _js_target_transition_impl,
    inputs = [],
    outputs = ["//bzl/js:target"],
)

def _js_filegroup_impl(ctx):
    defaultInfos = [src[DefaultInfo] for src in ctx.attr.srcs if DefaultInfo in src]

    files = depset(transitive = [info.files for info in defaultInfos])
    runfiles = ctx.runfiles().merge_all([info.default_runfiles for info in defaultInfos])

    return [
        DefaultInfo(files = files, runfiles = runfiles),
    ]

# Filegroup rule with option to force a node or web target transition.
js_filegroup = rule(
    implementation = _js_filegroup_impl,
    cfg = js_target_transition,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        # If 'web' or 'node', force a transition to the specified configuration
        # this rule and its transitive sources.
        "force_target": attr.string(default = ""),
        "srcs": attr.label_list(allow_files = True, default = []),
    },
)
