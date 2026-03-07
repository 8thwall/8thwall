load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

def _g8_config_impl(ctx):
    out = ctx.actions.declare_file("{}.h".format(ctx.attr.name))

    ctx.actions.write(
        output = out,
        content = "\n".join([
            "#pragma once",
            "",
            "constexpr char G8_VERSION[] = \"{}\";".format(ctx.attr.version[BuildSettingInfo].value),
            "",
        ]),
    )

    return [
        DefaultInfo(files = depset([out])),
        CcInfo(
            compilation_context = cc_common.create_compilation_context(
                headers = depset([out]),
            ),
        ),
    ]

g8_config = rule(
    implementation = _g8_config_impl,
    attrs = {
        "version": attr.label(
            mandatory = True,
            providers = [BuildSettingInfo],
        ),
    },
)
