load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//bzl/unity/impl:transitions.bzl", "unity_platform_transition", "unity_target_arch_transition")
load("//bzl/xcode:xcode.bzl", "XcodeProjectInfo")
load("//bzl/xcode:exportOptionsPList.bzl", "ExportOptionsPlistInfo", "get_bundle_id")
load("//bzl/xcode/impl:xcode_app.bzl", "xcode_app_impl")
load("//bzl/gradle:gradle.bzl", "GRADLE_DIR")
load("//bzl/gradle:gradle.bzl", "GRADLE_8_DIR")
load("//bzl/unity/impl:unity-project.bzl", "clean_up_old_assets")

_UnityStage1Info = provider(
    "Pass information about the unity app.",
    fields = {
        "files": "files",
        "platform": "Name of target platform",
        "app_name": "App name",
        "app_output_name": "App output name",
        "app_dir": "App directory",
    },
)

def copy_action(ctx, src, dst):
    """Action to copy a file from src to dst."""
    ctx.actions.run_shell(
        inputs = [src],
        outputs = [dst],
        command = "cp -R \"%s\" \"%s\"" % (
            src.path,
            dst.path,
        ),
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
        mnemonic = "RunShellCopyAction",
        progress_message = "unity-app.copy_action: Copying %s to %s" % (src.path, dst.path),
    )

def _get_local_platform(ctx):
    os_osx = ctx.attr._os_osx[platform_common.ConstraintValueInfo]
    os_windows = ctx.attr._os_windows[platform_common.ConstraintValueInfo]
    os_android = ctx.attr._os_android[platform_common.ConstraintValueInfo]
    os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]
    os_linux = ctx.attr._os_linux[platform_common.ConstraintValueInfo]
    cpu_x86_64 = ctx.attr._cpu_x86_64[platform_common.ConstraintValueInfo]

    if ctx.target_platform_has_constraint(os_osx):
        return "osx"
    if ctx.target_platform_has_constraint(os_windows):
        if ctx.target_platform_has_constraint(cpu_x86_64):
            return "windows64"
        else:
            return "windows32"
    if ctx.target_platform_has_constraint(os_android):
        return "android"
    if ctx.target_platform_has_constraint(os_ios):
        return "ios-xcode"
    if ctx.target_platform_has_constraint(os_linux):
        if ctx.target_platform_has_constraint(cpu_x86_64):
            return "linux64"
        else:
            return "linux32"
    return None

def contains(data, pattern):
    return data.find(pattern) != -1

def _get_build_commands(ctx, platform, provisioningProfile, distributionMethod, gradleDirectory):
    cpu = ctx.var.get("TARGET_CPU")
    is64bit = contains(cpu, "64") or contains(cpu, "darwin") or contains(cpu, "k8")
    if platform == "osx":
        return ["-executeMethod", "BazelBuild.BuildOSX"]
    elif platform == "windows64":
        return ["-buildWindows64Player"]
    elif platform == "windows32":
        return ["-buildWindows32Player"]
    elif platform == "linux64":
        return ["-buildLinux64Player"]
    elif platform == "linux21":
        return ["-buildLinux32Player"]
    elif platform == "ios-xcode":
        buildArgs = [
            "-buildTarget",
            "iOS",
            "--provisioningProfile",
            provisioningProfile,
            "--distributionMethod",
            distributionMethod,
        ]
        if ctx.attr.is_test_app == False:
            return buildArgs + [
                "-executeMethod",
                "BazelBuild.BuildiOS",
            ]
        else:
            return buildArgs + [
                "-testPlatform",
                "iOS",
            ]
    elif platform == "android":
        buildArgs = [
            "-buildTarget",
            "Android",
            "--gradle",
            gradleDirectory,
        ]
        if ctx.attr.is_test_app == False:
            return buildArgs + [
                "-executeMethod",
                "BazelBuild.BuildAndroid",
            ]
        else:
            return buildArgs + [
                "-testPlatform",
                "Android",
            ]
    else:
        fail("Unsupported platform [%s]" % platform)
        return [""]

