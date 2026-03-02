load("//bzl/unity/impl:transitions.bzl", "unity_platform_transition")
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

def _copy_action(ctx, src, dst):
    """Action to copy a file from src to dst."""
    ctx.actions.run_shell(
        inputs = [src],
        outputs = [dst],
        command = "cp \"%s\" \"%s\"" % (
            src.path,
            dst.path,
        ),
        execution_requirements = {
            "no-cache": "1",
            "local": "1",
        },
        mnemonic = "RunShellCopyAction",
        progress_message = "unity-package._copy_action: Copying %s to %s" % (src.path, dst.path),
    )

def _copy_sign(ctx, unsigned_bundle_file, signed_bundle_file):
    """Copy and sign a bundle

    Args:
        ctx (_type_): _description_
        unsigned_bundle_file (_type_): _description_
        signed_bundle_file (_type_): _description_
    """
    ctx.actions.run(
        inputs = [unsigned_bundle_file],
        outputs = [signed_bundle_file],
        executable = ctx.executable._copyandsign,
        arguments = [unsigned_bundle_file.path, signed_bundle_file.path],
        execution_requirements = {
            "no-cache": "1",
            "local": "1",
        },
    )

def _basename_from_string(ctx, string_path):
    # TODO: bazel does not expose any means in ctx for getting the path separator and
    #       even bazel_skylib only support unix (https://github.com/bazelbuild/bazel-skylib/blob/main/docs/paths_doc.md).
    #       So for now we support only unix hosts.
    os_sep = "/"
    return string_path.split(os_sep)[-1]

def _rename_to_signed_bundle(ctx, unsigned_bundle_file):
    # TODO: bazel does not expose any means in ctx for getting the path separator and
    #       even bazel_skylib only support unix (https://github.com/bazelbuild/bazel-skylib/blob/main/docs/paths_doc.md).
    #       So for now we support only unix hosts.
    os_sep = "/"
    filename_to_rename = unsigned_bundle_file.split(os_sep)[-1]
    new_filename = "".join(filename_to_rename.rsplit(".unsigned.bundle") + [".bundle"])
    return os_sep.join(unsigned_bundle_file.split(os_sep)[:-1] + [new_filename])

def _clean_up_old_assets(ctx, projectPrefix, outputs):
    actualAssetsFile = ctx.actions.declare_file(projectPrefix + ".actual-assets")
    expectedAssetsFile = ctx.actions.declare_file(projectPrefix + ".expected-assets")
    cleanUpFile = ctx.actions.declare_file(projectPrefix + ".cleanup")
    assetsDir = "%s/%s/%s/Assets" % (
        ctx.configuration.bin_dir.path,
        ctx.label.package,
        projectPrefix,
    )

    expectedOutputs = dict([(x.path, None) for x in outputs])
    for oneOut in outputs:
        pathParts = oneOut.dirname.partition("/Assets")
        if pathParts[1]:
            basePath = "".join(pathParts[0:2])
            dirParts = pathParts[2].split("/")
            addedPart = ""
            for part in dirParts:
                if part:
                    addedPart += "/" + part
                expectedPath = basePath + addedPart
                expectedOutputs[expectedPath] = None
                expectedOutputs[expectedPath + ".meta"] = None

    # List the asset files and directories that were expected.
    ctx.actions.write(
        output = expectedAssetsFile,
        content = "\n".join(sorted(expectedOutputs.keys())) + "\n",
    )

    # List the asset files and directories that were found.
    ctx.actions.run_shell(
        inputs = outputs,
        outputs = [actualAssetsFile],
        command = "find %s | sort > %s" % (assetsDir, actualAssetsFile.path),
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
    )

    # Remove any unexpected files or directories.
    ctx.actions.run_shell(
        inputs = [expectedAssetsFile, actualAssetsFile],
        outputs = [cleanUpFile],
        command = "comm -23 '%s' '%s' | tee '%s' | sed 's/.*/\"&\"/' | while read line; do rm -rfv $line; done" % (actualAssetsFile.path, expectedAssetsFile.path, cleanUpFile.path),
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
            "no-sandbox": "1",
        },
    )
    return cleanUpFile

