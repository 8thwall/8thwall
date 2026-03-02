load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//bzl/xcode:exportOptionsPList.bzl", "ExportOptionsPlistInfo", "get_bundle_id")
load("//bzl/xcode/impl:xcode_project.bzl", "XcodeProjectInfo")

# When an Xcode DEVELOPER_DIR is specified, include with env variables
def _developer_dir_wrapper(env_dict, d_dir):
    if d_dir:
        env_dict.update({"DEVELOPER_DIR": d_dir})
    return env_dict

# Shared by _xcode_app_impl and //bzl/unity/impl/unity-app.bzl.
def xcode_app_impl(ctx, app_name, project_info, device, apple_export_options, scheme, xcodebuild, xcodearchive, xcodeexport, xcodebuildsettings, xcpretty_files):
    if not app_name:
        app_name = ctx.label.name.title().replace("-", "").replace("_", "")

    # Specific Xcode DEVELOPER_DIR used to build apps (optional)
    developer_dir = ctx.attr._xcode_app_developer_dir[BuildSettingInfo].value

    bundle_id = get_bundle_id(app_name)
    outputs = []
    exe = app_name

    if not scheme:
        # Set scheme from app_name if not specified.
        scheme = app_name

    if not device:
        device = "iPhone 12"

    cpu = ctx.var.get("TARGET_CPU")

    target_configuration = {
        "opt": "Release",
        "fastbuild": "Release",
    }.get(ctx.var.get("COMPILATION_MODE"), "Debug")

    is_ios_simulator = ctx.attr._is_ios_simulator[platform_common.ConstraintValueInfo]
    is_os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]
    is_os_osx = ctx.attr._os_osx[platform_common.ConstraintValueInfo]

    target_archs = ["arm64"]

    if ctx.target_platform_has_constraint(is_ios_simulator):
        target_platform = "iphonesimulator"
        destination = "generic/platform=iOS Simulator"
    elif ctx.target_platform_has_constraint(is_os_ios):
        target_platform = "iphoneos"
        destination = "generic/platform=iOS"
    elif ctx.target_platform_has_constraint(is_os_osx):
        target_platform = "macosx"
        destination = "platform=macOS,arch=%s" % target_archs[0]
    else:
        target_platform = None
        destination = None
        fail("%s is an unsupported target_cpu" % cpu)

    if not target_platform:
        fail("%s is an unsupported target_cpu" % cpu)

    build_dir = "Products/%s-%s" % (target_configuration, target_platform)

    # Determine the the output binary filename
    app_name_dot_ext = app_name
    should_expect_ipa = apple_export_options != None
    if should_expect_ipa:
        # Build a .ipa binary if the is for ios and if the export options plist has a non-empty profile name
        app_name_dot_ext += ".ipa"
    else:
        # Else build a .app binary
        app_name_dot_ext += ".app"

    # Determine the full path of the output binary
    app_build_path = project_info.project_dir + "/" + build_dir
    app_path = app_build_path + "/" + app_name_dot_ext
    app_output = ctx.actions.declare_file(app_path)

    args = [
        "-configuration",
        target_configuration,
        "-destination",
        destination,
        "-sdk",
        target_platform,
        "ARCHS=%s" % " ".join(target_archs),
        "-project",
        project_info.xcodeproj.path,
        "-scheme",
        scheme,
    ]

    # Write out the XCode project build settings to file.
    settings_basename = ctx.label.name + ".settings"
    build_settings_out = ctx.actions.declare_file(settings_basename)
    ctx.actions.run(
        inputs = project_info.files,
        executable = xcodebuildsettings,
        env = _developer_dir_wrapper(
            {
                "XCODE_SRCROOT": project_info.xcodeproj.dirname,
                "PRODUCT_NAME": app_name,
                "APP_OUTPUT": app_output.path,
            },
            developer_dir,
        ),
        arguments = [
            # The first argument to this script is the output.
            build_settings_out.path,
            "-project",
            project_info.xcodeproj.path,
            "-configuration",
            target_configuration,
            "-destination",
            destination,
            "-sdk",
            target_platform,
            "ARCHS=%s" % " ".join(target_archs),
            # Currently not using schemes, and a result, it is unclear how to
            # specify the derived data path.
            #"-scheme",
            #exe,
            #"-derivedDataPath",
            #derived_data,
        ],
        outputs = [build_settings_out],
        mnemonic = "ParseProjectBuildSettings",
        progress_message = "Parsing Project Build Settings",
        execution_requirements = {"local": "1"},
    )

    xcode_input_files = project_info.files.to_list() + xcpretty_files

    if should_expect_ipa:
        # Archive the .xcarchive from the xcode project
        xcode_archive_basename = app_build_path + "/" + app_name + ".xcarchive"
        xcode_archive_out = ctx.actions.declare_file(xcode_archive_basename)
        ctx.actions.run(
            inputs = xcode_input_files,
            executable = xcodearchive,
            arguments = args,
            outputs = [xcode_archive_out],
            env = _developer_dir_wrapper(
                {
                    "XCPRETTY": "external/xcpretty/bin/xcpretty",
                    "XCARCHIVE_PATH": xcode_archive_out.path,
                },
                developer_dir,
            ),
            mnemonic = "ArchiveXCodeProject",
            progress_message = "Archiving xcode project with XCode",
            execution_requirements = {"local": "1"},
        )

        # Export the .ipa from the xcode archive
        apple_export_options_plist = apple_export_options[ExportOptionsPlistInfo].plist
        apple_export_options_plist_path = apple_export_options_plist.path
        xcode_input_files.append(apple_export_options_plist)
        ctx.actions.run(
            inputs = xcode_input_files + [xcode_archive_out],
            executable = xcodeexport,
            arguments = args,
            outputs = [app_output],
            env = _developer_dir_wrapper(
                {
                    "XCPRETTY": "external/xcpretty/bin/xcpretty",
                    "XCARCHIVE_PATH": xcode_archive_out.path,
                    "EXPORT_OPTIONS_PLIST_PATH": apple_export_options_plist_path,
                    "APP_OUTPUT_DIR": app_output.dirname,
                },
                developer_dir,
            ),
            mnemonic = "ExportXCodeArchive",
            progress_message = "Exporting xcode archive with XCode",
            execution_requirements = {"local": "1"},
        )
    else:
        # Build the .app from the xcode project
        ctx.actions.run(
            inputs = xcode_input_files,
            executable = xcodebuild,
            arguments = args,
            outputs = [app_output],
            env = _developer_dir_wrapper(
                {
                    "XCPRETTY": "external/xcpretty/bin/xcpretty",
                    "XCODE_SRCROOT": project_info.xcodeproj.dirname,
                    "PRODUCT_NAME": app_name,
                    "APP_OUTPUT": app_output.path,
                },
                developer_dir,
            ),
            mnemonic = "BuildXCodeApp",
            progress_message = "Building %s with XCode" % app_name_dot_ext,
            execution_requirements = {"local": "1"},
        )

    simulator = target_platform.endswith("simulator")
    if simulator:
        run_script = (
            "#!/bin/bash\n" + "DIR=`dirname ${BASH_SOURCE[0]}`\n" + "APP=${DIR}/%s\n" %
                                                                    app_path + "SETTINGS=${DIR}/%s\n" % settings_basename +
            (
                "BUNDLEID=%s\n" % bundle_id if bundle_id else 'BUNDLEID=`grep "PRODUCT_BUNDLE_IDENTIFIER =" ${SETTINGS} | sed \'s/.*= //\'`\n'
            ) +
            'SDK_VERSION=`grep "SDK_VERSION =" ${SETTINGS} | sed \'s/.*= //\'`\n' +
            'DEVICE="%s"\n' % device +
            "DEV_ID_REGEX=\"${DEVICE} (${SDK_VERSION}) \\[\\([A-F0-9\\-]*\\)\\] (Simulator)\"\n" +
            'DEV_ID=`instruments -s devices | grep "${DEV_ID_REGEX}" | sed "s/${DEV_ID_REGEX}/\\1/"`\n' +
            "xcrun instruments -w $DEV_ID 2>/dev/null\n" +
            'xcrun simctl install $DEV_ID "${APP}"\n' +
            "trap '{ xcrun simctl terminate $DEV_ID ${BUNDLEID} ; exit 1; }' INT\n" +
            "xcrun simctl launch --console $DEV_ID $BUNDLEID\n"
        )
    else:
        run_script = (
            "#!/bin/bash\n\n" +
            "DIR=`dirname ${BASH_SOURCE[0]}`\n" +
            "APP=${DIR}/%s\n" % app_path +
            'RUN="true"\n' +
            'RUN_MODE_FLAG=""\n' +
            'while getopts \":r:\" opt; do\n' +
            "  case $opt in\n" +
            "    r)\n" +
            "      RUN=$OPTARG\n" +
            "      ;;\n" +
            "    \\?)\n" +
            '      echo \"Invalid option: -$OPTARG\" >&2\n' +
            "      exit 1\n" +
            "      ;;\n" +
            "    :)\n" +
            '      echo \"Option -$OPTARG requires an argument.\" >&2\n' +
            "      exit 1\n" +
            "      ;;\n" +
            "  esac\n" +
            "done\n" +
            'if [ "$RUN" = "true" ] ; then\n' +
            '  RUN_MODE_FLAG="--noninteractive &"\n' +
            "fi\n" +
            "# Run ios-deploy, setting up non-interactive LLDB debugging. Since LLDB traps\n" +
            "# signals, this is run in the background and the script will wait for it to\n" +
            "# complete or get interrupted.\n" +
            "ios-deploy --bundle ${APP} ${RUN_MODE_FLAG}\n\n" +
            'if [ "$RUN" = "true" ] ; then\n' +
            "  # Get the PID from the original ios-deploy call.\n" +
            "  OUTER_PID=$!\n\n" +
            "  # Trap Ctrl-C and kill the forked child process of ios-deploy.\n" +
            "  trap '{ INNER_PID=`pgrep -P ${OUTER_PID}`; LLDB_PID=`pgrep -P ${INNER_PID}`; kill ${LLDB_PID}; echo; }' INT\n\n" +
            "  # Wait for the process to complete or be interrupted.\n" +
            "  wait\n" +
            "fi\n"
        )

    # Create wrapper script to open XCode app.
    run_script_name = "run-%s" % ctx.label.name
    if run_script_name.endswith(".ios"):
        # For consistency, replace .ios suffix with -ios.
        run_script_name = run_script_name[0:-4] + "-ios"
    run_script_out = ctx.actions.declare_file(run_script_name)
    ctx.actions.write(
        output = run_script_out,
        content = run_script,
        is_executable = True,
    )
    return [
        DefaultInfo(
            files = depset([
                app_output,
                run_script_out,
            ]),
            executable = run_script_out,
        ),
        OutputGroupInfo(
            build_settings = depset([build_settings_out]),
        ),
    ]

