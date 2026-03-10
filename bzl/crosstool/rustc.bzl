"""
Defines a wrapper around RUSTC which sets SDKROOT.
"""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

def _rustc_impl(ctx):
    out = ctx.actions.declare_file(ctx.attr.name)
    runfiles = ctx.runfiles(files = [out, ctx.file.rustc, ctx.file._workspace_env])
    runfiles = runfiles.merge(ctx.attr._workspace_env[DefaultInfo].default_runfiles)
    runfiles = runfiles.merge(ctx.attr.rustc[DefaultInfo].default_runfiles)

    ctx.actions.expand_template(
        template = ctx.file.src,
        output = out,
        substitutions = {
            "{{RUSTC}}": ctx.file.rustc.path,
            "{{WORKSPACE_ENV}}": ctx.file._workspace_env.path,
            "{{platform}}": ctx.attr.platform,
            "{{xcode_version}}": ctx.attr._xcode[BuildSettingInfo].value,
        },
    )

    return [
        DefaultInfo(
            files = depset([out]),
            runfiles = runfiles,
            executable = out,
        ),
    ]

rustc = rule(
    implementation = _rustc_impl,
    executable = True,
    attrs = {
        "rustc": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "src": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
        ),
        "_xcode": attr.label(
            default = Label("//bzl/xcode:version"),
            providers = [BuildSettingInfo],
        ),
        "platform": attr.string(
            mandatory = True,
        ),
    },
)