def _package_impl(ctx):
    # List of the default outputs to manually export for the rule.
    outputs = []

    target_deps_files = []
    for x in ctx.attr.target_deps:
        for y in x.files.to_list():
            target_deps_files.append(y)

    completion_token = ctx.actions.declare_file("{}.unity_package.completed".format(ctx.attr.name))

    ctx.actions.run_shell(
        inputs = target_deps_files,
        outputs = [completion_token],
        command = (
            "echo '>>> Unity plugin {name} completed' > {ct}\n".format(name = ctx.attr.name, ct = completion_token.path) +
            "\n".join(["echo {ctdep} >> {ct}; echo '^^^ Dependency file' >> {ct}\n ".format(ctdep = xd.path, ct = completion_token.path) for xd in target_deps_files])
        ) + " chmod +w {ct}".format(ct = completion_token.path),
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
    )

    outputs.append(completion_token)

    projectFiles = []
    projectContents = []
    excludeManifestContents = []
    destProjectDirs = dict()

    if ctx.attr.exclude_from_manifest:
        for excGroup in ctx.attr.exclude_from_manifest:
            pathStart = len(excGroup.files_path_prefix) + 1
            for src in excGroup.outputs:
                projectPath = "%s/%s" % (excGroup.project_relative_dir, src.path[pathStart:])
                excludeManifestContents.append(projectPath)

    # Dictionary of all file paths to be included in the asset package
    # used to ensure there are no duplicates files being added to the package
    project_srcs_paths = {}

    for srcGroup in ctx.attr.srcs:
        # Copy all of the source files into the new project directory.
        pathStart = len(srcGroup.files_path_prefix) + 1
        for src in srcGroup.outputs:
            projectPath = "%s/%s" % (srcGroup.project_relative_dir, src.path[pathStart:])

            if not projectPath in project_srcs_paths.keys():
                project_srcs_paths[projectPath] = None
                projectContents.append(projectPath)

                # Accumulate all top-level directory names.
                destProjectDirs[projectPath.partition("/")[0]] = None

                if src.path.endswith("unsigned.bundle"):
                    signed_bundle_filepath = _rename_to_signed_bundle(ctx, projectPath)

                    if not _basename_from_string(ctx, signed_bundle_filepath) in ctx.attr.nativelib_final_destinations:
                        signed_out = ctx.actions.declare_file("%s-files/%s" % (ctx.outputs.out.basename, signed_bundle_filepath))
                    else:
                        # Specific Workaround where we allow the user to specify the bundle location to avoid signing verification from Apple
                        # (if the user wants that)
                        bfd = ctx.attr.nativelib_final_destinations.get(_basename_from_string(ctx, signed_bundle_filepath))
                        signed_out = ctx.actions.declare_file("%s-files/%s" % (ctx.outputs.out.basename, bfd))

                    projectFiles.append(signed_out)

                    _copy_sign(
                        ctx,
                        src,
                        signed_out,
                    )
                elif src.path.endswith(".dll") and (_basename_from_string(ctx, projectPath) in ctx.attr.nativelib_final_destinations):
                    # This is a workaround to Unity 2021 that complains having a <plugin_name>.DLL and <plugin_name>.asmdef with same <plugin_name>.
                    dll_filepath = ctx.attr.nativelib_final_destinations.get(_basename_from_string(ctx, projectPath))
                    out = ctx.actions.declare_file("%s-files/%s" % (ctx.outputs.out.basename, dll_filepath))
                    projectFiles.append(out)
                    _copy_action(ctx, src, out)
                else:
                    out = ctx.actions.declare_file("%s-files/%s" % (ctx.outputs.out.basename, projectPath))
                    projectFiles.append(out)
                    _copy_action(ctx, src, out)

    # Remove anything in the Assets directory that might be orphaned from a previous build.
    cleanUpFile = _clean_up_old_assets(ctx, "%s-files" % ctx.outputs.out.basename, projectFiles)
    auxFiles = [cleanUpFile]

    if ctx.attr.package_only_path != "":
        destProjectDirs = {ctx.attr.package_only_path: []}

    unity_log_path = "%s/%s" % (
        ctx.outputs.out.dirname,
        ctx.outputs.out.basename + ".unity_package_log.txt",
    )

    destProject = ctx.outputs.out.path + "-files"
    build_args = [
        "-quit",
        "-batchmode",
        "-nographics",
        "-silent-crashes",
        "-logFile",
        unity_log_path,
        "-projectPath",
        "$WORKSPACE/" + destProject,
        "-exportPackage",
    ] + destProjectDirs.keys() + ["$WORKSPACE/" + ctx.outputs.out.path]

    if ctx.attr.manifest:
        rootChar = projectFiles[0].short_path.find("/Assets") + 1
        assetFiles = []
        if rootChar == -1:
            fail("Encountered file %s which isn't an Assets path")
        for projectFile in projectFiles:
            assetPath = projectFile.short_path[rootChar:]
            if assetPath in excludeManifestContents:
                continue
            assetFiles.append(assetPath)

        manifestFile = ctx.actions.declare_file("%s-files/Assets/%s" % (ctx.outputs.out.basename, ctx.attr.manifest))
        auxFiles.append(manifestFile)
        ctx.actions.run(
            inputs = projectFiles,
            outputs = [manifestFile],
            executable = ctx.executable.manifester,
            arguments = [manifestFile.path, destProject] + assetFiles,
            execution_requirements = {
                "no-cache": "1",
            },
        )
        projectContents.insert(0, "Assets/" + ctx.attr.manifest)

    unity = ctx.toolchains["@unity-version//:toolchain_type"]
    ctx.actions.run(
        inputs = auxFiles + projectFiles + target_deps_files,
        outputs = [ctx.outputs.out],
        executable = ctx.executable.unitybuild,
        arguments = build_args,
        env = {
            "OUT_DIR": ctx.outputs.out.dirname,
            "UNITY": unity.unityinfo.unity_path,
            "PYTHON3": ctx.attr._python[platform_common.ToolchainInfo].py3_runtime.interpreter.path,
            "WORKSPACE_ENV": ctx.file._workspace_env.path,
        },
        mnemonic = "CreatingUnityPackage",
        progress_message = "Packaging %s with Unity" % ctx.label.name,
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
        tools = ctx.files._python,
    )
    outputs = [depset([ctx.outputs.out])]
    for srcGroup in ctx.attr.srcs:
        for platform, files in srcGroup.debugging_symbol_files.items():
            outputs.append(files)

    return [DefaultInfo(files = depset(transitive = outputs))]

