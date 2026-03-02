load("//bzl/node:node-source-transition.bzl", "node_source_transition")
load("//bzl/wasm:wasm-platforms-transition.bzl", "wasm_platforms_transition")

def _declare_workspace_runfile(ctx, runfiles_dir, runfile, short_path):
    workspace_name = runfile.owner.workspace_name if runfile.owner and runfile.owner.workspace_name else ctx.workspace_name
    qualified_path = "{}/{}/{}".format(runfiles_dir, workspace_name, short_path)
    return ctx.actions.declare_file(qualified_path)

def _materialize_node_runfiles_impl(ctx):
    # Exclude certain files from the final build
    def should_exclude(file):
        exclude_extensions = [".map", ".d.ts"]
        for ext in exclude_extensions:
            if file.short_path.endswith(ext):
                return True
        return False

    # Build all deps with source-built node JS transition
    native_deps = depset(
        transitive = [
            depset([file for file in deps[DefaultInfo].files.to_list() if not should_exclude(file)])
            for deps in ctx.attr.deps
        ],
    )

    wasm_deps = depset(
        transitive = [
            depset([file for file in deps[DefaultInfo].files.to_list() if not should_exclude(file)])
            for deps in ctx.attr.wasm_deps
        ],
    )

    runfiles = ctx.runfiles()
    runfiles = runfiles.merge_all([dep[DefaultInfo].default_runfiles for dep in ctx.attr.deps])
    runfiles = runfiles.merge_all([dep[DefaultInfo].default_runfiles for dep in ctx.attr.wasm_deps])

    runfiles_dir = "runfiles"
    runfiles_outs = []

    # Regular runfiles symlink the path in the workspace-owned directory.
    for runfile in runfiles.files.to_list():
        if should_exclude(runfile):
            continue

        out = _declare_workspace_runfile(ctx, runfiles_dir, runfile, runfile.short_path)
        ctx.actions.symlink(output = out, target_file = runfile)
        runfiles_outs.append(out)

    # Empty filenames are created in the workspace owned directory.
    for runfile in runfiles.empty_filenames.to_list():
        out = _declare_workspace_runfile(ctx, runfiles_dir, runfile, runfile.short_path)
        ctx.actions.write(output = out, content = "")
        runfiles_outs.append(out)

    # Symlinks use the provided symlink name to the file in the workspace owned directory.
    for symlink in runfiles.symlinks.to_list():
        out = _declare_workspace_runfile(ctx, runfiles_dir, symlink.target_file, symlink.path)
        ctx.actions.symlink(output = out, target_file = symlink.target_file)
        runfiles_outs.append(out)

    # Root symlinks use the provided symlink name to the file in the top-level runfile directory.
    for root_symlink in runfiles.root_symlinks.to_list():
        qualified_path = "{}/{}".format(runfiles_dir, root_symlink.path)
        out = ctx.actions.declare_file(qualified_path)
        ctx.actions.symlink(output = out, target_file = root_symlink.target_file)
        runfiles_outs.append(out)

    all_files = depset(transitive = [native_deps, wasm_deps, depset(runfiles_outs)])

    return [
        DefaultInfo(
            files = all_files,
        ),
    ]

materialize_node_runfiles = rule(
    implementation = _materialize_node_runfiles_impl,
    cfg = node_source_transition,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "deps": attr.label_list(
            default = [],
        ),
        "wasm_deps": attr.label_list(
            default = [],
            cfg = wasm_platforms_transition,
        ),
    },
)
