"""
Bazel modules for configuration of unity projects
"""

load("//bzl/unity/impl:transitions.bzl", "unity_platform_transition")
load("//bzl/unity/impl:unity-plugin.bzl", "copy_action", "generate_missing_meta_files", "metafy", "symlink_action", "validate_meta_files")
load("//bzl/unity/impl:unity-upm-package.bzl", "TarballInfo")
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

EDITOR_GENERATED_DIR = "Assets/Editor/Generated"

UnityProjectInfo = provider(
    "Info needed to depends on Unity Project",
    fields = {
        "files": "depset of all files",
        "unity_srcs": "outputs and cleanup files",
        "files_path_prefix": "all files path prefix",
        "project_name": "string project name",
        "project_relative_path": "relativate to this project",
        "unity_toolchain": "actual unity toolchain used",
    },
)

def clean_up_old_assets(ctx, files_path_prefix, outputs, directory_to_ignore = None):
    """_summary_

    Args:
        ctx (context): Rule context
        files_path_prefix (string): File path Prefix
        outputs (File): output cleaned files

    Returns:
        _type_: _description_
    """
    actual_assets_file = ctx.actions.declare_file(ctx.label.name + ".actual-assets")
    expected_assets_file = ctx.actions.declare_file(ctx.label.name + ".expected-assets")
    clean_up_files = ctx.actions.declare_file(ctx.label.name + ".cleanup")
    expected_outputs = dict([(x.path, None) for x in outputs])
    for oneOut in outputs:
        path_parts = oneOut.dirname.partition(files_path_prefix)
        if path_parts[1]:
            base_path = "".join(path_parts[0:2])
            dir_parts = path_parts[2].split("/")
            added_part = ""
            for part in dir_parts:
                if part:
                    added_part += "/" + part
                expected_path = base_path + added_part
                expected_outputs[expected_path] = None
                expected_outputs[metafy(expected_path)] = None

    # List the asset files and directories that were expected.
    ctx.actions.write(
        output = expected_assets_file,
        content = "\n".join(sorted(expected_outputs.keys())) + "\n",
    )

    actual_files_find_command = "find %s" % (files_path_prefix)
    if directory_to_ignore:
        actual_files_find_command += " ! -path '*/%s*'" % (directory_to_ignore)
    actual_files_find_command += " | sort > %s" % (actual_assets_file.path)

    # List the asset files and directories that were found ignoring the Unity Library directory to prevent it
    # from getting deleted as this would cause currently open unity instances to require restart.
    ctx.actions.run_shell(
        inputs = outputs,
        outputs = [actual_assets_file],
        command = actual_files_find_command,
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
    )

    # Remove any unexpected files or directories.
    ctx.actions.run_shell(
        inputs = [expected_assets_file, actual_assets_file],
        outputs = [clean_up_files],
        command = "comm -23 '%s' '%s' | tee '%s' | sed 's/.*/\"&\"/' | while read line; do rm -rfv $line; done" % (actual_assets_file.path, expected_assets_file.path, clean_up_files.path),
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
            "no-sandbox": "1",
        },
    )
    return clean_up_files

