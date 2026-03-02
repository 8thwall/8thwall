def _env_vars_impl(repository_ctx):
    """Implementation for the env_vars rule."""
    found_vars = {}
    os_name = repository_ctx.os.name.lower()

    if os_name.find("windows") != -1:
        print("Windows detected, saving environment variables in unix format")

        # For windows, we have to save path in unix format to be compatible with the rest of the
        # bash script
        for key in repository_ctx.attr.env:
            new_value = "" if key not in repository_ctx.os.environ else repository_ctx.os.environ.get(key)
            found_vars[key] = new_value.replace("\\", "/")
    else:
        for key in repository_ctx.attr.env:
            new_value = "" if key not in repository_ctx.os.environ else repository_ctx.os.environ.get(key)
            found_vars[key] = new_value

    repository_ctx.file("BUILD", content = "")
    repository_ctx.file("vars.bzl", content = "{}\n".format("\n".join(["{k} = \"\"".format(k = key) if (value == None) else "{k} = \"{v}\"".format(k = key, v = value) for key, value in found_vars.items()])))

# Copy selected env variables to a .bzl file upon bazel start-up. Variables are
# cached until the next bazel clean, and are intended to be used for permanent
# locations on host filesystems that have no effect on bazel output.
env_vars = repository_rule(
    implementation = _env_vars_impl,
    local = True,
    attrs = {
        "env": attr.string_list(default = []),
    },
)
