"""
Detects the rules_nodejs host platform repository name.
"""

# NOTE(christoph): rules_nodejs 6 exposes only aliases on @nodejs_host, which cannot be resolved
# by repository_ctx.path. This detects the host platform so rules can point at real files in the
# platform repository (e.g. Label("@nodejs_" + host_platform(ctx) + "//:bin/node")).

_PLATFORM_BY_KERNEL = {
    "Darwin": "darwin",
    "Linux": "linux",
}

_MACHINE_BY_ARCH = {
    "arm64": "arm64",
    "aarch64": "arm64",
    "x86_64": "amd64",
}

def host_platform(repository_ctx):
    kernel = repository_ctx.execute(["uname", "-s"]).stdout.strip()
    machine = repository_ctx.execute(["uname", "-m"]).stdout.strip()
    return "%s_%s" % (
        _PLATFORM_BY_KERNEL.get(kernel, kernel),
        _MACHINE_BY_ARCH.get(machine, machine),
    )
