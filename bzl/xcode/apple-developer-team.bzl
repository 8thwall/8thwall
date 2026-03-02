"""This module defines a repository rule for managing Apple developer team and provisioning profile Bazel flags."""

def _define_apple_developer_team_impl(repository_ctx):
    build_content = [
        'load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")',
        'load("@bazel_skylib//rules:common_settings.bzl", "string_flag")',
        "",
    ]

    # We are going to iterate through all of the provisioning profiles on the user's system and find:
    # - The team_name to team_id pairs.
    # - The provisioning profile name to path pairs.
    # - A personal Apple Development certificate.
    # Note that the team_name is set in the WORKSPACE, it is not stored on the provisioning profile
    # itself, whereas the name is the name of the provisioning profile, and is stored on it.
    found_profile_team_names_to_team_ids = {}
    found_profile_names_to_paths = {}
    personal_apple_certificate = ""

    if repository_ctx.os.name == "mac os x":
        home = repository_ctx.os.environ.get("HOME")

        # XCode <=15 stores provisioning profiles in MobileDevice/, XCode >=16 in Developer/Xcode/.
        provisioning_profile_dirs = [
            "{}/Library/MobileDevice/Provisioning Profiles".format(home),
            "{}/Library/Developer/Xcode/UserData/Provisioning Profiles".format(home),
        ]

        # Find all provisioning profiles.
        provisioning_profile_paths = []
        for provisioning_profile_dir in provisioning_profile_dirs:
            res = repository_ctx.execute(["find", provisioning_profile_dir, "-type", "f"])
            if res.return_code == 0:
                for profile in res.stdout.strip().split("\n"):
                    if profile not in provisioning_profile_paths:
                        provisioning_profile_paths.append('"' + profile + '"')

        # Find the team_id and name for each provisioning profile.
        cmd = """
            for file_path in {provisioning_profile_paths}; do
                certificate=$(security cms -D -i "$file_path")
                team_id=$(/usr/libexec/PlistBuddy -c 'Print :Entitlements:com.apple.developer.team-identifier' /dev/stdin <<< "$certificate")
                name=$(/usr/libexec/PlistBuddy -c 'Print :Name' /dev/stdin <<< "$certificate")
                echo "$team_id,$name"
            done
        """.format(
            provisioning_profile_paths = " ".join(provisioning_profile_paths),
        )
        res = repository_ctx.execute(["bash", "-c", cmd])
        if res.return_code == 0:
            team_id_and_name_tuples = res.stdout.strip().split("\n")
            for index, team_id_and_name_tuple in enumerate(team_id_and_name_tuples):
                if team_id_and_name_tuple == "":
                    continue
                team_id, name = team_id_and_name_tuple.split(",", 1)

                # Check if the team_id is one of the ids we are looking for (i.e. specified in WORKSPACE).
                if team_id:
                    team_name = repository_ctx.attr.team_identifiers.get(team_id)
                    if team_name and team_name not in found_profile_team_names_to_team_ids:
                        found_profile_team_names_to_team_ids[team_name] = team_id

                # Also keep track of the provisioning profile name to path pairs.
                if name and name not in found_profile_names_to_paths:
                    found_profile_names_to_paths[name] = provisioning_profile_paths[index]

        # Find the first personal Apple Development certificate. Note that we could instead the
        # Apple Development certificate for each the team that the user is a member of, i.e. by
        # matching team_id to the certificate we find here. That would then require users to
        # specify which certificate to fetch from, i.e. `@apple-developer-team//:foo-personal-certificate`
        # instead of `@apple-developer-team//:personal-certificate`. So here we just start with the
        # simplest case of finding the first personal certificate.
        res = repository_ctx.execute(["security", "find-certificate", "-p", "-c", "Apple Development", "login.keychain"])
        if res.return_code == 0:
            # NOTE(paris): We specify /usr/bin/openssl here, because if you have a version installed with homebrew, the output may be different.
            # The best solution would be to use the bazel-built openssl, but repository_rule() is run during the Loading Phase, when bazel build
            # targets are not available. So we just use the system openssl, which is not great, but passable b/c we're only running this on OSX.
            res_x509 = repository_ctx.execute(["bash", "-c", "echo '{}' | /usr/bin/openssl x509 -noout -subject".format(res.stdout.strip())])
            if res_x509.return_code == 0:
                subject_parts = res_x509.stdout.strip().split("/")
                ou_value = None
                cn_value = None
                for part in subject_parts:
                    if part.startswith("OU="):
                        ou_value = part.split("=", 1)[1]
                    elif part.startswith("CN="):
                        cn_value = part.split("=", 1)[1]

                if ou_value and cn_value:
                    personal_apple_certificate = cn_value

    build_content += [
        "bool_flag(",
        '    name = "personal-certificate-flag",',
        "    build_setting_default = {},".format(personal_apple_certificate != ""),
        '    visibility = ["//visibility:public"],',
        ")",
    ]
    build_content += [
        "string_flag(",
        '    name = "personal-certificate",',
        '    build_setting_default = "{}",'.format(personal_apple_certificate),
        '    visibility = ["//visibility:public"],',
        ")",
        "",
    ]
    build_content += [
        "string_flag(",
        '    name = "empty-personal-certificate",',
        '    build_setting_default = "",',
        '    visibility = ["//visibility:public"],',
        ")",
    ]

    for team_id, team_name in repository_ctx.attr.team_identifiers.items():
        build_content += [
            "# Apple Developer Team Identifier = {}.".format(team_id),
            "bool_flag(",
            '    name = "{}-flag",'.format(team_name),
            "    build_setting_default = {},".format(team_name in found_profile_team_names_to_team_ids),
            ")",
            "config_setting(",
            '    name = "{}",'.format(team_name),
            '    flag_values = {{ ":{}-flag": "True" }},'.format(team_name),
            ")",
            "config_setting(",
            '    name = "{}-and-personal-certificate",'.format(team_name),
            "    flag_values = {",
            '        ":{}-flag": "True",'.format(team_name),
            '        ":personal-certificate-flag": "True",',
            "    },",
            ")",
            "",
        ]

    for name, target_name in repository_ctx.attr.provisioning_profile_names.items():
        build_content += [
            "# Provisioning Profile Name = {}.".format(name),
            "bool_flag(",
            '    name = "{}-flag",'.format(target_name),
            "    build_setting_default = {},".format(name in found_profile_names_to_paths),
            '    visibility = ["//visibility:public"],',
            ")",
            "string_flag(",
            '    name = "{}",'.format(target_name),
            "    build_setting_default = {},".format(found_profile_names_to_paths[name] if name in found_profile_names_to_paths else "\"\""),
            '    visibility = ["//visibility:public"],',
            ")",
            "config_setting(",
            '    name = "{}-and-personal-certificate",'.format(target_name),
            "    flag_values = {",
            '        ":{}-flag": "True",'.format(target_name),
            '        ":personal-certificate-flag": "True",',
            "    },",
            ")",
            "",
        ]

    build_content += [
        "string_flag(",
        '    name = "empty-provisioning-profile",',
        '    build_setting_default = "",',
        '    visibility = ["//visibility:public"],',
        ")",
    ]

    repository_ctx.file("BUILD", content = "\n".join(build_content))

# Creates custom bazel flags for a Apple developer teams. Useful for
# designating that certain bazel targets should only be built if the developer
# has installed an Apple provisioning profile matching a specific Apple
# developer team. If the provisionin profile is not installed, the registration
# will not occur.
apple_developer_team = repository_rule(
    implementation = _define_apple_developer_team_impl,
    local = True,
    attrs = {
        "team_identifiers": attr.string_dict(mandatory = True),
        "provisioning_profile_names": attr.string_dict(default = {}),
    },
)