def _get_output_name(ctx, platform, app_name):
    if platform == "osx":
        return app_name + ".app"
    elif platform.startswith("windows"):
        return app_name + ".exe"
    elif platform.startswith("linux"):
        return app_name + ".bin"
    elif platform == "ios-xcode":
        if ctx.attr.is_test_app == False:
            return "xcode/%s/Unity-iPhone.xcodeproj" % app_name
        else:
            return "xcode/%s/PlayerWithTests/Unity-iPhone.xcodeproj" % app_name
    elif platform == "android":
        return app_name + ".apk"
    else:
        return ""

def _get_run_script(ctx, platform, app_name, exe, dirname):
    bundle_id = get_bundle_id(app_name)
    if platform == "android":
        return (
            "#!/bin/bash\n" +
            "set -eu\n" +
            "DIR=\"{}\"\n".format(dirname) +
            "APK=${DIR}/%s\n" % exe +
            "RUN=\"true\"\n" +
            "SERIAL=\"\"\n" +
            "while getopts \":r:s:\" opt; do\n" +
            "  case $opt in\n" +
            "    s)\n" +
            "      SERIAL=\"-s $OPTARG\"\n" +
            "      ;;\n" +
            "    r)\n" +
            "      RUN=$OPTARG\n" +
            "      ;;\n" +
            "    \\?)\n" +
            "      echo \"Invalid option: -$OPTARG\" >&2\n" +
            "      exit 1\n" +
            "      ;;\n" +
            "    :)\n" +
            "      echo \"Option -$OPTARG requires an argument.\" >&2\n" +
            "      exit 1\n" +
            "      ;;\n" +
            "  esac\n" +
            "done\n" +
            "adb ${SERIAL} install -d -r ${APK}\n" +
            "trap '{ adb ${SERIAL} shell am force-stop %s ; exit 1; }' INT\n" % bundle_id +
            "if [ \"$RUN\" = \"true\" ] ; then\n" +
            "  adb ${SERIAL} logcat -c\n" +
            "  adb ${SERIAL} shell am start -n %s/com.unity3d.player.UnityPlayerActivity\n" % bundle_id +
            "  adb ${SERIAL} logcat -v color -s 8thWall,8thWallJava,Unity,libc,DEBUG,MessageQueue,libEGL,native,AndroidRuntime\n" +
            "fi\n"
        )
    if platform == "ios-xcode":
        if ctx.attr.is_test_app == False:
            xcodeproj = "xcode/%s/Unity-iPhone.xcodeproj" % app_name
        else:
            xcodeproj = "xcode/%s/PlayerWithTests/Unity-iPhone.xcodeproj" % app_name
        return (
            "#!/bin/bash\n" +
            "# Generated by unity.bzl\n" +
            "XCODE_BETA_FLAG=\"\"\n" +
            "while getopts \":b\" opt; do\n" +
            "  case $opt in\n" +
            "    b)\n" +
            "      echo \"Opening with XCode-beta!\"\n" +
            "      XCODE_BETA_FLAG=\"-beta\"\n" +
            "      ;;\n" +
            "    \\?)\n" +
            "      echo \"Invalid option: -$OPTARG\" >&2\n" +
            "      exit 1\n" +
            "      ;;\n" +
            "  esac\n" +
            "done\n" +
            "DIR=\"{}\"\n".format(dirname) +
            "# Quit xcode, since it won't reopen the project otherwise.\n" +
            "osascript -e 'quit app \"Xcode\"'\n" +
            "# Incantation to open a symlink xcode project and keep symlink path.\n" +
            "open -a Xcode${XCODE_BETA_FLAG} --args \"${PWD}/${DIR}/%s\"\n" % xcodeproj
        )
    return (
        "#!/bin/bash\n" +
        "DIR=\"{}\"\n".format(dirname) +
        "EXE=${DIR}/%s\n" % exe +
        "exec ${EXE} | sed -e '/Mono/D' -e '/\\[UnityMemory\\] Configuration/D' -e '/memorysetup-/D' \n"
    )

def _display_platform(platform):
    if platform == "ios-xcode":
        return "iOS"
    if platform == "osx":
        return "MacOSX"
    return platform.title()

def _get_exe(platform, app_name, app_output_name):
    if platform == "osx":
        return "%s/Contents/MacOS/%s" % (app_output_name, app_name)
    return app_output_name

