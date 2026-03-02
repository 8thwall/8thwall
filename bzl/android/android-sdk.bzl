"""module
Bazel rule to ensure requested Android SDK packages are installed locally.
Package installation on the build runners requires manual action, so this rule
fails the build if packages are missing or need to be uninstalled.
"""

def _android_sdk_impl(repository_ctx):
    fail_build = False
    fail_build_reasons = []
    is_ci_machine = False
    run_sdkmanager = False

    # Check whether all the required SDK packages are installed.
    packages_needing_install = []

    # Generally this will be empty, however there is a bad
    # symlink in ndk/23.1.7779620 which causes bazel to crash, so let's figure out if it needs to be uninstalled.
    packages_needing_uninstall = []

    package_versions = {}

    BAD_ANDROID_NDK_PACKAGES = [
        "ndk;23.1.7779620",
    ]

    # Check for the ANDROID_HOME environment variable (pointing to the Android SDK).
    android_home = repository_ctx.os.environ.get("ANDROID_HOME")

    ci_env_var = repository_ctx.os.environ.get("CI")
    if ci_env_var:
        is_ci_machine = True

    # Find packages that need to be installed.
    for pkg in repository_ctx.attr.packages:
        pkg_parts = pkg.split(";")
        if len(pkg_parts) == 2:
            package_versions[pkg_parts[0]] = pkg_parts[1]

        pkg_path = "{}/{}".format(android_home, "/".join(pkg_parts))

        # Package needs install if the directory doesn't exist, is empty, or has an .installer file meaning installation was previously interrupted.
        needs_install = repository_ctx.execute(["sh", "-c", "[ -d \"/{pkg}/\" ] && [ ! -z \"$(ls {pkg})\" ] && [ ! -f {pkg}/.installer ] || exit 1".format(pkg = pkg_path)]).return_code
        if needs_install:
            packages_needing_install.append(pkg)

    # Check whether any packages need to be uninstalled.
    for pkg in BAD_ANDROID_NDK_PACKAGES:
        pkg_parts = pkg.split(";")
        pkg_path = "{}/{}".format(android_home, "/".join(pkg_parts))

        # Package not installed if the directory doesn't exist, is empty, or has an .installer file meaning installation was previously interrupted.
        not_installed = repository_ctx.execute(["sh", "-c", "[ -d \"/{pkg}/\" ] && [ ! -z \"$(ls {pkg})\" ] && [ ! -f {pkg}/.installer ] || exit 1".format(pkg = pkg_path)]).return_code
        if not not_installed:
            # Uninstall the package.
            packages_needing_uninstall.append(pkg)

    # If we're running on CI and packages need to change, fail the build.
    if is_ci_machine and (packages_needing_install or packages_needing_uninstall):
        fail_build = True
        for each in packages_needing_install:
            fail_build_reasons.append("Android package {} needs to be installed to this build runner. Please share this log in #gitlab-admin.".format(each))
        for each in packages_needing_uninstall:
            fail_build_reasons.append("Android package {} needs to be uninstalled from this build runner. Please share this log in #gitlab-admin.".format(each))

    # If this is a local machine (not CI), ensure that sdkmanager is present.
    if not is_ci_machine and (packages_needing_install or packages_needing_uninstall):
        if not android_home:
            fail_build = True
            fail_build_reasons.append("ANDROID_HOME environment variable not set")

        # Find the sdkmanager application
        sdkmanager_symlink = repository_ctx.which("sdkmanager")

        if not sdkmanager_symlink:
            fail_build = True
            fail_build_reasons.append("Could not find 'sdkmanager' in PATH.")

        realpath = repository_ctx.which("realpath")
        if not realpath:
            fail_build = True
            fail_build_reasons.append("Could not find 'realpath' in PATH.")

        run_sdkmanager = True

    # If this is a local machine (not CI), try to resolve the packages changes.
    if run_sdkmanager:
        # sdkmanager is the android tool for managing SDK packages, but it
        # requires a Java runtime and is unfortunately coupled to the JVM
        # version. Some require Java v8, some require Java v11, some might work
        # work with the default. Let's try all 3.
        java_versions = {}

        java_hints = []

        java_home_dir = repository_ctx.os.environ.get("JAVA_HOME")
        if not java_home_dir:
            java_hints.append("No Java JDK found in env variable JAVA_HOME")
        else:
            java_versions[java_home_dir] = java_home_dir

        def get_java_version(java_path):
            java_version = repository_ctx.execute([
                "sh",
                "-c",
                "{}/bin/java -XshowSettings:all -version 2>&1 | awk '/java\\.specification\\.version/ {{ print $3 }}'".format(java_path),
            ]).stdout.strip()
            if java_version == "1.8":
                java_version = 8
            return java_version

        if repository_ctx.os.name == "mac os x":
            # Look for a Java 8 (1.8.x).
            java_result = repository_ctx.execute(["/usr/libexec/java_home", "-F", "-v", "1.8"], quiet = True)
            if java_result.return_code != 0:
                java_hints.append("Unable to find a Java 8 JDK with \\\\`/usr/libexec/java_home -F -v 1.8\\\\`")
            else:
                java_path = java_result.stdout.strip()
                java_versions[java_path] = get_java_version(java_path)

            # Look for a Java 11 (11.x).
            java_result = repository_ctx.execute(["/usr/libexec/java_home", "-F", "-v", "11"], quiet = True)
            if java_result.return_code != 0:
                java_hints.append("Unable to find a Java 11 JDK with \\\\`/usr/libexec/java_home -F -v 11\\\\`")
            else:
                java_path = java_result.stdout.strip()
                java_versions[java_path] = get_java_version(java_path)

            # Look for any Java.
            java_result = repository_ctx.execute(["/usr/libexec/java_home"], quiet = True)
            if java_result.return_code == 0:
                java_path = java_result.stdout.strip()
                java_versions[java_path] = get_java_version(java_path)

        elif repository_ctx.os.name == "linux":
            # In most Linux installation the /usr/bin/javac is a symlink to the actual installation
            java_result = repository_ctx.execute(["sh", "-c", "readlink -e /usr/bin/javac | xargs dirname | xargs dirname"], quiet = True)
            if java_result.return_code != 0:
                java_hints.append("Unable to find a Java JDK by resolving symlink at /usr/bin/javac")
            else:
                java_path = java_result.stdout.strip()
                java_versions[java_path] = get_java_version(java_path)

        # Touch this config to ensure it exists.
        repository_ctx.execute(
            [
                "sh",
                "-c",
                "mkdir -p ~/.android && touch -a ~/.android/repositories.cfg",
            ],
        )

        sdkmanager_success = False

        for java_path, java_version in java_versions.items():
            if sdkmanager_success:
                # We succeeded with a java_version.
                break
            else:
                # Start by assuming success.
                sdkmanager_success = True

            result = repository_ctx.execute(
                [
                    "sh",
                    "-c",
                    "yes | sdkmanager --sdk_root={} --licenses".format(android_home),
                ],
                environment = {
                    "JAVA_HOME": java_path,
                },
                quiet = True,
            )
            if result.return_code != 0:
                sdkmanager_success = False
                fail_build_reasons.append("Failed to run sdkmanager with a Java {} JDK, likely an incompatible JDK version".format(java_version))
                continue

            for pkg in packages_needing_install:
                repository_ctx.report_progress("Installing Android sdkmanager package \"%s\"" % pkg)

                result = repository_ctx.execute(
                    [
                        "sdkmanager",
                        "--verbose",
                        "--sdk_root={}".format(android_home),
                        pkg,
                    ],
                    environment = {
                        "JAVA_HOME": java_path,
                    },
                    quiet = False,
                )
                if result.return_code != 0:
                    sdkmanager_success = False
                    print("Failed to install {}, with return code{}".format(pkg, result.return_code))
                    fail_build_reasons.append("Failed to install the Android SDK package \\\\\"{}\\\\\" with sdkmanager and a Java {} JDK".format(pkg, java_version))
                    continue

            for pkg in packages_needing_uninstall:
                repository_ctx.report_progress("Uninstalling Android sdkmanager package \"%s\"" % pkg)

                result = repository_ctx.execute(
                    [
                        "sdkmanager",
                        "--sdk_root={}".format(android_home),
                        "--uninstall",
                        pkg,
                    ],
                    environment = {
                        "JAVA_HOME": java_path,
                    },
                    quiet = True,
                )
                if result.return_code != 0:
                    sdkmanager_success = False
                    fail_build_reasons.append("Failed to uninstall the Android SDK package \\\\\"{}\\\\\" with sdkmanager and a Java {} JDK".format(pkg, java_version))
                    continue

        if not sdkmanager_success:
            # Unable to succeed with any combination of Java version and sdkmanager
            fail_build = True
            fail_build_reasons.extend(java_hints)

    # Generate a different build file depending on whether we passed or failed.
    if fail_build:
        # Android SDK requirements not satisfied, so create a BUILD file that
        # will generate compile-time errors when you build for Android,
        # explaining the reasons why the Android prerequisites were unsatisfied.
        build_file_contents = """
# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.
package(default_visibility = ["//visibility:public"])
filegroup(
    name = "stub",
    srcs = [":error_message"],
)

filegroup(
    name = "adb",
    srcs = [":error_message"],
)

genrule(
    name = "invalid_android_sdk_packages_error",
    outs = [
        "error_message",
        "error_message.jar",
    ],
    cmd = \"\"\"echo -e \\\\\\\\nAndroid SDK issues:\\\\\\\\n \\\\
      ' '\\\\* {}\\\\\\\\n \\\\
\\\\\\\\nResolve these issues, run \\\\`bazel clean\\\\`, then retry the build.\\\\\\\\n;
    exit 1 \"\"\",
)
""".format("\\\\\\\\n \\\\ \\\\* ".join(fail_build_reasons))

    else:
        # Android SDK requirements satisfied, so create a standard BUILD file.
        build_file_contents = """
# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.
package(default_visibility = ["//visibility:public"])
filegroup(
    name = "stub",
    srcs = [],
)

alias(
    name = "adb",
    actual = "@androidsdk//:adb",
)

"""

    # Generate the build file.
    repository_ctx.file(
        "BUILD",
        content = build_file_contents,
    )

    # Generate the bzl file.
    defines_content = [
        "# Definitions for Android toolchain.",
        'ANDROID_SDK = "{sdk}"'.format(sdk = android_home),
    ]

    ndk_version = package_versions.get("ndk")
    if ndk_version:
        defines_content.append('ANDROID_NDK = "{sdk}/ndk/{ndk}"'.format(sdk = android_home, ndk = ndk_version))
        defines_content.append('# ANDROID_NDK_VERSION = "{ndk}"'.format(ndk = ndk_version))

    build_tools_version = package_versions.get("build-tools")
    if build_tools_version:
        defines_content.append('# ANDROID_BUILD_TOOLS_VERSION = "{build_tools}"'.format(build_tools = build_tools_version))

    repository_ctx.file(
        "defines.bzl",
        content = "\n".join(defines_content),
        executable = False,
    )

_android_sdk = repository_rule(
    implementation = _android_sdk_impl,
    attrs = {
        "packages": attr.string_list(default = []),
    },
    local = True,
)

def android_sdk(name, packages = []):
    _android_sdk(
        name = name,
        packages = packages,
    )
