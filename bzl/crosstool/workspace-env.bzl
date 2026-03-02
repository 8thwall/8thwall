def _workspace_env_impl(repository_ctx):
    """Implementation for the workspace_env rule."""
    pwd_result = repository_ctx.execute(["pwd"])
    execroot = repository_ctx.path(pwd_result.stdout.strip()).dirname
    workspace = repository_ctx.path(repository_ctx.attr._workspace).dirname
    output_base = repository_ctx.path(execroot).dirname
    execution_root = "{}/execroot/_main".format(output_base)

    repository_ctx.file(
        "workspace-env.sh",
        content = (
            """#!/bin/sh --norc
function get_realpath() {
    local previous="$1"
    local next=$(readlink "${previous}")
    while [ -n "${next}" ]; do
        previous="${next}"
        next=$(readlink "${previous}")
    done
    echo "${previous}"
}

shellpath=$(get_realpath $BASH_SOURCE)
source "$(dirname $shellpath)/workspace-env-vars"
"""
        ),
        executable = True,
    )

    repository_ctx.file(
        "workspace-env-vars",
        content = (
            """
export BAZEL_EXECUTION_ROOT="{execroot}"
export BAZEL_OUTPUT_BASE="{base}"
export BAZEL_WORKSPACE="{workspace}"
""".format(execroot = execution_root, base = output_base, workspace = workspace)
        ),
    )

    repository_ctx.file(
        "workspace-env.json",
        content = (
            "{\n" +
            "   \"BAZEL_EXECUTION_ROOT\": \"{execroot}\",\n".format(execroot = execution_root) +
            "   \"BAZEL_OUTPUT_BASE\": \"{base}\",\n".format(base = output_base) +
            "   \"BAZEL_WORKSPACE\": \"{workspace}\"\n".format(workspace = workspace) +
            "}\n"
        ),
    )

    repository_ctx.file(
        "BUILD",
        content = (
            """
filegroup(
    name = "workspace-env-json",
    srcs = [
        "workspace-env.json",
    ],
    visibility = [ "//visibility:public" ],
)

filegroup(
    name = "workspace-env",
    srcs = [
        "workspace-env.sh",
    ],
    visibility = [ "//visibility:public" ],
)

"""
        ),
    )

# Some rules need to access to varying global environment variables without
# bazel knowing, since these would affect Bazel's cache key but are logically
# equal. This rule provides a target that can bring these into shell scripts.
workspace_env = repository_rule(
    implementation = _workspace_env_impl,
    local = True,
    attrs = {
        "workspace_name": attr.string(mandatory = True),
        "_workspace": attr.label(default = "@//:WORKSPACE", allow_single_file = True),
    },
)