def _xcode_app_impl(ctx):
    return xcode_app_impl(
        ctx = ctx,
        app_name = ctx.attr.app_name,
        project_info = ctx.attr.project[XcodeProjectInfo],
        device = ctx.attr.device,
        apple_export_options = ctx.attr.apple_export_options,
        scheme = ctx.attr.scheme,
        xcodebuild = ctx.executable._xcodebuild,
        xcodearchive = ctx.executable._xcodearchive,
        xcodeexport = ctx.executable._xcodeexport,
        xcodebuildsettings = ctx.executable._xcodebuildsettings,
        xcpretty_files = ctx.files._xcpretty,
    )

# Bazel rule to build a standalone App app for the target configuration.
#
# Args:
#   name (string): Target name.
#   project (Label): A 'xcode_project' rule that this app will build.
#   device(string): A valid iOS device name. Defaults to 'iPhone 12'.
#   app(string): A pretty name for the app output. Defaults to CamelCase target name.
#   bundle_id(string): (optional) Use this bundle id instead of reading id from project.
xcode_app = rule(
    implementation = _xcode_app_impl,
    executable = True,
    attrs = {
        "app_name": attr.string(),
        "project": attr.label(
            mandatory = True,
            providers = [XcodeProjectInfo],
        ),
        "device": attr.string(
            default = "iPhone 12",
        ),
        "bundle_id": attr.string(),
        "apple_export_options": attr.label(
            allow_single_file = True,
        ),
        "scheme": attr.string(
            default = "",
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
        "_xcodebuild": attr.label(
            default = Label("//bzl/xcode:xcodebuild"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_xcodearchive": attr.label(
            default = Label("//bzl/xcode:xcodearchive"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_xcodeexport": attr.label(
            default = Label("//bzl/xcode:xcodeexport"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_xcodebuildsettings": attr.label(
            default = Label("//bzl/xcode:xcodebuildsettings"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_xcode_app_developer_dir": attr.label(
            default = "//bzl/xcode:xcode-app-developer-dir",
            providers = [BuildSettingInfo],
            cfg = "exec",
        ),
        "_xcpretty": attr.label(
            default = Label("@xcpretty//:xcpretty"),
            cfg = "exec",
        ),
    },
    fragments = ["apple", "objc"],
)
