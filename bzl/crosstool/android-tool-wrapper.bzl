load("//bzl/android:android-version.bzl", "ANDROID_NDK_VERSION")

NDK_RELATIVE = "external/androidsdk/ndk/{version}".format(version = ANDROID_NDK_VERSION)

def _android_tool_wrapper_impl(ctx):
    llvm_prebuilt_relative = "{ndk}/toolchains/llvm/prebuilt/{exec_prebuilt_dir}".format(ndk = NDK_RELATIVE, exec_prebuilt_dir = ctx.attr.prebuilt_dir)

    out = ctx.actions.declare_file(ctx.label.name + ".sh")
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {
            "{{WORKSPACE_ENV}}": ctx.file._workspace_env.path,
            "{{LLVM_PREBUILT}}": llvm_prebuilt_relative,
        },
        is_executable = True,
    )
    return [DefaultInfo(executable = out, files = depset([out]), runfiles = ctx.runfiles([out]))]

android_tool_wrapper = rule(
    implementation = _android_tool_wrapper_impl,
    executable = True,
    attrs = {
        "_workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "template": attr.label(
            allow_single_file = [".sh.tpl"],
            mandatory = True,
        ),
        "prebuilt_dir": attr.string(
            mandatory = True,
        ),
    },
)
