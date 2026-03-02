"""Inspect the execution computer and generate a matching platform rule with constraints."""

def _execution_platform_configure_impl(repository_ctx):
    platform_cpu = repository_ctx.os.arch.lower()
    platform_os = repository_ctx.os.name.lower()

    if platform_os.startswith("mac os"):
        platform_os = "osx"
    if platform_os.startswith("windows"):
        platform_os = "windows"

    if platform_cpu == "amd64" or platform_cpu == "x64":
        platform_cpu = "x86_64"
    elif platform_cpu == "i386" or platform_cpu == "i486" or platform_cpu == "i586" or platform_cpu == "i686" or platform_cpu == "i786":
        platform_cpu = "x86_32"
    elif platform_cpu == "arm64":
        platform_cpu = "aarch64"

    constraints_contents = [
        "HOST_CONSTRAINTS = [",
        "    \"@platforms//cpu:{cpu}\",".format(cpu = platform_cpu),
        "    \"@platforms//os:{os}\",".format(os = platform_os),
    ]

    if platform_os == "linux":
        codename = repository_ctx.execute(["bash", "-c", "lsb_release -cs"]).stdout.strip()
        major_version = int(repository_ctx.execute(["bash", "-c", "lsb_release -rs | cut -f1 -d."]).stdout.strip())

        if platform_cpu == "aarch64":
            # Use "v3" for arm64 Linux.
            constraints_contents += [
                "    \"@the8thwall//bzl/crosstool:v3-linux\",",
            ]
        elif codename == "kali-rolling" or (codename == "focal" and major_version >= 22):
            # Use "v2" for Kali linux or Ubuntu 22+.
            constraints_contents += [
                "    \"@the8thwall//bzl/crosstool:v2-linux\",",
            ]
        else:
            # Fallback to "v1" linux.
            constraints_contents += [
                "    \"@the8thwall//bzl/crosstool:v1-linux\",",
            ]

    constraints_contents += [
        "]",
        "",
    ]

    if platform_os == "windows":
        constraints_contents += [
            "REQUIRES_RESPONSE_FILES = 1",
            "",
        ]
    else:
        constraints_contents += [
            "REQUIRES_RESPONSE_FILES = 0",
            "",
        ]

    repository_ctx.file(
        "constraints.bzl",
        content = "\n".join(constraints_contents),
        executable = False,
    )

    repository_ctx.template(
        "BUILD",
        Label("@the8thwall//bzl/crosstool:execution-platform-configure.BUILD.tpl"),
    )

execution_platform_configure = repository_rule(
    implementation = _execution_platform_configure_impl,
)
