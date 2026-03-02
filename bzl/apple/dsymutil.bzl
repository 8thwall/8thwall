def _dsymutil_aspect_impl(target, ctx):
    # Skip incompatible targets. Ideally we would check for the existance of
    # "IncompatiblePlatformProvider" but this provider is not available in Starlark.
    if not CcInfo in target:
        return []

    dsym_files = []

    os_osx = ctx.attr._os_osx[platform_common.ConstraintValueInfo]
    os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]

    is_apple = ctx.target_platform_has_constraint(os_osx) or ctx.target_platform_has_constraint(os_ios)

    if is_apple and (ctx.rule.kind in ("cc_binary", "cc_test")):
        binary = target[DefaultInfo].files.to_list()[0]
        dsym_files.extend([
            ctx.actions.declare_file("{name}.dSYM/Contents/Resources/DWARF/{name}".format(name = binary.basename), sibling = binary),
            ctx.actions.declare_file("{name}.dSYM/Contents/Info.plist".format(name = binary.basename), sibling = binary),
        ])
        ctx.actions.run(
            arguments = [target.files.to_list()[0].path],
            inputs = target.files,
            outputs = dsym_files,
            executable = ctx.executable._dsymutil,
            mnemonic = "Dsymutil",
            progress_message = "Running dsymutil on %{input}",
            execution_requirements = {"no-sandbox": "1"},  # Run outside the sandbox where the .o files are visible.
        )

    return [
        OutputGroupInfo(dsym = depset(dsym_files, transitive = [target.files]), dsym_only = depset(dsym_files)),
    ]

dsymutil_aspect = aspect(
    implementation = _dsymutil_aspect_impl,
    attrs = {
        "_dsymutil": attr.label(
            default = "//bzl/llvm:dsymutil",
            executable = True,
            cfg = "exec",
        ),
        "_os_osx": attr.label(
            default = "@platforms//os:osx",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_ios": attr.label(
            default = "@platforms//os:ios",
            providers = [platform_common.ConstraintValueInfo],
        ),
    },
)

def _dsym_files_impl(ctx):
    dsym_files = []
    for dep in ctx.attr.deps:
        dsym_files.append(dep[OutputGroupInfo].dsym_only)
    dsym_depset = depset(transitive = dsym_files)

    return [DefaultInfo(files = dsym_depset)]

dsym_files = rule(
    implementation = _dsym_files_impl,
    attrs = {
        "deps": attr.label_list(aspects = [dsymutil_aspect]),
    },
)
