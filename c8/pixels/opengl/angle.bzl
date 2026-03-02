load("//bzl/js:js.bzl", "NpmPackageProvider", "js_files_provider")

def _force_angle_impl(settings, attr):
    return {"//c8/pixels/opengl:angle": True}

force_angle = transition(
    implementation = _force_angle_impl,
    inputs = [],
    outputs = ["//c8/pixels/opengl:angle"],
)

def _angle_transition_impl(ctx):
    target = ctx.attr.actual[0]
    providers = [target[DefaultInfo]]
    if OutputGroupInfo in target:
        providers.append(target[OutputGroupInfo])
    if CcInfo in target:
        providers.append(target[CcInfo])
    if NpmPackageProvider in target:
        providers.append(target[NpmPackageProvider])
    if js_files_provider in target:
        providers.append(target[js_files_provider])

    return providers

# Force the --//c8/pixels/opengl:angle AKA --config=angle configuration.
angle_transition = rule(
    implementation = _angle_transition_impl,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "actual": attr.label(cfg = force_angle),
    },
)
