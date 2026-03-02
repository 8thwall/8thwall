load("//bzl/unity/impl:transitions.bzl", "unity_platform_transition", "unity_target_platforms_transition")
load("//bzl/android:android.bzl", "android_deploy_aar")
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//bzl/apple:dsymutil.bzl", "dsym_files")

def validate_meta_files(ctx, srcs):
    src_meta_paths = {}
    for src in srcs:
        if src.path.endswith(".meta"):
            src_meta_paths[src.path] = ""
    problematic_paths = []
    for src in srcs:
        if not src.path.endswith(".meta"):
            meta_path = src.path + ".meta"
            if meta_path in src_meta_paths:
                src_meta_paths.pop(meta_path)
            else:
                problematic_paths.append(src.path)
    for src_meta_path in src_meta_paths:
        problematic_paths.append(src_meta_path)
    if (len(problematic_paths) > 0):
        failure_message = "\n---------------------\n"
        failure_message += "*** Unity meta file validation failed for %s. Missing source + meta pairing for the following files ***\n" % (ctx.label.name)
        for problematic_path in problematic_paths:
            failure_message += "- " + problematic_path + "\n"
        failure_message += "---------------------\n"
        fail(failure_message)
    else:
        print("Unity meta file validation passed for " + ctx.label.name)

