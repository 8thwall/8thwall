def _execute(repository_ctx, command):
    """Execute a command, return stdout if succeed and throw an error if it fails."""
    result = repository_ctx.execute(command)
    if result.stderr:
        fail(result.stderr)
    else:
        return result.stdout.strip()

def _http_toolchain_impl(repository_ctx):
    """Implementation for the http_toolchain rule."""
    repository_ctx.download_and_extract(
        repository_ctx.attr.url,
        sha256 = repository_ctx.attr.sha256,
        stripPrefix = repository_ctx.attr.strip_prefix,
    )

    for fileToAdd in repository_ctx.attr.additional_files:
        repository_ctx.symlink(fileToAdd, repository_ctx.path(fileToAdd).basename)

    toolchainDir = repository_ctx.path(".")

    repository_ctx.file(
        "defines.bzl",
        content = """# Definitions for {name} toolchain.
TOOLCHAIN = "{toolchain}"
""".format(name = repository_ctx.name, toolchain = toolchainDir),
        executable = False,
    )
    if repository_ctx.attr.build_file:
        repository_ctx.template("BUILD", repository_ctx.attr.build_file, executable = False)
    else:
        repository_ctx.file(
            "BUILD",
            content = """
# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "toolchain",
    srcs = [],
)
""",
            executable = False,
        )

# Downloads and extracts a compilation toolchain, and creates variables in a
# .bzl file needed for locating the toolchain.
http_toolchain = repository_rule(
    implementation = _http_toolchain_impl,
    attrs = {
        "url": attr.string(),
        "sha256": attr.string(),
        "build_file": attr.label(allow_single_file = True),
        "strip_prefix": attr.string(),
        # Additional files to include in the toolchain.
        "additional_files": attr.label_list(),
    },
)
