def _configure_rosetta_impl(repository_ctx):
    """Implementation for the local_tool rule."""
    is_osx = False
    is_arm64 = False
    has_rosetta2 = False

    os = repository_ctx.os.name.lower()
    if os.startswith("mac os"):
        is_osx = True

    cpu_triple = repository_ctx.execute(["bash", "-c", "echo $MACHTYPE"]).stdout
    if cpu_triple.startswith("aarch64") or cpu_triple.startswith("arm64"):
        is_arm64 = True

    if is_osx and is_arm64:
        has_rosetta2 = (
            0 == repository_ctx.execute(["bash", "-c", "arch -x86_64 /usr/bin/true"]).return_code
        )

    if has_rosetta2:
        repo_bzl_contents = """
# Since this is an OSX arm64 host platform with rosetta installed, register
# osx_x86_64 as a valid execution platform. This is useful for running tools
# that require x86_64 architectures on Apple Silicon.
def register_rosetta_execution_platform():
    native.register_execution_platforms(
        "@{name}//:platform",
    )
""".format(name = repository_ctx.name)
    else:
        repo_bzl_contents = """
# This is not an OSX arm64 host platform with rosetta installed, so pass here.
def register_rosetta_execution_platform():
    pass
"""

    repository_ctx.file(
        "repo.bzl",
        content = repo_bzl_contents,
        executable = False,
    )

    repository_ctx.file(
        "BUILD",
        content = '''
constraint_setting(
    name = "platform_setting"
)

constraint_value(
    name = "platform_constraint",
    constraint_setting = ":platform_setting",
    visibility = ["//visibility:public"],
)

platform(
    name = "platform",
    visibility = ["//visibility:public"],
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:osx",
        ":platform_constraint",
    ],
)
''',
        executable = False,
    )

# If this is an OSX arm64 execution platform with rosetta installed, provide a
# workspace rule in repo.bzl to add osx_x86_64 as a valid execution platform.
# This is useful for running tools that require x86_64 architectures.
configure_rosetta = repository_rule(
    implementation = _configure_rosetta_impl,
    local = True,
)
