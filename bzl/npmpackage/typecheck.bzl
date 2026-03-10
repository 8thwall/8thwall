load("//bzl/js:js.bzl", "js_test")

def _npm_typecheck_test_src(ctx):
    outfile = ctx.actions.declare_file(ctx.label.name + ".ts")

    ctx.actions.run(
        executable = ctx.executable._generator,
        arguments = [
            ctx.file.package.dirname,
            ctx.attr.npm_rule.label.workspace_name,
            outfile.path,
        ],
        inputs = [ctx.file.package] + ctx.files.npm_rule,
        outputs = [outfile],
    )

    return DefaultInfo(files = depset([outfile]))

npm_typecheck_test_src = rule(
    implementation = _npm_typecheck_test_src,
    attrs = {
        "package": attr.label(mandatory = True, allow_single_file = True),
        "npm_rule": attr.label(mandatory = True, allow_files = True),
        "_generator": attr.label(
            default = Label("//bzl/npmpackage:generate-typecheck"),
            executable = True,
            cfg = "exec",
            allow_single_file = True,
        ),
    },
)

# NOTE(christoph) This macro is used to ensure that all libraries imported from npm have their
# types properly understood by typescript. generate-typecheck.ts analyzes the dependencies
# and generates a typescript file that will only compile if all typed libraries are properly parsed.

# If you see an error like:
# [tsl] ERROR in ...npm-mocha.src.ts(11,1)
#      TS2578: Unused '@ts-expect-error' directive.

# Run:
# bazel-bin/bzl/npmpackage/mocha/npm-mocha.src.ts
# vim bazel-bin/bzl/npmpackage/mocha/npm-mocha.src.ts

# And look at the line under the comment on 11. That is the package that is missing types
# even though it should be typed.

# For other errors, it's possible it might need to be added to SKIPPED_PACKAGES or
# DEFAULT_IMPORT_PACKAGES.
def npm_typecheck(name):
    src_name = name + ".src"
    test_name = name + "-test"

    prefix = "@" + name + "//:"

    package_rule = prefix + "package.json"
    npm_rule = prefix + name

    npm_typecheck_test_src(
        name = src_name,
        package = package_rule,
        npm_rule = npm_rule,
        testonly = 1,
    )

    js_test(
        name = test_name,
        size = "small",
        main = [":" + src_name],
        npm_rule = npm_rule,
        esnext = 1,
    )
