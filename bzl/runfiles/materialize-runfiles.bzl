"""Materialize the runfiles tree.

This file contains a Starlark rule materialize_runfiles for materializing the
merged 'runfiles' tree for a set of inputs. This is useful for packaging
runtime dependencies when exporting binaries to be executed or deployed
elsewhere. Similar to bazel runfiles the outputs are symlinks, and dependent
rules can copy or package these files as desired.
"""

def _declare_workspace_runfile(ctx, runfiles_dir, runfile, short_path):
    workspace_name = runfile.owner.workspace_name if runfile.owner and runfile.owner.workspace_name else ctx.workspace_name
    qualified_path = "{}/{}/{}".format(runfiles_dir, workspace_name, short_path)
    return ctx.actions.declare_file(qualified_path)

def _materialize_runfiles_impl(ctx):
    runfiles_dir = ctx.label.name

    runfiles = ctx.runfiles()
    runfiles = runfiles.merge_all([dep.default_runfiles for dep in ctx.attr.deps])
    outs = []

    # Regular runfiles symlink the path in the workspace-owned directory.
    for runfile in runfiles.files.to_list():
        out = _declare_workspace_runfile(ctx, runfiles_dir, runfile, runfile.short_path)
        ctx.actions.symlink(output = out, target_file = runfile)
        outs.append(out)

    # Empty filenames are created in the workspace owned directory.
    for runfile in runfiles.empty_filenames.to_list():
        out = _declare_workspace_runfile(ctx, runfiles_dir, runfile, runfile.short_path)
        ctx.actions.write(output = out, content = "")
        outs.append(out)

    # Symlinks use the provided symlink name to the file in the workspace owned directory.
    for symlink in runfiles.symlinks.to_list():
        out = _declare_workspace_runfile(ctx, runfiles_dir, symlink.target_file, symlink.path)
        ctx.actions.symlink(output = out, target_file = symlink.target_file)
        outs.append(out)

    # Root symlinks use the provided symlink name to the file in the top-level runfile directory.
    for root_symlink in runfiles.root_symlinks.to_list():
        qualified_path = "{}/{}".format(runfiles_dir, root_symlink.path)
        out = ctx.actions.declare_file(qualified_path)
        ctx.actions.symlink(output = out, target_file = root_symlink.target_file)
        outs.append(out)

    return [
        DefaultInfo(files = depset(outs)),
    ]

materialize_runfiles = rule(
    implementation = _materialize_runfiles_impl,
    attrs = {
        "deps": attr.label_list(mandatory = True),
    },
)