def _unity_app_stage1_impl(ctx):
    original_platform = ctx.attr.platform
    platform = ctx.attr.platform
    app_name = ctx.attr.project[0].project_name
    scenes = ctx.attr.scenes

    if platform == "local":
        target_cpu = ctx.var.get("TARGET_CPU")
        platform = _get_local_platform(ctx)
        if not platform:
            fail("Unknown local platform for cpu=%s" % target_cpu)

    provisioning_profile_name = ""
    if ctx.attr.apple_export_options != None:
        provisioning_profile_name = ctx.attr.apple_export_options[ExportOptionsPlistInfo].profilename

    distribution_method = ""
    if ctx.attr.apple_export_options != None:
        distribution_method = ctx.attr.apple_export_options[ExportOptionsPlistInfo].distributionmethod

    # Unity 6 and up requires Gradle 8, and older versions have issues with Gradle 8, so specify the correct one depending on version
    if "6000" in str(ctx.attr.project[0].unity_toolchain.unityinfo.unity_path):
        gradle_path = GRADLE_8_DIR
    else:
        gradle_path = GRADLE_DIR

    if ctx.attr.use_unity_provided_gradle:
        gradle_path = ""

    print("Using Gradle version:[" + gradle_path  +  "] for platform: " + str(ctx.attr.project[0].unity_toolchain.unityinfo.unity_path))

    project_files = []

    # Copy all project files to build directory.
    pathStart = len(ctx.attr.project[0].files_path_prefix) + 1
    for src_in in ctx.attr.project[0].unity_srcs:
        src_out_name = "%s/%s" % (ctx.label.name, src_in.path[pathStart:])
        src_out = ctx.actions.declare_file(src_out_name)
        copy_action(ctx, src_in, src_out)
        project_files.append(src_out)

    outputs = []

    # Create the app_output
    app_output_name = _get_output_name(ctx, platform, app_name)
    app_output = ctx.actions.declare_file(app_output_name)
    outputs.append(app_output)

    ios_project_out = None
    xcodeproj_out = None
    xcode_project_dir = None
    if platform == "ios-xcode":
        # Also add project directory as an output.
        xcode_project_dir = "xcode/" + app_name
        if ctx.attr.is_test_app == False:
            ios_project_out = ctx.actions.declare_file("%s/Unity-iPhone" % xcode_project_dir)
        else:
            ios_project_out = ctx.actions.declare_file("%s/PlayerWithTests/Unity-iPhone" % xcode_project_dir)
        outputs.append(ios_project_out)
        xcodeproj_out = app_output

    files_path_prefix = "%s/%s/%s" % (ctx.configuration.bin_dir.path, ctx.label.package, ctx.label.name)

    relative_project_path = "%s/%s" % (
        files_path_prefix,
        ctx.attr.project[0].project_relative_path,
    )

    full_project_path = "$WORKSPACE/%s" % (
        relative_project_path,
    )

    unity_log_path = "%s/%s/%s" % (
        ctx.bin_dir.path,
        ctx.label.package,
        app_name + ".unity_app_log.txt",
    )

    print("Unity app logfile will be located at : " + unity_log_path)

    platform_args = _get_build_commands(ctx, platform, provisioning_profile_name, distribution_method, gradle_path)

    if ctx.attr.is_test_app == True:
        test_app_options_file_path = "%s/%s/%s" % (
            ctx.label.name,
            ctx.attr.project[0].project_relative_path,
            "Assets/Editor/Generated/TestAppOptions.cs",
        )
        test_app_options = ctx.actions.declare_file(test_app_options_file_path)
        output_path = "../.."
        ctx.actions.expand_template(
            template = ctx.file._test_app_options_template,
            output = test_app_options,
            substitutions = {
                "%OUTPUT_PATH%": output_path,
                "%FILE_NAME%": app_output_name.rsplit("/", 1)[0],
            },
        )
        project_files = project_files + [test_app_options]
    build_args = [
        "-batchmode",
        "-nographics",
        "-upmNoDefaultPackages",
        "-projectPath",
        full_project_path,
        "-logFile",
        unity_log_path,
        "--appname",
        app_name,
    ]
    if ctx.attr.is_test_app == False:
        build_args = build_args + (["-quit"])
    else:
        test_categories = ctx.var.get("test-categories", "")
        build_args = build_args + (["-runTests"])
        if test_categories != "":
            print("Test categories: " + test_categories)
            build_args = build_args + ([
                "-testCategory",
                test_categories,
            ])

    build_args = build_args + (platform_args)

    if ctx.attr.is_hmd_app == True:
        build_args = build_args + (["--buildHMD"])

    if len(scenes) > 0:
        build_args += [
            "--scenes",
            ",".join(scenes),
        ]

    display_platform = _display_platform(platform)

    os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]

    if ctx.target_platform_has_constraint(os_ios):
        if ctx.attr.is_test_app == False:
            display_output_name = "%s/Unity-iPhone.xcodeproj" % app_name
        else:
            display_output_name = "%s/PlayerWithTests/Unity-iPhone.xcodeproj" % app_name
    else:
        display_output_name = app_output_name

    # Remove anything in the build directory that might be orphaned from a previous build.
    clean_up_file = clean_up_old_assets(ctx, files_path_prefix, project_files)

    ctx.actions.run(
        inputs = [clean_up_file] + project_files,
        executable = ctx.executable.unitybuild,
        arguments = build_args,
        outputs = outputs,
        tools = ctx.files._python,
        env = {
            "WORKSPACE_ENV": ctx.file._workspace_env.path,
            "UNITY": ctx.attr.project[0].unity_toolchain.unityinfo.unity_path,
            "PYTHON3": ctx.attr._python[platform_common.ToolchainInfo].py3_runtime.interpreter.path,
        },
        mnemonic = "BuildUnityApp%s" % display_platform,
        progress_message = "Generating %s (%s) with Unity" % (display_output_name, display_platform),
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
    )

    script_name = ctx.label.name
    script_output = ctx.actions.declare_file("run-" + script_name)
    exe = _get_exe(platform, app_name, app_output_name)
    script_content = _get_run_script(ctx, platform, app_name, exe, script_output.dirname)
    ctx.actions.write(
        output = script_output,
        content = script_content,
        is_executable = True,
    )
    output_info = [
        DefaultInfo(
            files = depset(outputs + [script_output]),
            executable = script_output,
        ),
        _UnityStage1Info(
            files = depset(outputs),
            app_name = app_name,
            app_output_name = app_output_name,
            platform = platform,
            app_dir = script_output.dirname,
        ),
    ]

    if ctx.target_platform_has_constraint(os_ios):
        output_info.append(
            XcodeProjectInfo(
                files = depset([ios_project_out]),
                xcodeproj = xcodeproj_out,
                project_dir = xcode_project_dir,
            ),
        )
    return output_info