def _project_impl(ctx):
    # Validate that each unity src file and directory has a .meta file pairing
    if ctx.files.meta_verification_srcs:
        validate_meta_files(ctx, ctx.files.meta_verification_srcs)

    # List of the default outputs to manually export for the rule.
    outputs = []

    # Dictionary of all file paths in the project, including generated sources
    # used to ensure there are no duplicates between plugin and project files being
    # added to the unity project where project files take priority over plugin files
    outputs_paths = {}

    # List of all ios framework keys for ios plugins
    ios_frameworks_keys = []

    # All the debugging symbol files coming from the non upm plugins. We append this to the files output to
    # have the debugging symbol files outputted by the target, but to not be directly in the outputs list
    # as the debugging symbols dont have anything to do with unity project sources
    debugging_symbols = []

    # Copy all of the plugin files.
    for plugin in ctx.attr.plugins:
        ios_frameworks_keys += plugin.ios_frameworks_keys
        path_start = len(plugin.files_path_prefix) + 1
        for src in plugin.outputs:
            asset_path = "%s/%s/%s/%s" % (ctx.label.name, ctx.attr.project_relative_path, plugin.project_relative_dir, src.path[path_start:])
            if not asset_path in outputs_paths.keys():
                outputs_paths[asset_path] = None
                out = ctx.actions.declare_file(asset_path)
                outputs.append(out)
                symlink_action(ctx, src, out)

        # get the debugging symbol files if they exist
        for platform, files in plugin.debugging_symbol_files.items():
            for file in files.to_list():
                debugging_symbol_path = "debugging_symbols/%s/%s/%s/%s" % (ctx.label.name, plugin.label.name, platform, file.basename)
                if not debugging_symbol_path in outputs_paths.keys():
                    outputs_paths[debugging_symbol_path] = None
                    debugging_symbol_out = ctx.actions.declare_file(debugging_symbol_path)
                    symlink_action(ctx, file, debugging_symbol_out)
                    debugging_symbols.append(debugging_symbol_out)

    # Copy all of the external source files.
    for external_src_label, destination_directory in ctx.attr.external_srcs.items():
        for data_src in external_src_label.files.to_list():
            asset_path = "%s/%s/%s" % (ctx.label.name, destination_directory, data_src.basename)
            if not asset_path in outputs_paths.keys():
                outputs_paths[asset_path] = None
                data_out = ctx.actions.declare_file(asset_path)
                outputs.append(data_out)
                copy_action(ctx, data_src, data_out)

    ios_sdk_platform = ctx.fragments.apple.single_arch_platform.name_in_plist

    bitcode_setting = "NO" if str(ctx.fragments.apple.bitcode_mode) == "none" else "YES"

    ios_platform_type = ctx.fragments.apple.single_arch_platform.platform_type
    minimum_os_version = ctx.attr._ios_min_version[BuildSettingInfo].value
    if ios_platform_type == apple_common.platform_type.macos:
        minimum_os_version = ctx.attr._osx_min_version[BuildSettingInfo].value

    # Write out a PostProcessBuild script with added iOS SDK Frameworks.
    script_class = "BuildPostProcessor"

    post_process_build_path = "%s/%s/%s/%s.cs" % (
        ctx.label.name,
        ctx.attr.project_relative_path,
        EDITOR_GENERATED_DIR,
        script_class,
    )

    if not post_process_build_path in outputs_paths.keys():
        outputs_paths[post_process_build_path] = None
        post_process_build_out = ctx.actions.declare_file(post_process_build_path)

        ctx.actions.write(
            output = post_process_build_out,
            content = (
                "using System;\n" +
                "using System.Reflection;\n" +
                "using UnityEngine;\n" +
                "using UnityEditor;\n" +
                "using UnityEditor.Callbacks;\n" +
                "\n" +
                "public class %s {\n" % script_class +
                "  [PostProcessBuild]\n" +
                "  public static void XcodeProjectSettings(BuildTarget buildTarget, string pathToBuiltProject) {\n" +
                "    if (buildTarget == BuildTarget.iOS) {\n" +
                "      Type pbxProjectType = Type.GetType(\"UnityEditor.iOS.Xcode.PBXProject, UnityEditor.iOS.Extensions.Xcode, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null\");\n" +
                "      var proj = Activator.CreateInstance(pbxProjectType);\n" +
                "      MethodInfo readFromFileMethod = pbxProjectType.GetMethod(\"ReadFromFile\");\n" +
                "      string unityTarget = null;\n" +
                "      MethodInfo unityMainTargetGuidMethod = pbxProjectType.GetMethod(\"GetUnityMainTargetGuid\");\n" +
                "      MethodInfo unityFrameworkTargetGuidMethod = pbxProjectType.GetMethod(\"GetUnityFrameworkTargetGuid\");\n" +
                "      MethodInfo addFrameworkToProjectMethod = pbxProjectType.GetMethod(\"AddFrameworkToProject\");\n" +
                "      MethodInfo writeToFileMethod = pbxProjectType.GetMethod(\"WriteToFile\");\n" +
                "      string projPath = pathToBuiltProject + \"/Unity-iPhone.xcodeproj/project.pbxproj\";\n" +
                "      readFromFileMethod.Invoke(proj, new[] {projPath});\n" +
                "      // Unity 2019.3+ uses GetUnityMainTargetGuid()/GetUnityFrameworkTargetGuid() instead of TargetGuidByName()\n" +
                "      if (unityMainTargetGuidMethod != null && unityFrameworkTargetGuidMethod != null) {\n" +
                "        unityTarget = (string)unityFrameworkTargetGuidMethod.Invoke(proj, null);\n" +
                "      } else {\n" +
                "        unityMainTargetGuidMethod = pbxProjectType.GetMethod(\"TargetGuidByName\");\n" +
                "        unityTarget = (string)unityMainTargetGuidMethod.Invoke(proj, new[] {\"Unity-iPhone\"});\n" +
                "      }\n" +
                "      " + "\n      ".join(ios_frameworks_keys) + "\n" +
                "      writeToFileMethod.Invoke(proj, new[] {projPath});\n" +
                "    }\n" +
                "  }\n" +
                "}\n"
            ),
        )
        outputs.append(post_process_build_out)

    # Copy the Editor scripts, required for build commands.
    for editor_script in ctx.files.editor_scripts:
        editor_script_path = "%s/%s/%s/%s" % (
            ctx.label.name,
            ctx.attr.project_relative_path,
            EDITOR_GENERATED_DIR,
            editor_script.basename,
        )

        if not editor_script_path in outputs_paths.keys():
            outputs_paths[editor_script_path] = None
            editor_script_out = ctx.actions.declare_file(editor_script_path)
            ctx.actions.expand_template(
                template = editor_script,
                output = editor_script_out,
                substitutions = {
                    "{osxOutput}": '"../../%s.app"' % ctx.attr.project_name,
                    "{androidOutput}": '"../../%s.apk"' % ctx.attr.project_name,
                    "{xcodeOutputDir}": '"../../%s%s"' % (ctx.attr.xcode_prefix, ctx.attr.project_name),
                    "{iosSdkPlatform}": '"%s"' % ios_sdk_platform,
                    "{iosMinimumVersion}": '"%s"' % minimum_os_version,
                    "{bitcodeSetting}": '"%s"' % bitcode_setting,
                },
            )
            outputs.append(editor_script_out)

    files_path_prefix = "%s/%s/%s" % (ctx.configuration.bin_dir.path, ctx.label.package, ctx.label.name)

    # Copy the project files.
    path_start = len("%s/%s" % (ctx.label.package, ctx.attr.project_relative_path)) + 1
    for src in ctx.files.srcs:
        if not EDITOR_GENERATED_DIR in src.path[path_start:]:
            out_path = "%s/%s/%s" % (ctx.label.name, ctx.attr.project_relative_path, src.path[path_start:])
            if not out_path in outputs_paths.keys():  # and not src.is_directory: #  and not out_path.endswith("StreamingAssets"):
                outputs_paths[out_path] = None
                out = ctx.actions.declare_file(out_path)
                symlink_action(ctx, src, out)
                outputs.append(out)
            else:
                # buildifier: disable=print
                print("Ignoring generated file in source tree: " + src.path)
        else:
            # buildifier: disable=print
            print("Ignoring generated file in source tree: " + src.path)

    if ctx.attr.manifest:
        manifest_out = "%s/%s/Packages/manifest.json" % (ctx.label.name, ctx.attr.project_relative_path)
        if not manifest_out in outputs_paths.keys():
            outputs_paths[manifest_out] = None
            out = ctx.actions.declare_file(manifest_out)
            symlink_action(ctx, ctx.file.manifest, out)
            outputs.append(out)

    # Determine all the missing .meta files
    generate_missing_meta_files(ctx, files_path_prefix, outputs, outputs_paths)

    unity = ctx.toolchains["@unity-version//:toolchain_type"]
    executable = ctx.actions.declare_file("run-" + ctx.label.name)

    # Create wrapper script to open unity editor
    ctx.actions.write(
        output = executable,
        content = (
            "#!/bin/bash\n" +
            "DIR=`dirname ${BASH_SOURCE[0]}`\n" +
            "UNITY_APP=\"{unity}\"\n".format(unity = unity.unityinfo.unity_path) +
            "${UNITY_APP} -projectPath \"${PWD}/${DIR}/%s/%s\" &\n" % (
                ctx.label.name,
                ctx.attr.project_relative_path,
            )
        ),
        is_executable = True,
    )

    # Copy in upm packages and their debugging_symbols (if present)
    for package in ctx.attr.upm_packages:
        for file in package.files.to_list():
            outputs.append(file)
        for debugging_symbol in package[TarballInfo].debugging_symbols.to_list():
            debugging_symbols.append(debugging_symbol)

    # Remove anything in the Assets directory that might be orphaned from a previous build.
    clean_up_files = clean_up_old_assets(ctx, files_path_prefix, outputs, "Library")

    return struct(
        files = depset(outputs + [executable, clean_up_files] + debugging_symbols),
        unity_srcs = outputs + [clean_up_files],
        files_path_prefix = files_path_prefix,
        project_name = ctx.attr.project_name,
        project_relative_path = ctx.attr.project_relative_path,
        unity_toolchain = unity,
        # We need this for any integration tests that rely on this project.
        # The string slice is to remove the "/BUILD" from the end of it as
        # we just need the path up to that point
        build_file_path = ctx.build_file_path[:-6],
    )