def copy_action(ctx, src, dst):
    """Action to copy a file from src to dst."""
    ctx.actions.run_shell(
        inputs = [src],
        outputs = [dst],
        command = "cp -v \"%s\" \"%s\"" % (
            src.path,
            dst.path,
        ),
        execution_requirements = {
            "no-cache": "1",
            "local": "1",
        },
        mnemonic = "RunShellCopyAction",
        progress_message = "unity-plugin.copy_action: Copying %s to %s" % (src.path, dst.path),
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

def symlink_action(ctx, src, dst):
    """Action to symlink a file from src to dst."""
    ctx.actions.symlink(
        output = dst,
        target_file = src,
        progress_message = "Symlinking %s to %s" % (src.path, dst.path),
    )

def metafy(filePath):
    return "%s.meta" % filePath

def _new_meta_file(ctx, filePath):
    pathParts = filePath.partition("Assets/")
    if pathParts[1]:
        assetPath = "".join(pathParts[1:])
    else:
        assetPath = filePath
    metaOut = ctx.actions.declare_file(metafy(filePath))
    ctx.actions.run_shell(
        inputs = [],
        outputs = [metaOut],
        command = (
            "cat <<EOF > %s\n" % metaOut.path +
            "fileFormatVersion: 2\n" +
            "guid: `echo -n %s | openssl md5 | cut -d' ' -f2`\n" % assetPath +
            "MonoImporter:\n" +
            "  serializedVersion: 2\n" +
            "  defaultReferences: []\n" +
            "  executionOrder: 0\n" +
            "  icon: {instanceID: 0}\n" +
            "  userData:\n" +
            "  assetBundleName:\n" +
            "  assetBundleVariant:\n" +
            "EOF\n"
        ) + " chmod +w %s" % metaOut.path,
    )
    return metaOut

def _new_plugin_meta_file(ctx, filePath, os_constraint_value, arch_constraint_value):
    is_os_osx = os_constraint_value == ctx.attr._os_osx[platform_common.ConstraintValueInfo]
    is_os_windows = os_constraint_value == ctx.attr._os_windows[platform_common.ConstraintValueInfo]
    is_os_android = os_constraint_value == ctx.attr._os_android[platform_common.ConstraintValueInfo]
    is_os_ios = os_constraint_value == ctx.attr._os_ios[platform_common.ConstraintValueInfo]
    is_os_linux = os_constraint_value == ctx.attr._os_linux[platform_common.ConstraintValueInfo]
    is_cpu_arm32 = arch_constraint_value == ctx.attr._cpu_arm[platform_common.ConstraintValueInfo]
    is_cpu_arm64 = arch_constraint_value == ctx.attr._cpu_arm64[platform_common.ConstraintValueInfo]
    is_cpu_x86_32 = arch_constraint_value == ctx.attr._cpu_x86_32[platform_common.ConstraintValueInfo]
    is_cpu_x86_64 = arch_constraint_value == ctx.attr._cpu_x86_64[platform_common.ConstraintValueInfo]

    editor_os_name = "AnyOS"
    if is_os_osx:
        editor_os_name = "OSX"
    elif is_os_windows:
        editor_os_name = "Windows"
    elif is_os_linux:
        editor_os_name = "Linux"

    cpu_name = "AnyCPU"
    if is_cpu_arm32:
        cpu_name = "ARMv7"
    elif is_cpu_arm64:
        cpu_name = "ARM64"
    elif is_cpu_x86_32:
        cpu_name = "X86"
    elif is_cpu_x86_64:
        cpu_name = "x86_64"

    pathParts = filePath.partition("Assets/")
    if pathParts[1]:
        assetPath = "".join(pathParts[1:])
    else:
        assetPath = filePath
    metaOut = ctx.actions.declare_file(metafy(filePath))
    ctx.actions.run_shell(
        inputs = [],
        outputs = [metaOut],
        command = (
            "cat <<EOF > %s\n" % metaOut.path +
            "fileFormatVersion: 2\n" +
            "guid: `echo -n %s | openssl md5 | cut -d' ' -f2`\n" % assetPath +
            "fileFormatVersion: 2\n" +
            "PluginImporter:\n" +
            "  externalObjects: {}\n" +
            "  serializedVersion: 2\n" +
            "  iconMap: {}\n" +
            "  executionOrder: {}\n" +
            "  isPreloaded: 0\n" +
            "  isOverridable: 0\n" +
            "  platformData:\n" +
            "  - first:\n" +
            "      : Any\n" +
            "    second:\n" +
            "      enabled: 0\n" +
            "      settings: {}\n" +
            "  - first:\n" +
            "      Android: Android\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_android) else "0") +
            "      settings:\n" +
            "        CPU: %s\n" % cpu_name +
            "  - first:\n" +
            "      Editor: Editor\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_osx or is_os_windows or is_os_linux) else "0") +
            "      settings:\n" +
            "        CPU: AnyCPU\n" +
            "        DefaultValueInitialized: true\n" +
            "        OS: %s\n" % editor_os_name +
            "  - first:\n" +
            "      Standalone: Linux\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_linux) else "0") +
            "      settings:\n" +
            "        CPU: %s\n" % cpu_name +
            "  - first:\n" +
            "      Standalone: Linux64\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_linux) else "0") +
            "      settings:\n" +
            "        CPU: %s\n" % cpu_name +
            "  - first:\n" +
            "      Standalone: LinuxUniversal\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_linux) else "0") +
            "      settings:\n" +
            "        CPU: %s\n" % cpu_name +
            "  - first:\n" +
            "      Standalone: OSXUniversal\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_osx) else "0") +
            "      settings:\n" +
            "        CPU: AnyCPU\n" +
            "  - first:\n" +
            "      '': OSXIntel\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_osx) else "0") +
            "      settings: {}\n" +
            "  - first:\n" +
            "      '': OSXIntel64\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_osx) else "0") +
            "      settings: {}\n" +
            "  - first:\n" +
            "      Standalone: Win\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_windows) else "0") +
            "      settings:\n" +
            "        CPU: %s\n" % cpu_name +
            "  - first:\n" +
            "      Standalone: Win64\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_windows) else "0") +
            "      settings:\n" +
            "        CPU: %s\n" % cpu_name +
            "  - first:\n" +
            "      iPhone: iOS\n" +
            "    second:\n" +
            "      enabled: %s\n" % ("1" if (is_os_ios) else "0") +
            "      settings:\n" +
            "        CPU: AnyCPU\n" +
            "        AddToEmbeddedBinaries: true\n" +
            "        CompileFlags: null\n" +
            "        FrameworkDependencies: Foundation;UIKit;Security;Metal\n" +
            "EOF\n"
        ) + " chmod +w %s" % metaOut.path,
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
    )

    return metaOut

def generate_missing_meta_files(ctx, files_path_prefix, outputs, outputs_paths):
    meta_files_to_create_paths = {}
    meta_exceptions = [".meta", ".DS_Store", "~"]
    pathStart = len(files_path_prefix) + 1
    already_has_metas = {}

    for output in outputs:
        output_path = output.path[pathStart:]
        output_path_splits = output_path.split("/")
        split_path = ctx.label.name
        for output_path_split in output_path_splits[:len(output_path_splits)]:
            split_path = "%s/%s" % (split_path, output_path_split)
            if split_path.endswith(".meta"):
                already_has_metas[split_path[:len(split_path) - 5]] = None

    for output in outputs:
        output_path = output.path[pathStart:]
        output_path_splits = output_path.split("/")
        split_path = ctx.label.name
        for output_path_split in output_path_splits[:len(output_path_splits)]:
            split_path = "%s/%s" % (split_path, output_path_split)
            is_exception = [meta_exception for meta_exception in meta_exceptions if split_path.endswith(meta_exception)]
            is_tracked = split_path in meta_files_to_create_paths
            already_has_meta = split_path in already_has_metas
            if not is_exception and not is_tracked and not already_has_meta:
                meta_files_to_create_paths[split_path] = None

    # Generate the missing .meta files and add them to the final list of outputs
    for meta_path in meta_files_to_create_paths.keys():
        if not metafy(meta_path) in outputs_paths:
            outputs_paths[metafy(meta_path)] = None
            outputs.append(_new_meta_file(ctx, meta_path))

def _plugin_impl(ctx):
    # Validate that each unity src file and directory has a .meta file pairing
    if ctx.files.meta_verification_srcs:
        validate_meta_files(ctx, ctx.files.meta_verification_srcs)

    # List of a src and plugin files in the build tree
    outputs = []

    # Dictionary of all file paths in the project, including generated sources
    # used to ensure there are no duplicates between plugin and project files being
    # added to the unity project where project files take priority over plugin files
    outputs_paths = {}

    target_deps_files = []
    for x in ctx.attr.target_deps:
        for y in x.files.to_list():
            target_deps_files.append(y)

    completion_token = ctx.actions.declare_file("{}.unity_plugin.completed".format(ctx.label.name))

    ctx.actions.run_shell(
        inputs = target_deps_files,
        outputs = [completion_token],
        command = (
            "echo '>>> Unity plugin {name} completed' > {ct}\n".format(name = ctx.label.name, ct = completion_token.path) +
            "\n".join(["echo {ctdep} >> {ct}; echo '^^^ Dependency file' >> {ct}\n ".format(ctdep = xd.path, ct = completion_token.path) for xd in target_deps_files])
        ) + " chmod +w {ct}".format(ct = completion_token.path),
        execution_requirements = {
            "local": "1",
            "no-cache": "1",
        },
    )

    ios_sdk_platform = ctx.fragments.apple.single_arch_platform.name_in_plist

    # Compute the lines required to add iOS SDK Frameworks.
    add_ios_frameworks = {}

    for platform, plugin in ctx.split_attr.plugin.items():
        if not platform:
            # No platforms to build.
            continue
        if not platform.startswith("ios_"):
            # Not iOS.
            continue
        if CcInfo not in plugin:
            # No objc provider.
            continue
        for framework in plugin[CcInfo].compilation_context.framework_includes.to_list():
            frameworkStr = 'addFrameworkToProjectMethod.Invoke(proj, new object[] {unityTarget, "%s.framework", true});'
            add_ios_frameworks[frameworkStr % framework] = None

    plugin_basename = ctx.attr.project

    actual_platforms = [item for item in ctx.attr.target_platforms if item not in ctx.attr.skip_native_platforms]

    target_constraints = {platform[platform_common.PlatformInfo].label.name: platform[platform_common.PlatformInfo].constraints for platform in actual_platforms}

    os_osx = ctx.attr._os_osx[platform_common.ConstraintValueInfo]
    os_windows = ctx.attr._os_windows[platform_common.ConstraintValueInfo]
    os_android = ctx.attr._os_android[platform_common.ConstraintValueInfo]
    os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]
    os_linux = ctx.attr._os_linux[platform_common.ConstraintValueInfo]

    cpu_arm = ctx.attr._cpu_arm[platform_common.ConstraintValueInfo]
    cpu_arm64 = ctx.attr._cpu_arm64[platform_common.ConstraintValueInfo]
    cpu_x86_32 = ctx.attr._cpu_x86_32[platform_common.ConstraintValueInfo]
    cpu_x86_64 = ctx.attr._cpu_x86_64[platform_common.ConstraintValueInfo]

    artifacts = {}

    for platform, plugin in ctx.split_attr.plugin.items():
        if not platform:
            continue
        plugin_format = None
        os_constraint_value = None
        arch_constraint_value = None

        if plugin and platform in target_constraints:
            constraint = target_constraints.get(platform)

            if constraint.has_constraint_value(os_osx):
                # osx plugins use the same plugin_format to make them universal (fat)
                os_constraint_value = os_osx
                plugin_format = "MacOS/{plugin_name}.bundle"
            if constraint.has_constraint_value(os_windows):
                os_constraint_value = os_windows
                if constraint.has_constraint_value(cpu_x86_32):
                    arch_constraint_value = cpu_x86_32
                    plugin_format = "Windows/x86/{plugin_name}.dll"
                elif constraint.has_constraint_value(cpu_x86_64):
                    arch_constraint_value = cpu_x86_64
                    plugin_format = "Windows/x86_64/{plugin_name}.dll"
            if constraint.has_constraint_value(os_android):
                os_constraint_value = os_android
                if constraint.has_constraint_value(cpu_x86_32):
                    arch_constraint_value = cpu_x86_32
                    plugin_format = "Android/x86/lib{plugin_name}.so"
                elif constraint.has_constraint_value(cpu_x86_64):
                    arch_constraint_value = cpu_x86_64
                    plugin_format = "Android/x86_64/lib{plugin_name}.so"
                elif constraint.has_constraint_value(cpu_arm):
                    arch_constraint_value = cpu_arm
                    plugin_format = "Android/armeabi-v7a/lib{plugin_name}.so"
                elif constraint.has_constraint_value(cpu_arm64):
                    arch_constraint_value = cpu_arm64
                    plugin_format = "Android/arm64-v8a/lib{plugin_name}.so"
            if constraint.has_constraint_value(os_ios):
                # ios plugins use the same plugin_format to make them universal (fat)
                os_constraint_value = os_ios
                plugin_format = "iOS/{plugin_name}.framework"
            if constraint.has_constraint_value(os_linux):
                os_constraint_value = os_linux
                if constraint.has_constraint_value(cpu_x86_32):
                    arch_constraint_value = cpu_x86_32
                    plugin_format = "Linux/x86/lib{plugin_name}.so"
                elif constraint.has_constraint_value(cpu_x86_64):
                    arch_constraint_value = cpu_x86_64
                    plugin_format = "Linux/x86_64/lib{plugin_name}.so"
                elif constraint.has_constraint_value(cpu_arm):
                    arch_constraint_value = cpu_arm
                    plugin_format = "Linux/armeabi-v7a/lib{plugin_name}.so"
                elif constraint.has_constraint_value(cpu_arm64):
                    arch_constraint_value = cpu_arm64
                    plugin_format = "Linux/arm64-v8a/lib{plugin_name}.so"

            if not plugin_format:
                fail("platform '{}' doesn't resolve to a known plugin format".format(platform))

            plugin_name = plugin_format.format(plugin_name = plugin_basename)

            # Apple platforms will have multiple plugins per dynamic library.
            # This will group them so that they can be combined using lipo.
            artifacts.setdefault(plugin_name, [[], "", "", ""])
            artifacts[plugin_name][0].append(plugin)
            artifacts[plugin_name][1] = plugin_basename
            artifacts[plugin_name][2] = os_constraint_value
            artifacts[plugin_name][3] = arch_constraint_value

    basePath = "%s/%s" % (ctx.label.name, ctx.attr.plugin_relative_dir)

    for editor_script in ctx.files.editor_scripts:
        outPath = "%s/Editor/%s" % (basePath, editor_script.basename)
        if not outPath in outputs_paths:
            outputs_paths[outPath] = None
            srcOut = ctx.actions.declare_file(outPath)
            symlink_action(ctx, editor_script, srcOut)
            outputs.append(srcOut)

    for plugin_name, plugin_info in artifacts.items():
        plugins = plugin_info[0]
        plugin_basename = plugin_info[1]
        os_constraint_value = plugin_info[2]
        arch_constraint_value = plugin_info[3]
        
        is_framework_plugin = plugin_name.find(".framework") > 0
        is_bundle_plugin = plugin_name.find(".bundle") > 0
        platform_name = plugin_name.split("/", 1)[0]

        plugin_out = None

        if is_framework_plugin or is_bundle_plugin:
            framework_bundle_dir = "%s/%s" % (basePath, plugin_name)

            if is_framework_plugin:
                outFile = "%s/%s" % (framework_bundle_dir, plugin_basename)
                plugin_out = ctx.actions.declare_file(outFile)
                if not outFile in outputs_paths:
                    outputs_paths[outFile] = None
                    plugin_out = ctx.actions.declare_file(outFile)
                    outputs.append(plugin_out)
            else:
                # Bundles are first created as unsigned binaries, which will later be copied and signed.
                outFile = "%s/unsigned/%s" % (basePath, plugin_name)
                plugin_out = ctx.actions.declare_file(outFile)

            # Removed the if check here in order to always create the metafiles, rather than skipping preexisting ones
            # This addresses cases where existing metafiles could have bad data, causing errors when trying to load the plugin [ARDK-5055]

            #if not metafy(framework_bundle_dir) in outputs_paths:
            outputs_paths[metafy(framework_bundle_dir)] = None

            # For framework/bundle plugins, create the plugin unity meta file placed adjacent to the plugin's containing directory
            outputs.append(_new_plugin_meta_file(ctx, framework_bundle_dir, os_constraint_value, arch_constraint_value))

            # For framework/bundle plugins, create the Info.plist
            if is_framework_plugin:
                info_out_path = "%s/Info.plist" % (framework_bundle_dir)
            else:
                info_out_path = "%s/Contents/Info.plist" % (framework_bundle_dir)

            if not info_out_path in outputs_paths:
                outputs_paths[info_out_path] = None
                info_out = ctx.actions.declare_file(info_out_path)
                outputs.append(info_out)

                minimum_os_version = ctx.attr._ios_min_version[BuildSettingInfo].value
                if os_constraint_value.label == Label("@@platforms//os:osx"):
                    minimum_os_version = ctx.attr._osx_min_version[BuildSettingInfo].value

                ctx.actions.write(
                    info_out,
                    '<?xml version="1.0" encoding="UTF-8"?>\n' +
                    '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">\n' +
                    '<plist version="1.0">\n' +
                    "<dict>\n" +
                    "    <key>CFBundleDevelopmentRegion</key>\n" +
                    "    <string>English</string>\n" +
                    "    <key>CFBundleExecutable</key>\n" +
                    "    <string>%s</string>\n" % plugin_basename +
                    "    <key>CFBundleIconFile</key>\n" +
                    "    <string></string>\n" +
                    "    <key>CFBundleIdentifier</key>\n" +
                    "    <string>com.niantic.%s</string>\n" % plugin_basename +
                    "    <key>CFBundleInfoDictionaryVersion</key>\n" +
                    "    <string>6.0</string>\n" +
                    "    <key>CFBundlePackageType</key>\n" +
                    "    <string>%s</string>\n" % ("FMWK" if (is_framework_plugin) else "APPL") +
                    "    <key>CFBundleSignature</key>\n" +
                    "    <string>????</string>\n" +
                    "    <key>CFBundleVersion</key>\n" +
                    "    <string>1659028229</string>\n" +
                    "    <key>CFBundleShortVersionString</key>\n" +
                    "    <string>5.0.0</string>\n" +
                    "    <key>CSResourcesFileMapped</key>\n" +
                    "    <true/>\n" +
                    "    <key>MinimumOSVersion</key>\n" +
                    "    <string>%s</string>\n" % minimum_os_version +
                    "</dict>\n" +
                    "</plist>\n",
                )
        else:
            outFile = "%s/%s" % (basePath, plugin_name)

            # Removed the if checks here in order to always create the metafiles, rather than skipping preexisting ones
            # This addresses cases where existing metafiles could have bad data, causing errors when trying to load the plugin [ARDK-5055]

            #if not outFile in outputs_paths:
            outputs_paths[outFile] = None
            plugin_out = ctx.actions.declare_file(outFile)
            outputs.append(plugin_out)

            #if not metafy(outFile) in outputs_paths:
            outputs_paths[metafy(outFile)] = None

            # Create the non-framework/bundle unity plugin meta file placed adjacent to the plugin
            outputs.append(_new_plugin_meta_file(ctx, outFile, os_constraint_value, arch_constraint_value))

        # Copy the plugin contents into the plugin destination
        if is_framework_plugin:
            # For frameworks, copy and rename the dynamic library, then update its rpaths for each linked library.
            dylib_path = "@rpath/%s.framework/%s" % (plugin_basename, plugin_basename)
            install_name_changes = "-id " + dylib_path

            ctx.actions.run_shell(
                inputs = [plugins[0].files.to_list()[0]],
                outputs = [plugin_out],
                tools = [ctx.file._install_name_tool],
                env = {
                    "RUNFILES_DIR": "{}.runfiles".format(ctx.file._install_name_tool.path),
                    "INSTALL_NAME_TOOL": "{int_full}.runfiles/{workspace}/{int_short}".format(int_full = ctx.file._install_name_tool.path, workspace = ctx.workspace_name, int_short = ctx.file._install_name_tool.short_path),
                },
                command = "cp -v {src} {dst} && chmod u+w {dst} && $INSTALL_NAME_TOOL {changes} {dst} && chmod u-w {dst}".format(
                    src = plugins[0].files.to_list()[0].path,
                    dst = plugin_out.path,
                    changes = install_name_changes,
                ),
            )
        elif len(plugins) > 1:
            # Combine the plugins using lipo.
            ctx.actions.run(
                inputs = depset(transitive = [plugin.files for plugin in plugins]),
                outputs = [plugin_out],
                executable = ctx.file._lipo_tool,
                arguments = [plugin.files.to_list()[0].path for plugin in plugins] + ["-create", "-output", plugin_out.path],
            )
        else:
            # Copy the dynamic library.
            copy_action(ctx, plugins[0].files.to_list()[0], plugin_out)

        # Bundles starts out as unsigned, so create a signed version
        if is_bundle_plugin:
            # Create a meta file for each parent directory in the bundle
            signed_outFile = "%s/%s" % (framework_bundle_dir, "Contents")
            signed_outFile = "%s/%s" % (signed_outFile, platform_name)
            signed_outFile = "%s/%s" % (signed_outFile, plugin_basename)
            if not signed_outFile in outputs_paths:
                outputs_paths[signed_outFile] = None
                signed_plugin_out = ctx.actions.declare_file(signed_outFile)
                _copy_sign(
                    ctx,
                    plugin_out,
                    signed_plugin_out,
                )
                outputs.append(signed_plugin_out)

    # Copy any sources to Plugins/Android.
    for plugin_src in ctx.files.android_plugin_srcs:
        outFile = "%s/Android/%s" % (basePath, plugin_src.basename)
        if not outFile in outputs_paths:
            outputs_paths[outFile] = None
            plugin_src_out = ctx.actions.declare_file(outFile)
            symlink_action(ctx, plugin_src, plugin_src_out)
            outputs.append(plugin_src_out)

    # Copy any sources to Plugins/iOS.
    for plugin_src in ctx.files.ios_plugin_srcs:
        outFile = "%s/iOS/%s" % (basePath, plugin_src.basename)
        if not outFile in outputs_paths:
            outputs_paths[outFile] = None
            plugin_src_out = ctx.actions.declare_file(outFile)
            symlink_action(ctx, plugin_src, plugin_src_out)
            outputs.append(plugin_src_out)

    # Copy any sources to Plugins.
    for plugin_src in ctx.files.plugin_srcs:
        outFile = "%s/%s" % (basePath, plugin_src.basename)
        if not outFile in outputs_paths:
            outputs_paths[outFile] = None
            plugin_src_out = ctx.actions.declare_file(outFile)
            symlink_action(ctx, plugin_src, plugin_src_out)
            outputs.append(plugin_src_out)

    # Copy CS generated sources to
    for cs_gen_src in ctx.files.cs_gen_srcs:
        extension = "".join(cs_gen_src.basename.rpartition(".")[1:])
        out_path = "%s/%s" % (basePath, cs_gen_src.basename)

        # Symlink the cs_gen_src file.
        if not out_path in outputs_paths:
            outputs_paths[out_path] = None
            src_out = ctx.actions.declare_file(out_path)
            symlink_action(ctx, cs_gen_src, src_out)
            outputs.append(src_out)

    debugging_symbol_outputs = {}

    # Apple specific logic to get dsyms for ios and osx. The outputed file
    # paths are the same so we seperate them by platform.
    for platform, dsym in ctx.split_attr.apple_dsym.items():
        if not platform:
            # No platforms to build.
            continue
        dsym_files = dsym.files.to_list()
        if len(dsym_files) > 0:
            debugging_symbol_outputs[platform] = depset(dsym_files)

    # Logic for non apple platforms to keep the unstripped plugin symbols
    for platform, plugin in ctx.split_attr.unstripped_plugin.items():
        if not platform or platform in debugging_symbol_outputs:
            # No platforms to build or we have already added the platform (the apple dsyms).
            continue
        debugging_symbol_outputs[platform] = depset(plugin.files.to_list())

    files_path_prefix = "%s/%s/%s" % (ctx.configuration.bin_dir.path, ctx.label.package, ctx.label.name)

    # Include source tree sources, ignoring ones that are already generated by Bazel
    for src in ctx.files.srcs:
        # Trim any prefix from the src file that is relative to the root
        to_trim = "%s/%s/" % (ctx.label.package, ctx.attr.trim_prefix_from_root)
        trimmed_path = src.short_path.replace(to_trim, "")

        if not ctx.attr.plugin_relative_dir in trimmed_path:
            # Take the trimmed path and append it to the designated src files path
            outPath = "%s/%s" % (ctx.label.name, trimmed_path)

            if not outPath in outputs_paths:
                outputs_paths[outPath] = None
                srcOut = ctx.actions.declare_file(outPath)
                symlink_action(ctx, src, srcOut)
                outputs.append(srcOut)
            else:
                print("Ignoring generated file in source tree: " + src.path)
        else:
            print("Ignoring generated file in source tree: " + src.path)

    # Determine all the missing .meta files
    generate_missing_meta_files(ctx, files_path_prefix, outputs, outputs_paths)

    return struct(
        files = depset(outputs + [completion_token]),
        outputs = outputs,
        project_relative_dir = ctx.attr.project_relative_dir,
        debugging_symbol_files = debugging_symbol_outputs,
        files_path_prefix = files_path_prefix,
        ios_frameworks_keys = add_ios_frameworks.keys(),
        package_name = ctx.attr.package_name,
    )