_unity_app_stage1 = rule(
    implementation = _unity_app_stage1_impl,
    executable = True,
    cfg = unity_target_arch_transition,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "_unity_platform": attr.label(
            default = "@unity-version//:local",
        ),
        "scenes": attr.string_list(
            mandatory = True,
        ),
        "project": attr.label(
            cfg = unity_platform_transition,
            mandatory = True,
            providers = ["unity_srcs", "unity_toolchain"],
        ),
        "_test_app_options_template": attr.label(
            default = Label("//bzl/unity:TestAppOptionsTemplate.cs"),
            allow_single_file = True,
        ),
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
        ),
        "unitybuild": attr.label(
            default = Label("//bzl/unity:unity-build"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "platform": attr.string(
            mandatory = True,
        ),
        "apple_export_options": attr.label(
            allow_single_file = True,
        ),
        "use_unity_provided_gradle": attr.bool(
            default = False,
        ),
        "is_hmd_app": attr.bool(
            default = False,
        ),
        "is_test_app": attr.bool(
            default = False,
        ),
        "test_categories": attr.string(
            default = "",
        ),
        "_python": attr.label(
            default = Label("@rules_python//python:current_py_toolchain"),
            providers = [platform_common.ToolchainInfo],
            cfg = "exec",
        ),
        "_cpu_x86_64": attr.label(
            default = "@platforms//cpu:x86_64",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_osx": attr.label(
            default = "@platforms//os:osx",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_ios": attr.label(
            default = "@platforms//os:ios",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_android": attr.label(
            default = "@platforms//os:android",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_linux": attr.label(
            default = "@platforms//os:linux",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_windows": attr.label(
            default = "@platforms//os:windows",
            providers = [platform_common.ConstraintValueInfo],
        ),
    },
)

