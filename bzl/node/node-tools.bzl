load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

def _node_wrapper_impl(ctx):
    output = ctx.actions.declare_file(ctx.label.name)
    runfiles = ctx.runfiles(files = ctx.files.node + [output])
    node_path = "{workspace}/{path}".format(workspace = ctx.workspace_name, path = ctx.file.node.short_path)

    # See https://github.com/bazelbuild/bazel/issues/14444 for discussion on detecting tool configuration.
    is_exec = "-exec" in ctx.bin_dir.path or "host" in ctx.bin_dir.path
    lldb_flavor = ctx.attr._lldb[BuildSettingInfo].value or "none"
    if is_exec:
        # Don't use lldb for host tools.
        lldb_flavor = "none"

    # For use when not running within bazel run or bazel test. Here we go into
    # standard interactive lldb, and the caller would type 'r' to start the
    # program.
    # NOTE(mc): An improved version of this would get lldb from the hermetic toolchain for each crosstool host.
    lldb_interactive = "lldb $NODE -- \"$@\""

    # NOTE: Due to a bug in current mac lldb version, SIGINT is ignored if there isn't a keypress in the CLI.
    # See: https://github.com/llvm/llvm-project/issues/53673
    # and potential fix:
    # https://github.com/llvm/llvm-project/commit/14bd14f9f92f9f5eff43ec3a977b127dea1cb521#diff-6af975ad6be04a4c0b06e06ee09df4e3da853364364b061596760c21752a3de5
    # As a result, we send a SIGTERM instead.
    #
    # So with the SIGINT->SIGTERM fix, the following says, run lldb in batch
    # mode, quietly, as a background process, then wait for it to complete. If
    # a crash happens, print out a backtrace and then exit with status 1. If
    # the program terminates on its own accord, get the exit code and then
    # return it. Create a trap for ^C (SIGINT), and if received send it to the
    # lldb process as a SIGTERM to workaround the issue. Ideally it would
    # forward a SIGINT.
    lldb_non_interactive = """
    # Trap Ctrl-C and pass it to lldb
    trap 'echo "Ctrl-C detected, terminating LLDB [pid=$LLDB_PID]..."; kill -s SIGTERM $LLDB_PID' SIGINT
    lldb --batch -Q -o r -k 'bt all' -k 'quit 1' -o 'script import os; os._exit(lldb.debugger.GetSelectedTarget().GetProcess().GetExitStatus())' $NODE -- \"$@\" &
    LLDB_PID=$!
    wait $LLDB_PID
    echo done
"""

    run_command = {
        "none": "$NODE \"$@\"",
        "auto": lldb_non_interactive,
        "interactive": lldb_interactive,
    }[lldb_flavor]

    ctx.actions.write(
        output,
        content = """#!/bin/bash --norc
set -eu
export RUNFILES_DIR=${{RUNFILES_DIR:-{runfiles}}}
NODE=${{RUNFILES_DIR}}/{node}
NODE_BIN=$(dirname $NODE)
export PATH=${{NODE_BIN}}:$PATH
{run_command}
""".format(runfiles = "$0.runfiles", node = node_path, run_command = run_command),
        is_executable = True,
    )

    return [
        DefaultInfo(executable = output, runfiles = runfiles),
    ]

node_wrapper = rule(
    implementation = _node_wrapper_impl,
    attrs = {
        "node": attr.label(
            mandatory = True,
            allow_single_file = True,
            executable = True,
            cfg = "target",
        ),
        "_lldb": attr.label(
            default = "//bzl/node:lldb",
            providers = [BuildSettingInfo],
        ),
    },
    executable = True,
)

def _node_tool_wrapper_impl(ctx):
    output = ctx.actions.declare_file(ctx.label.name)
    runfiles = ctx.runfiles(files = ctx.files.node + ctx.files.node_tool + ctx.files.data + [output])

    # Also merge in node's runfiles.
    runfiles = runfiles.merge(ctx.attr.node[DefaultInfo].default_runfiles)

    node_path = "{workspace}/{path}".format(workspace = ctx.workspace_name, path = ctx.file.node.short_path)
    node_tool_path = "{workspace}/{path}".format(workspace = ctx.workspace_name, path = ctx.file.node_tool.short_path)

    ctx.actions.write(
        output,
        is_executable = True,
        content = """#!/bin/bash --norc
set -eu
DIR=$(dirname ${{BASH_SOURCE[0]}})
export RUNFILES_DIR=${{RUNFILES_DIR:-{runfiles}}}
{env}
${{RUNFILES_DIR}}/{node} ${{RUNFILES_DIR}}/{node_tool} "$@"
""".format(
            env = "\n".join(["export {}=\"{}\"".format(key, value) for key, value in ctx.attr.env.items()]),
            runfiles = "$0.runfiles",
            node = node_path,
            node_tool = node_tool_path,
        ),
    )
    return [
        DefaultInfo(executable = output, runfiles = runfiles),
    ]

node_tool_wrapper = rule(
    implementation = _node_tool_wrapper_impl,
    attrs = {
        "node": attr.label(
            mandatory = True,
            allow_single_file = True,
            executable = True,
            cfg = "target",
        ),
        "node_tool": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "data": attr.label_list(default = [], allow_files = True),
        "env": attr.string_dict(default = {}),
    },
    executable = True,
)
