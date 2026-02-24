"""
Repository rule for installing npm packages.
"""

def _local_fail(msg):
    """Output failure message."""
    red = "\033[0;31m"
    no_color = "\033[0m"
    fail("\n%snpm install error:%s %s\n" % (red, no_color, msg))

def map(f, list):
    return [f(x) for x in list]

def _execute(repository_ctx, *args, **kwargs):
    """Execute a command, return stdout if succeed and throw an error if it fails."""
    result = repository_ctx.execute(*args, **kwargs)

    if result.return_code != 0:
        _local_fail(result.stderr or result.stdout)

    return result.stdout.strip()

# If the npm rule doesn't install any packages, we need to ensure the node_modules folder exists
# so we can run find on it.
def _ensure_node_modules_folder(ctx):
    _execute(
        ctx,
        ["mkdir", "-p", "node_modules"],
    )

def _npm_package_impl(repository_ctx):
    # Symlink the package and package-lock.
    repository_ctx.symlink(repository_ctx.attr.package, "package.json")
    repository_ctx.symlink(repository_ctx.attr.package_lock, "package-lock.json")

    for patchFile in repository_ctx.attr.patches:
        repository_ctx.symlink(patchFile, "patches/%s" % repository_ctx.path(patchFile).basename)

    node = repository_ctx.path(repository_ctx.attr.node).realpath
    node_bin_dir = node.dirname
    npm = "{}/npm".format(node_bin_dir)

    env_vars = dict(repository_ctx.attr.env)
    env_vars.update({
        "NODE_ENV": "production",
        "PATH": "{}:{}".format(node_bin_dir, repository_ctx.os.environ["PATH"]),
    })

    if "NPM_CONFIG_REGISTRY" in repository_ctx.os.environ:
        env_vars.update({"NPM_CONFIG_REGISTRY": repository_ctx.os.environ["NPM_CONFIG_REGISTRY"]})

    _execute(
        repository_ctx,
        [npm, "ci", "--loglevel=error", "--also=dev", "--prefix", "."],
        environment = env_vars,
    )
    _ensure_node_modules_folder(repository_ctx)

    exported_filenames = [f.name for f in repository_ctx.attr.exports_files]

    if repository_ctx.attr.export_zip:
        _execute(
            repository_ctx,
            # NOTE(christoph): This prevents date modified timestamps from changing the output zip
            ["find", ".", "-exec", "touch", "-t", "198001010000", "{}", "+"],
        )

        _execute(
            repository_ctx,
            ["zip", "-qqr", "node_modules.zip", "node_modules", "package.json", "package-lock.json", "patches"],
        )

        exported_filenames.append("node_modules.zip")

    # Create a sha1 hash of the folder to capture the state of the node_modules folder, without
    # depending on bazel to compute the hash of a directory which is unsound.
    # This fingerprint file will be used to resolve to the full set of dependencies by following
    # the symlink.
    result = repository_ctx.execute([
        "bash",
        "-c",
        "find -L . -type f -print0 | sort -z | xargs -0 sha1sum | sha1sum | cut -d \" \" -f 1",
    ])

    if result.return_code != 0:
        fail("Failed to compute sha1 hash")

    repository_ctx.file("fingerprint.sha1".format(repository_ctx.name), content = result.stdout, executable = False)

    repository_ctx.file(
        "BUILD",
        content = """
filegroup(
    name = "{name}",
    srcs = [
        "fingerprint.sha1",
    ],
    visibility = ["//visibility:public"],
)
exports_files(
    [
        "package.json",
        "package-lock.json",
{exported_files}
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "modules", # NOTE(christoph): The filegroup can't be the same name as the folder itself.
    srcs = [
        "node_modules",
        "fingerprint.sha1",
    ],
    visibility = ["//visibility:public"],
)
""".format(
            name = repository_ctx.attr.name,
            exported_files = ",\n".join(["        \"{}\"".format(f) for f in exported_filenames]),
        ),
        executable = False,
    )

npm_package = repository_rule(
    implementation = _npm_package_impl,
    attrs = {
        "package": attr.label(mandatory = True, allow_single_file = True),
        "package_lock": attr.label(mandatory = True, allow_single_file = True),
        "patches": attr.label_list(default = [], allow_files = True),
        "node": attr.label(default = "@nodejs_host//:bin/node", allow_single_file = True),
        "exports_files": attr.label_list(default = [], allow_files = True),
        "excluded_files": attr.string_list(default = []),
        "export_zip": attr.bool(default = False),
        "env": attr.string_dict(default = {}),
    },
)