_unity_package = rule(
    implementation = _package_impl,
    cfg = unity_platform_transition,
    attrs = {
        "target_deps": attr.label_list(
            default = [],
            allow_files = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "_unity_platform": attr.label(
            default = "@unity-version//:local",
        ),
        "out": attr.output(),
        "srcs": attr.label_list(
            default = [],
        ),
        "manifest": attr.string(
            default = "",
        ),
        "exclude_from_manifest": attr.label_list(
            default = [],
        ),
        "package_only_path": attr.string(
            default = "",
        ),
        "nativelib_final_destinations": attr.string_dict(
            default = {},
        ),
        "unitybuild": attr.label(
            default = Label("//bzl/unity:unity-build"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_copyandsign": attr.label(
            default = Label("//bzl/unity:copy-and-sign"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "manifester": attr.label(
            default = Label("//bzl/unity:generate-manifest"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
        ),
        "_python": attr.label(
            default = Label("@rules_python//python:current_py_toolchain"),
            providers = [platform_common.ToolchainInfo],
            cfg = "exec",
        ),
    },
    toolchains = [
        "@unity-version//:toolchain_type",
    ],
)

def unity_package(name, srcs, target_deps = [], exclude_from_manifest = None, package_name = None, package_only_path = None, nativelib_final_destinations = None, manifest = None, visibility = None, **kwargs):
    """Bazel rule to generate a .unitypackage file.

    Args:
      name (string): Target name.
      package_name (string): Package basename.
      package_only_path (string): Package files in only specified path.
      nativelib_final_destinations (dict): A dict that specify a different final path within the unity package for a specific native library file (only works for OSX and Windows)
      manifest (string): Optional file manifest to include, path relative to Assets dir.
      srcs (List[Label]): unity_plugins and unity_packages to package.
      exclude_from_manifest (List[Label]): Optional unity_plugins and unity_packages to exclude.
      visibility (Label): Bazel package visibility.
      target_deps (List[Label]): rules to be executed before, no matter what
    """

    # If target_compatible_with is not specified, require a unity installation.
    if "target_compatible_with" not in kwargs:
        kwargs["target_compatible_with"] = ["@unity-version//:unity-installed-constraint"]

    out = (package_name or name) + ".unitypackage"

    _unity_package(
        name = name,
        target_deps = target_deps,
        srcs = srcs,
        exclude_from_manifest = exclude_from_manifest,
        out = out,
        manifest = manifest,
        visibility = visibility,
        package_only_path = package_only_path,
        nativelib_final_destinations = nativelib_final_destinations,
        **kwargs
    )
