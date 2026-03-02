_UNITY_TARGET_PLATFORMS = ["android_arm64", "android_armv7a", "android_x86_64", "osx_arm64", "osx_x86_64", "ios_arm64", "windows_x86_64"]

def _unity_version_impl(repository_ctx):
    build_content = [
        'load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")',
        'load(":toolchains.bzl", "unity_toolchain")',
        'package(default_visibility = [ "//visibility:public" ])',
        "",
        'constraint_setting(name = "unity-platform-constraint-setting")',
        "",
        'constraint_setting(name = "unity-installed-constraint-setting")',
        "",
        "constraint_value(",
        '    name = "unity-platform-constraint",',
        '    constraint_setting = ":unity-platform-constraint-setting",',
        ")",
        "",
        "constraint_value(",
        '    name = "unity-installed-constraint",',
        '    constraint_setting = ":unity-installed-constraint-setting",',
        ")",
        "",
        'constraint_setting(name = "major-version")',
        "",
        'constraint_setting(name = "minor-version")',
        "",
        'constraint_setting(name = "patch-version")',
        "",
        'toolchain_type(name = "toolchain_type")',
        "",
    ]

    minor_versions = {}
    major_versions = {}
    unity_paths = {}
    unity_contents_paths = {}
    installed_versions = {}
    execution_platforms = []
    toolchains = []
    max_version = None

    os_name = repository_ctx.os.name
    os_type = os_name
    if os_name.find("mac os") != -1:
        os_type = "macos"
    elif os_name.find("windows") != -1:
        os_type = "windows"
    elif os_name.find("linux") != -1:
        os_type = "linux"

    for unity_hub_editor_dir in repository_ctx.attr.unity_search_paths:
        potential_versions = repository_ctx.execute([
            "bash",
            "-c",
            "find \"%s\" -mindepth 1 -maxdepth 1 -type d -exec basename {} \\;" % unity_hub_editor_dir,
        ]).stdout.strip().split("\n")

        for version in potential_versions:
            unity_location = ""
            unity_contents_location = "s"

            if os_type == "macos":
                unity_location = "{hub}/{version}/Unity.app/Contents/MacOS/Unity".format(hub = unity_hub_editor_dir, version = version)
                unity_contents_location = "{hub}/{version}/Unity.app/Contents".format(hub = unity_hub_editor_dir, version = version)
            if os_type == "windows":
                # Default unity exe location in Windows in WSL/msys2 format
                unity_location = "{hub}/{version}/Editor/Unity.exe".format(hub = unity_hub_editor_dir, version = version)
                unity_contents_location = ""

            has_unity_editor = (0 == repository_ctx.execute(["bash", "-c", "test -f '%s'" % unity_location]).return_code)

            if has_unity_editor:
                major, minor, _patch = version.split(".")
                major_spec = major
                minor_spec = "{}.{}".format(major, minor)
                installed_versions[version] = (version, minor_spec, major_spec)
                unity_paths[version] = unity_location
                unity_contents_paths[version] = unity_contents_location
                execution_platforms.append("@{workspace}//:unity-{version}-platform".format(workspace = repository_ctx.name, version = version))
                toolchains.append("@{workspace}//:{version}-toolchain".format(workspace = repository_ctx.name, version = version))
                if not max_version or version > max_version:
                    max_version = version

    if max_version:
        versions = installed_versions[max_version]
        version_constraint_values = []
        for v in versions:
            version_constraint_values.append('        ":{}",'.format(v))
        build_content += [
            "alias(",
            "    name = \"local\",",
            '    actual = ":unity-{}-platform",'.format(max_version),
            ")",
            "",
            "platform(",
            '    name = "host-unity-local-platform",',
            "    constraint_values = [",
            '        ":unity-platform-constraint",',
            '        ":unity-installed-constraint",',
        ] + version_constraint_values + [
            "    ],",
            "    parents = [",
            "        \"@local_config_platform//:host\",",
            "    ],",
            ")",
            "",
        ]
        for ptfm in _UNITY_TARGET_PLATFORMS:
            build_content += [
                "platform(",
                '    name = "{platform}-unity-local-platform",'.format(platform = ptfm),
                "    constraint_values = [",
                '        ":unity-platform-constraint",',
                '        ":unity-installed-constraint",',
            ] + version_constraint_values + [
                "    ],",
                "    parents = [",
                "        \"@the8thwall//bzl:{platform}\",".format(platform = ptfm),
                "    ],",
                ")",
                "",
            ]
    else:
        print("Unity is not installed, this placeholder platform will not satisfy unity version constraints.")
        toolchains.append("@unity-version//:no-unity-installed-toolchain")
        build_content += [
            "# Unity is not installed, this placeholder platform will not satisfy unity version constraints.",
            "platform(",
            '    name = "local",',
            "    constraint_values = [",
            '        ":unity-platform-constraint",',
            "    ],",
            ")",
            "",
            "unity_toolchain(",
            '    name = "no-unity-installed-unity-local-toolchain",',
            '    version = "None",',
            '    unity_path = "None",',
            '    unity_contents_path = "None",',
            ")",
            "",
            "toolchain(",
            '    name = "no-unity-installed-toolchain",',
            "    target_compatible_with = [",
            '        ":unity-platform-constraint",',
            "    ],",
            '    toolchain = ":no-unity-installed-unity-local-toolchain",',
            '    toolchain_type = ":toolchain_type",',
            ")",
            "",
        ]

    for version, versions in installed_versions.items():
        # Collect all of the version constrain lines.
        version_constraint_values = []
        for v in versions:
            version_constraint_values.append('        ":{}",'.format(v))

        # Build the platform and toolchains.
        build_content += [
            "# Unity {}.".format(version, str(version)),
            "platform(",
            '    name = "unity-{}-platform",'.format(version),
            "    constraint_values = [",
            '        ":unity-platform-constraint",',
            '        ":unity-installed-constraint",',
        ] + version_constraint_values + [
            "    ],",
            ")",
            "",
            "platform(",
            '    name = "host-unity-{ver}-platform",'.format(ver = version),
            "    constraint_values = [",
            '        ":unity-platform-constraint",',
            '        ":unity-installed-constraint",',
        ] + version_constraint_values + [
            "    ],",
            "    parents = [",
            "        \"@local_config_platform//:host\",",
            "    ],",
            ")",
            "",
        ]
        for ptfm in _UNITY_TARGET_PLATFORMS:
            build_content += [
                "platform(",
                '    name = "{platform}-unity-{ver}-platform",'.format(platform = ptfm, ver = version),
                "    constraint_values = [",
                '        ":unity-platform-constraint",',
                '        ":unity-installed-constraint",',
            ] + version_constraint_values + [
                "    ],",
                "    parents = [",
                "        \"@the8thwall//bzl:{platform}\",".format(platform = ptfm),
                "    ],",
                ")",
                "",
            ]
        build_content += [
            "unity_toolchain(",
            '    name = "unity-{}",'.format(version),
            '    version = "{}",'.format(version),
            '    unity_path = "{}",'.format(unity_paths[version]),
            '    unity_contents_path = "{}",'.format(unity_contents_paths[version]),
            ")",
            "",
            "toolchain(",
            '    name = "{}-toolchain",'.format(version),
            "    target_compatible_with = [",
            '        ":unity-platform-constraint",',
            '        ":unity-installed-constraint",',
        ]

        for v in versions:
            build_content.append('        ":{}",'.format(v))
        build_content += [
            "    ],",
            '    toolchain = ":unity-{}",'.format(version),
            '    toolchain_type = ":toolchain_type",',
            ")",
            "",
        ]

    major_versions = {}
    minor_versions = {}
    patch_versions = {}

    for version in repository_ctx.attr.versions + list(installed_versions.keys()):
        # Add empty versions = None.
        version_parts = version.split(".")
        if len(version_parts) > 0:
            major_versions[version_parts[0]] = None
        if len(version_parts) > 1:
            minor_spec = "{}.{}".format(version_parts[0], version_parts[1])
            minor_versions[minor_spec] = None
        if len(version_parts) > 2:
            patch_versions[version] = None

    for version in major_versions.keys():
        build_content += [
            "constraint_value(",
            '    name = "{}",'.format(version),
            '    constraint_setting = ":major-version",',
            ")",
            "",
        ]

    for version in minor_versions.keys():
        build_content += [
            "constraint_value(",
            '    name = "{}",'.format(version),
            '    constraint_setting = ":minor-version",',
            ")",
            "",
        ]

    for version in patch_versions.keys():
        build_content += [
            "constraint_value(",
            '    name = "{}",'.format(version),
            '    constraint_setting = ":patch-version",',
            ")",
            "",
        ]

    repository_ctx.file("BUILD", content = "\n".join(build_content))

    bzl_content = [
        "UnityInfo = provider(",
        "    doc = \"Information about how to invoke the Unity app.\",",
        "    fields = [\"version\", \"unity_path\", \"unity_contents_path\"],",
        ")",
        "",
        "def _unity_toolchain_impl(ctx):",
        "    toolchain_info = platform_common.ToolchainInfo(",
        "        unityinfo = UnityInfo(",
        "            version = ctx.attr.version,",
        "            unity_path = ctx.attr.unity_path,",
        "            unity_contents_path = ctx.attr.unity_contents_path,",
        "        ),",
        "    )",
        "    return [toolchain_info]",
        "",
        "unity_toolchain = rule(",
        "    implementation = _unity_toolchain_impl,",
        "    attrs = {",
        "        \"version\": attr.string(mandatory = True),",
        "        \"unity_path\": attr.string(mandatory = True),",
        "        \"unity_contents_path\": attr.string(mandatory = True),",
        "    },",
        ")",
        "",
    ]

    repository_ctx.file("toolchains.bzl", content = "\n".join(bzl_content))

    repo_bzl_content = [
        "def register_unity_toolchains():",
    ]

    if not toolchains and not execution_platforms:
        repo_bzl_content += [
            "    pass",
            "",
        ]
    else:
        repo_bzl_content += [
            "    native.register_execution_platforms(",
            "        \"@unity-version//:local\",",
        ]
        for platform in execution_platforms:
            repo_bzl_content.append("        \"{}\",".format(platform))
        repo_bzl_content += [
            "    )",
            "",
        ]
        if toolchains:
            repo_bzl_content.append(
                "    native.register_toolchains(",
            )
            for tool in toolchains:
                repo_bzl_content.append("        \"{}\",".format(tool))
            repo_bzl_content += [
                "    )",
                "",
            ]

    repository_ctx.file("repo.bzl", content = "\n".join(repo_bzl_content))

# Uses "Unity Hub" to check if a specific version of Unity is installed with
# requested modules. If so, it will generate config_setting that can be used to
# build rules when it is installed.
unity_version = repository_rule(
    implementation = _unity_version_impl,
    local = True,
    attrs = {
        # Versions of Unity, e.g. [ "2021.3.7f1" ], to be added as Bazel constraint_values.
        "versions": attr.string_list(default = []),
        "unity_search_paths": attr.string_list(default = [
            "/Applications/Unity/Hub/Editor",  # Default unity hub installation in macOS
            "/c/Program Files/Unity/Hub/Editor",  # Default unity hub installation in Windows (WSL or msys2 format)
        ]),
    },
)
