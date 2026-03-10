def _local_tool_impl(repository_ctx):
    """Implementation for the local_tool rule."""
    tool = repository_ctx.which(repository_ctx.attr.tool)

    BUILD_contents = [
        'package(default_visibility = ["//visibility:public"])',
        "",
        'constraint_setting(name = "installed-setting")',
        "",
        "constraint_value(",
        '    name = "installed",',
        '    constraint_setting = ":installed-setting",',
        ")",
        "",
        "constraint_value(",
        '    name = "missing",',
        '    constraint_setting = ":installed-setting",',
        ")",
        "",
        "filegroup(",
        '    name = "{}",'.format(repository_ctx.attr.tool),
        '    srcs = [ "{}.sh" ],'.format(repository_ctx.attr.tool),
        ")",
        "",
    ]

    repository_ctx.file(
        "BUILD",
        content = "\n".join(BUILD_contents),
        executable = False,
    )

    if tool:
        sh_contents = [
            "#!/bin/bash --norc",
            "",
            "set -eu",
            "",
            "export RUNFILES_DIR=${RUNFILES_DIR:-$0.runfiles}",
            'TOOL="{}"'.format(tool),
            "",
            '$TOOL "$@"',
            "",
        ]
    else:
        sh_contents = [
            "#!/bin/bash --norc",
            "",
            "set -eu",
            "",
            'echo "{} not found. (if you install it, run bazel clean)"'.format(repository_ctx.attr.tool),
            "",
            "exit 1",
            "",
        ]

    repository_ctx.file(
        "{}.sh".format(repository_ctx.attr.tool),
        content = "\n".join(sh_contents),
        executable = True,
    )

# Creates wrapper script to a tool installed locally and available in the
# default shell path. This should ideally be used only for runfiles, as using
# for compile-time tools will affect remote caching.
local_tool = repository_rule(
    implementation = _local_tool_impl,
    attrs = {
        "tool": attr.string(mandatory = True),
    },
    local = True,
)