def _unity_app_stage2_impl(ctx):
    os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]
    stage1info = ctx.attr.stage1[_UnityStage1Info]
    if ctx.target_platform_has_constraint(os_ios):
        xcodeprojectinfo = ctx.attr.stage1[XcodeProjectInfo]
        return xcode_app_impl(
            ctx = ctx,
            app_name = stage1info.app_name,
            project_info = xcodeprojectinfo,
            device = None,
            apple_export_options = ctx.attr.apple_export_options,
            scheme = "Unity-iPhone",
            xcodebuild = ctx.executable._xcodebuild,
            xcodearchive = ctx.executable._xcodearchive,
            xcodeexport = ctx.executable._xcodeexport,
            xcodebuildsettings = ctx.executable._xcodebuildsettings,
            xcpretty_files = ctx.files._xcpretty,
        )
    else:
        script_name = ctx.label.name
        script_output = ctx.actions.declare_file("run-" + script_name)
        exe = _get_exe(stage1info.platform, stage1info.app_name, stage1info.app_output_name)
        script_content = _get_run_script(ctx, stage1info.platform, stage1info.app_name, exe, stage1info.app_dir)
        ctx.actions.write(
            output = script_output,
            content = script_content,
            is_executable = True,
        )

        return [
            DefaultInfo(
                files = depset([script_output], transitive = [ctx.attr.stage1[_UnityStage1Info].files]),
                executable = script_output,
            ),
        ]

_unity_app_stage2 = rule(
    implementation = _unity_app_stage2_impl,
    executable = True,
    #cfg = unity_target_arch_transition,
    attrs = {
        #"_allowlist_function_transition": attr.label(
        #default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        #),
        "stage1": attr.label(
            mandatory = True,
        ),
        "_os_ios": attr.label(
            default = "@platforms//os:ios",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_osx": attr.label(
            default = "@platforms//os:osx",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_is_ios_simulator": attr.label(
            default = "//bzl/crosstool:ios-simulator",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "apple_export_options": attr.label(
            allow_single_file = True,
        ),
        "_xcodebuild": attr.label(
            default = Label("//bzl/xcode:xcodebuild"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_xcodearchive": attr.label(
            default = Label("//bzl/xcode:xcodearchive"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_xcodeexport": attr.label(
            default = Label("//bzl/xcode:xcodeexport"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_xcodebuildsettings": attr.label(
            default = Label("//bzl/xcode:xcodebuildsettings"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_xcode_app_developer_dir": attr.label(
            default = "//bzl/xcode:xcode-app-developer-dir",
            providers = [BuildSettingInfo],
            cfg = "exec",
        ),
        "_xcpretty": attr.label(
            default = Label("@xcpretty//:xcpretty"),
            cfg = "host",
        ),
    },
    fragments = ["apple"],
)

def unity_app(name, project, apple_export_options = None, tags = [], tags_only_stage2 = [], scenes = [], **kwargs):
    """Bazel rule to build a standalone Unity app for the target configuration.

    Args:
        name (string): Target name.
        project (Label): A 'unity_project' rule that this app will build.
        apple_export_options (Label): An 'exportOptionsPlist' rule that specifies the export options for the app.
        tags (List[string]): List of tags to apply to the target.
        tags_only_stage2 (List[string]): List of tags to apply to the stage2 target.
        scenes (List[string]): List of scenes to include in the build.
        **kwargs: Additional arguments to pass to the rule
    """

    if name.endswith(".app"):
        fail("Remove '.app' suffix from //%s:%s" % (native.package_name(), name), "name")

    if name.endswith(".ipa"):
        fail("Remove '.ipa' suffix from //%s:%s" % (native.package_name(), name), "name")

    _unity_app_stage1(
        name = name + "-stage1",
        scenes = scenes,
        project = project,
        platform = "local",
        tags = tags,
        apple_export_options = apple_export_options,
        **kwargs
    )

    _unity_app_stage2(
        name = name,
        stage1 = name + "-stage1",
        tags = tags + tags_only_stage2,
        apple_export_options = apple_export_options,
    )