_unity_project = rule(
    implementation = _project_impl,
    cfg = unity_platform_transition,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "_unity_platform": attr.label(
            default = "@unity-version//:local",
        ),
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
            cfg = "exec",
        ),
        "_python": attr.label(
            default = Label("@rules_python//python:current_py_toolchain"),
            providers = [platform_common.ToolchainInfo],
            cfg = "exec",
        ),
        # We use the value of one of these, depending on whether we're building for iOS or OSX.
        "_ios_min_version": attr.label(
            default = Label("//bzl/xcode:ios-min-version"),
            providers = [BuildSettingInfo]
        ),
        "_osx_min_version": attr.label(
            default = Label("//bzl/xcode:osx-min-version"),
            providers = [BuildSettingInfo]
        ),
        "meta_verification_srcs": attr.label_list(
            allow_files = True,
        ),
        "project_name": attr.string(
            mandatory = True,
        ),
        "project_relative_path": attr.string(
            mandatory = True,
        ),
        "plugins": attr.label_list(
            default = [],
        ),
        "xcode_prefix": attr.string(
            # Prefix for output xcode projects.
            mandatory = True,
        ),
        "srcs": attr.label_list(
            allow_files = True,
        ),
        "external_srcs": attr.label_keyed_string_dict(
            default = {},
        ),
        "editor_scripts": attr.label_list(
            default = [Label("//bzl/unity:editor-scripts")],
        ),
        "upm_packages": attr.label_list(
            default = [],
        ),
        "manifest": attr.label(
            allow_single_file = True,
        ),
    },
    fragments = ["apple"],
    toolchains = [
        "@unity-version//:toolchain_type",
    ],
)

