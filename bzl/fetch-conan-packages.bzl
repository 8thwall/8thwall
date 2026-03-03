def _conan_fetch_impl(repository_ctx):
    print("CONAN FETCH {} {} REMOTE: {}".format(repository_ctx.os.name, repository_ctx.os.arch, repository_ctx.attr.repository))
    if repository_ctx.os.name == "mac os x":
        host_os = "Macos"
    elif repository_ctx.os.name == "linux":
        host_os = "Linux"
    else:
        host_os = "Unknown"
    if repository_ctx.os.arch == "aarch64":
        host_arch = "armv8"
    elif repository_ctx.os.arch in ["x86_64", "amd64"]:
        host_arch = "x86_64"
    else:
        host_arch = "Unknown"
    build_profile = "{os}-{os}-{arch}-Release.profile".format(os = host_os, arch = host_arch)
    repository_ctx.file("BUILD", content = "", executable = False)
    requires_content = ["[requires]"]
    requires_content.extend(repository_ctx.attr.requires)
    requires_content += [""]
    requires_content += ["[generators]"]
    requires_content += ["BazelDeps"]
    requires_content += [""]
    repository_ctx.file("conanfile.txt", content = "\n".join(requires_content), executable = False)
    failure = 0
    conan_env = {}
    alldeps_content = []
    alldeps_loads = []
    alldeps_functions = []
    for suffix in repository_ctx.attr.profiles:
        host_profile = repository_ctx.attr.profiles[suffix].format(os = host_os)  #bazel target
        check_profile_result = repository_ctx.execute([
            "conan",
            "profile",
            "show",
            host_profile,
        ], environment = conan_env, quiet = False)
        if check_profile_result.return_code > 0:
            continue
        remotes_result = repository_ctx.execute([
            "conan",
            "remote",
            "list",
        ], environment = conan_env, quiet = False)
        conan_install_cmd = [
            "conan",
            "install",
            "-r",
            repository_ctx.attr.repository,
            ".",
            "-pr:b",
            build_profile,
            "-pr:h",
            host_profile,
            "-if",
            "conanfiles_{}".format(suffix),
        ]
        print("CONAN FETCH COMMAND: {}".format(conan_install_cmd))
        install_result = repository_ctx.execute(conan_install_cmd, environment = conan_env, quiet = False)
        if install_result.return_code > 0:
            print(install_result.stdout)
            print(install_result.stderr)
            failure += 1
        conanfiles = repository_ctx.execute([
            "cat",
            "conanfiles_{suffix}/dependencies.bzl".format(suffix = suffix),
        ], quiet = True)
        depsbzl = conanfiles.stdout
        depsbzl = depsbzl.replace("load_conan_dependencies", "load_conan_dependencies_{}".format(suffix))
        depsbzl = depsbzl.replace("name=\"", "name=\"conan_{}_".format(suffix))
        repository_ctx.file("conanfiles_{suffix}/dependencies.bzl".format(suffix = suffix), content = depsbzl, executable = False)
        buildfiles = repository_ctx.execute([
            "find",
            "conanfiles_{}".format(suffix),
            "-type",
            "f",
            "-iname",
            "BUILD",
        ], quiet = True)
        for f in buildfiles.stdout.strip().split("\n"):
            clean = repository_ctx.execute([
                "perl",
                "-i",
                "-ne",
                'print unless m/.*?\"@.*?\",.*/',
                f,
            ], quiet = True)
        alldeps_loads += ["load('@conan-packages//conanfiles_{suffix}:dependencies.bzl', 'load_conan_dependencies_{suffix}')".format(suffix = suffix)]
        alldeps_functions += ["  load_conan_dependencies_{}()".format(suffix)]
    alldeps_content += alldeps_loads
    alldeps_content += ["def load_all_conan_deps():"]
    alldeps_content += alldeps_functions
    repository_ctx.file("load_all_conan_deps.bzl", content = "\n".join(alldeps_content), executable = False)

    if failure != 0:
        fail("Unable to install conan profile {}".format(repository_ctx.attr.profiles))

conan_fetch = repository_rule(
    implementation = _conan_fetch_impl,
    attrs = {
        "requires": attr.string_list(default = []),
        "repository": attr.string(default = "conan-nianticar-upgrade"),
        "profiles": attr.string_dict(mandatory = True),
    },
)