_unity_plugin = rule(
    implementation = _plugin_impl,
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
        # Whether to build for all target_platforms. If false (default), filter
        # to target_platforms that match the current --platforms setting.
        "build_all_target_platforms": attr.bool(
            default = False,
        ),
        "target_platforms": attr.label_list(
            default = ["@local_config_platform//:host"],
            providers = [platform_common.PlatformInfo],
        ),
        "skip_native_platforms": attr.label_list(
            default = [],
            providers = [platform_common.PlatformInfo],
        ),
        "project": attr.string(
            mandatory = True,
        ),
        "plugin": attr.label(
            # This should be allow_single_file, but a 1:2+ transition makes the attribute a list.
            allow_files = True,
            cfg = unity_target_platforms_transition,
            providers = [CcInfo],
        ),
        # Additional files to include in Android plugins directory.
        "android_plugin_srcs": attr.label_list(
            allow_files = True,
            # Same here, the following incantation causes bazel to break out any fat
            # binaries into individual single-arch files.
            cfg = android_common.multi_cpu_configuration,
        ),
        "plugin_relative_dir": attr.string(
            default = "Plugins/Libraries",
        ),
        "project_relative_dir": attr.string(
            default = "Assets",
        ),
        # Additional files to include in ios plugins directory.
        "ios_plugin_srcs": attr.label_list(
            allow_files = True,
        ),
        # Additional files to include in plugins directory.
        "plugin_srcs": attr.label_list(
            allow_files = True,
        ),
        "xcode_prefix": attr.string(
            # Prefix for output xcode projects.
            mandatory = True,
        ),
        "srcs": attr.label_list(
            allow_files = True,
        ),
        "meta_verification_srcs": attr.label_list(
            allow_files = True,
        ),
        "_copyandsign": attr.label(
            default = Label("//bzl/unity:copy-and-sign"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
        ),
        "editor_scripts": attr.label_list(
            default = [],
            allow_files = True,
        ),
        "cs_gen_srcs": attr.label_list(
            default = [],
            allow_files = True,
        ),
        "trim_prefix_from_root": attr.string(
            default = "",
        ),
        "package_name": attr.string(
            default = "",
        ),
        "_install": attr.label(
            default = "//bzl/llvm:llvm-lipo",
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_lipo_tool": attr.label(
            default = "//bzl/llvm:llvm-lipo",
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_install_name_tool": attr.label(
            default = "//bzl/llvm:llvm-install-name-tool",
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_cpu_arm": attr.label(
            default = "@platforms//cpu:arm",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_cpu_arm64": attr.label(
            default = "@platforms//cpu:arm64",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_cpu_x86_32": attr.label(
            default = "@platforms//cpu:x86_32",
            providers = [platform_common.ConstraintValueInfo],
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
        "apple_dsym": attr.label(
            # This should be allow_single_file, but a 1:2+ transition makes the attribute a list.
            allow_files = True,
            cfg = unity_target_platforms_transition,
        ),
        "unstripped_plugin": attr.label(
            # This should be allow_single_file, but a 1:2+ transition makes the attribute a list.
            allow_files = True,
            cfg = unity_target_platforms_transition,
            providers = [CcInfo],
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
    },
    fragments = ["apple"],
)

def unity_plugin(
        name,
        project,
        deps = [],
        target_deps = [],
        android_java_deps = [],
        android_manifest = "",
        android_srcs = [],
        android_proguard_specs = [],
        android_proguard_includes = "",
        android_merge_aars = [],
        ios_srcs = [],
        plugin_srcs = [],
        srcs = [],
        meta_verification_srcs = None,
        editor_srcs = [],
        cs_gen_srcs = [],
        trim_prefix_from_root = None,
        **kwargs):
    """Bazel rule to build a Unity plugin (set of files intended to be included in another project).

    Args:
      name (string): Target name and relative path to the Unity project directory.
      project (Label): Pretty name for the Unity project, e.g., MyUnityProject.
      deps (List[Label]): C/C++ Libraries that should be linked into Native Plugins.
      target_deps (List[Label]): Additional deps from other plugin targets.
      android_java_deps (List[Label]): Java Libraries that should be linked into Android Builds.
      android_manifest (Label): Android Manifest that should be merged with the host application's
                                manifest.
      android_srcs (List[Label]): Sources to copy into the Plugins/Android directory.
      android_proguard_specs(List[Label]): Specifications used to when running the Android aar
                                           through proguard.
      android_proguard_includes (List[Label]): Specifications to include when running the Android
                                               aar through proguard.
      android_merge_aars (List[Label]): Android aars to merge into final aar.
      ios_srcs (List[Label]): Sources to copy into the Plugins/iOS directory.
      plugin_srcs (List[Label]): CSharp sourcs to copy into the Plugins directory.
      srcs (List[Label]): List of files to include in the plugin.
      editor_srcs (List[Label]): List of files to include in the Editor directory.
      cs_gen_srcs (List[Label]): List of files to include in the Editor directory.
      trim_prefix_from_root (string): Helpful if you have an extra folder from your root
                                      project you want to remove.
      **kwargs: Additional arguments.
    """

    plugin = None
    dsym = None
    if deps:
        # Rule to build shared object library for Unity plugins.
        plugin = "lib%s-plugin" % name
        native.cc_binary(
            name = plugin,
            deps = deps,
            linkshared = 1,
            linkstatic = 1,
            linkopts = ["-DGENERATE_SHARED_BUNDLE"],
        )
        dsym = plugin + "-dsym"
        dsym_files(
            name = dsym,
            deps = [plugin],
        )

    android_java_plugin = []
    plugin_basename = project + "Plugin"
    if android_java_deps:
        android_java_plugin = [plugin_basename]
        android_deploy_aar(
            name = plugin_basename,
            deps = android_java_deps,
            manifest = android_manifest,
            proguard_specs = android_proguard_specs,
            proguard_includes = android_proguard_includes,
            merge_aars = android_merge_aars,
            strip_native = 1,
        )

    # set meta verification sources to default if request via empty array
    if meta_verification_srcs == [] and trim_prefix_from_root != None:
        meta_verification_srcs = native.glob(
            include = [trim_prefix_from_root + "/**"],
            exclude = [
                trim_prefix_from_root,
                trim_prefix_from_root + "/**/.*/**",
            ],
            exclude_directories = 0,
        )

    _unity_plugin(
        name = name,
        target_deps = target_deps,
        project = project,
        plugin = plugin,
        android_plugin_srcs = android_java_plugin + android_srcs,
        ios_plugin_srcs = ios_srcs,
        plugin_srcs = plugin_srcs,
        srcs = srcs,
        meta_verification_srcs = meta_verification_srcs,
        editor_scripts = editor_srcs,
        cs_gen_srcs = cs_gen_srcs,
        xcode_prefix = "xcode/",
        trim_prefix_from_root = trim_prefix_from_root,
        apple_dsym = dsym,
        unstripped_plugin = plugin,
        **kwargs
    )

    # Build with stripped binaries.
    _unity_plugin(
        name = name + ".stripped",
        target_deps = [":{}".format(name)] + ["{}.stripped".format(target) for target in target_deps],
        project = project,
        plugin = plugin + ".stripped" if plugin else plugin,
        android_plugin_srcs = android_java_plugin + android_srcs,
        ios_plugin_srcs = ios_srcs,
        plugin_srcs = plugin_srcs,
        srcs = srcs,
        meta_verification_srcs = meta_verification_srcs,
        editor_scripts = editor_srcs,
        cs_gen_srcs = cs_gen_srcs,
        xcode_prefix = "xcode/",
        trim_prefix_from_root = trim_prefix_from_root,
        apple_dsym = dsym,
        unstripped_plugin = plugin,
        **kwargs
    )