def unity_project(
        name,
        project_name,
        project_relative_path,
        meta_verification_srcs = None,
        upm_packages = None,
        manifest = None,
        external_srcs = {},
        project_additional_includes = [],
        project_additional_excludes = [],
        plugins = [],
        **kwargs):
    """Bazel rule to build a Unity project by symlinking files from the source repository.

    Args:
        name (string): Build rule target name.
        project_name (string): Pretty name for the Unity project, e.g., MyUnityProject.
        project_relative_path (string): Relative path to the Unity project directory.
        meta_verification_srcs (list): File list to be used for meta file validation
        upm_packages (_type_, optional): Unity UPM package targets to include in the project.
        manifest (_type_, optional): Unity manifest file to overwrite in the project.
        external_srcs (dictionary): Additional sources to include in the unity project.
        project_additional_includes (list): Source paths to include when bazel globs sources
        project_additional_excludes (list): Source paths to exclude when bazel globs sources
        plugins (List[Label]): List of unity_plugin targets to include in the project.
    """

    additional_includes = []
    for project_additional_include in project_additional_includes:
        additional_includes.append("%s/%s" % (project_relative_path, project_additional_include))

    additional_excludes = []
    for project_additional_exclude in project_additional_excludes:
        additional_excludes.append("%s/%s" % (project_relative_path, project_additional_exclude))

    # To have the option to include a specific manifest.json, you must not include the packages
    # dir when one is passed. If it is included, bazel will complain that the packages dir
    # is a prefix of the file path to the manifest file.
    if not manifest:
        additional_includes.append(project_relative_path + "/Packages")

    for external_src_dir in external_srcs.values():
        if project_relative_path in external_src_dir:
            project_relative_external_src_dir = external_src_dir.split(project_relative_path + "/", 1)[1]
            running_subdirectory = ""
            for project_relative_external_src_dir_split in project_relative_external_src_dir.split("/"):
                if running_subdirectory == "":
                    running_subdirectory = project_relative_external_src_dir_split
                else:
                    running_subdirectory = "%s/%s" % (running_subdirectory, project_relative_external_src_dir_split)
                additional_includes.append("%s/%s/*" % (project_relative_path, running_subdirectory))
                additional_excludes.append("%s/%s" % (project_relative_path, running_subdirectory))

    srcs = native.glob(
        include = [
            project_relative_path + "/ProjectSettings",
            project_relative_path + "/Assets/*",
            project_relative_path + "/Assets/Editor/*",
            project_relative_path + "/Assets/Plugins/*",
        ] + additional_includes,
        exclude = [
            project_relative_path + "/Assets",
            project_relative_path + "/Assets/Editor",
            project_relative_path + "/Assets/Plugins",
        ] + additional_excludes,
        exclude_directories = 0,
    )

    # set meta verification sources to default if request via empty array
    if meta_verification_srcs == [] and project_relative_path != None:
        meta_verification_srcs = native.glob(
            include = [project_relative_path + "/Assets/**"],
            exclude = [
                project_relative_path + "/Assets",
                project_relative_path + "/Assets/**/.*/**",
            ],
            exclude_directories = 0,
        )

    _unity_project(
        name = name,
        project_name = project_name,
        project_relative_path = project_relative_path,
        srcs = srcs,
        meta_verification_srcs = meta_verification_srcs,
        external_srcs = external_srcs,
        plugins = plugins,
        xcode_prefix = "xcode/",
        upm_packages = upm_packages,
        manifest = manifest,
        **kwargs
    )
