def _manifest_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.manifest_template,
        output = ctx.outputs.out,
        substitutions = {
        },
    )

def _copy_action(ctx, src, dst):
    """Action to copy a file from src to dst."""
    ctx.actions.run_shell(
        inputs = [src],
        outputs = [dst],
        command = "cp \"%s\" \"%s\"" % (
            src.path,
            dst.path,
        ),
    )

def _copy_and_substitute_action(ctx, src, dst, regex):
    """Action to copy a file from src to dst."""
    ctx.actions.run_shell(
        inputs = [src],
        outputs = [dst],
        command = "sed \"%s\" \"%s\" > \"%s\"" % (
            regex,
            src.path,
            dst.path,
        ),
    )

def _extract_library_from_apk_impl(ctx):
    ctx.actions.run(
        inputs = [ctx.file.src],
        outputs = [ctx.outputs.out],
        progress_message = "Extracting Android native libraries",
        executable = ctx.file.extract_lib_from_apk,
        arguments = [
            ctx.label.name,
            ctx.file.src.path,
            ctx.outputs.out.dirname,
            ctx.outputs.out.basename,
        ],
    )

_manifest = rule(
    implementation = _manifest_impl,
    attrs = {
        "activity_label": attr.string(mandatory = True),
        "application_label": attr.string(mandatory = True),
        "min_sdk_version": attr.int(mandatory = True),
        "target_sdk_version": attr.int(mandatory = True),
        "manifest_template": attr.label(
            default = Label("//bzl/android:manifest"),
            allow_single_file = True,
        ),
    },
    fragments = ["cpp"],
    outputs = {"out": "%{name}/AndroidManifest.xml"},
)

_extract_library_from_apk = rule(
    implementation = _extract_library_from_apk_impl,
    attrs = {
        "libname": attr.string(
            mandatory = True,
        ),
        "src": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "extract_lib_from_apk": attr.label(
            default = Label("//bzl/android:extract-lib-from-apk"),
            allow_single_file = True,
        ),
    },
    outputs = {"out": "lib%{libname}.so"},
)

def _generate_plugin_aar_impl(ctx):
    inputs = [ctx.file.manifest, ctx.file.classes_jar, ctx.file.apk, ctx.file.proguard_includes, ctx.file.manifest_merger_jar]
    arguments = [
        ctx.file.manifest.path,
        ctx.file.classes_jar.path,
        ctx.file.apk.path,
        ctx.file.proguard_includes.path,
        ctx.outputs.out.dirname,
        ctx.outputs.out.basename,
        ctx.file.manifest_merger_jar.path,
    ]

    archs = ctx.split_attr.merge_aars.keys()
    if archs:
        merge_aar_files = ctx.split_attr.merge_aars[archs[0]]
    else:
        merge_aar_files = []

    for merge_aar_file_list in merge_aar_files:
        inputs.append(merge_aar_file_list.files.to_list()[0])
        arguments.append(merge_aar_file_list.files.to_list()[0].path)

    env = {
        "ARCHS": ",".join([str(a) for a in archs]),
    }
    if ctx.attr.strip_native:
        env["STRIP_NATIVE"] = "1"

    ctx.actions.run(
        inputs = inputs,
        outputs = [ctx.outputs.out],
        progress_message = "Generating Plugin AAR",
        executable = ctx.file.generate_plugin_aar,
        arguments = arguments,
        env = env,
    )

def _android_cc_deploy_zip_impl(ctx):
    filesToZip = []
    filesToZipNames = []

    # Copy over all native libraries to arch-specific lib directories.
    for (arch, archTarget) in ctx.split_attr.native.items():
        for soFile in archTarget.files.to_list():
            soFileOutName = "%s/lib/%s/%s" % (
                ctx.label.name,
                arch,
                soFile.basename.rpartition("-deps.so")[0] + ".so",
            )

            soFileOut = ctx.actions.declare_file(soFileOutName)
            _copy_action(ctx, soFile, soFileOut)
            filesToZip.append(soFileOut)
            filesToZipNames.append(soFileOutName)

    escapedPackage = "\\/".join(ctx.label.package.split("/"))

    # Copy over any JAR or AAR packages to jni directory.
    for src in ctx.files.srcs:
        ext = src.extension
        dst = {
            "aar": "jni",
            "jar": "jni",
            "h": "include",
        }.get(ext, "")

        srcFileOutName = "%s/%s/%s" % (
            ctx.label.name,
            dst,
            src.basename,
        )

        srcFileOut = ctx.actions.declare_file(srcFileOutName)
        if dst == "include":
            _copy_and_substitute_action(
                ctx,
                src,
                srcFileOut,
                's/#include[[:space:]]*\\"%s\\//#include \\"/' % escapedPackage,
            )
        else:
            _copy_action(ctx, src, srcFileOut)
        filesToZip.append(srcFileOut)
        filesToZipNames.append(srcFileOutName)

    ctx.actions.run_shell(
        inputs = filesToZip,
        outputs = [ctx.outputs.out],
        progress_message = "Creating Android CC zip archive",
        command = "cd %s; zip -q %s %s" % (
            ctx.outputs.out.dirname,
            ctx.outputs.out.basename,
            " ".join(filesToZipNames),
        ),
    )

_generate_plugin_aar = rule(
    implementation = _generate_plugin_aar_impl,
    attrs = {
        "manifest": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "apk": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "classes_jar": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "proguard_includes": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "merge_aars": attr.label_list(
            allow_files = True,
            cfg = android_common.multi_cpu_configuration,
        ),
        "manifest_merger_jar": attr.label(
            default = Label("@android-manifest-merger//file"),
            allow_single_file = True,
            cfg = "exec",
        ),
        "generate_plugin_aar": attr.label(
            default = Label("//bzl/android:generate-plugin-aar"),
            allow_single_file = True,
        ),
        "strip_native": attr.bool(
            default = False,
        ),
    },
    outputs = {"out": "%{name}.aar"},
)

_android_cc_deploy_zip = rule(
    implementation = _android_cc_deploy_zip_impl,
    attrs = {
        "srcs": attr.label_list(
            default = [],
            allow_files = True,
        ),
        "native": attr.label(
            default = None,
            cfg = android_common.multi_cpu_configuration,
        ),
    },
    outputs = {"out": "%{name}.zip"},
)

def android_cc_library(name, srcs = [], deps = []):
    """Bazel rule to build a cc_library for C/C++ code with the Android crosstool.
    If multiple architectures are specified with the flag --fat_apk_cpu, the resulting
    library will contain multiple architectures using the lipo tool.

    Args:
      name (string): Target name. Outputs will be libname.so.
      hdrs (List[Label]): Public header files to include. Put private headers in srcs.
      srcs (List[Label]): Source files to include in the library.
      deps (List[Label]): Dependent cc_library targets to include in the library.
    """

    if not deps and not srcs:
        fail("Must specify deps and/or srcs in android_cc_library rules.")

    cc_shared_lib = "lib" + name + "-cc-shared.so"
    cc_target = name + "-cc"
    manifest_target = name + "-manifest"

    # Create a shared library that statically links all dependencies.
    native.cc_binary(
        name = cc_shared_lib,
        srcs = srcs,
        deps = deps,
        linkshared = 1,
        linkstatic = 1,
    )
    native.cc_library(
        name = cc_target,
        srcs = [cc_shared_lib],
    )

    _manifest(
        name = manifest_target,
        application_label = "Dummy App",
        activity_label = "DummyApp",
        min_sdk_version = 21,
        target_sdk_version = 21,
    )
    native.android_binary(
        name = name + "-apk",
        deps = [cc_target] + ["//bzl/android:main-activity"],
        manifest = manifest_target,
        custom_package = "com.the8thwall.apps.client.exploratory.hellounity",
    )

    _extract_library_from_apk(
        name = name,
        libname = name,
        src = name + "-apk_unsigned.apk",
    )

def android_so_library(name, libname = None, deps = [], visibility = None):
    """Bazel rule to package a shared library from a precompiled .so file in cc_library rules.
    If multiple architectures are specified with the flag --fat_apk_cpu, the resulting
    library will contain multiple architectures using the lipo tool.

    Args:
      name (string): Target name. Outputs will preserve their original names.
      deps (List[Label]): Dependent cc_library targets with .so files to include in the library.
      visibility (List[Label]): Visibility targets.
    """

    if not deps:
        fail("Must specify deps in android_so_library rules.")

    manifest_target = name + "-manifest"

    _manifest(
        name = manifest_target,
        application_label = "Dummy App",
        activity_label = "DummyApp",
        min_sdk_version = 21,
        target_sdk_version = 21,
    )
    native.android_binary(
        name = name + "-apk",
        deps = deps + ["//bzl/android:main-activity"],
        manifest = manifest_target,
        custom_package = "com.the8thwall.apps.client.exploratory.hellounity",
    )

    _extract_library_from_apk(
        name = name,
        libname = libname or name,
        src = name + "-apk_unsigned.apk",
        visibility = None or visibility,
    )

def android_deploy_aar(name, deps = [], manifest = "", assets_dir = "", assets = [], proguard_specs = [], proguard_includes = None, merge_aars = [], strip_native = False, **kwargs):
    """
    Bazel rule to build an Android AAR package from a set of dependencies and a manifest file. This will
    include any transitive dependencies in the AAR package, which is not normally included when building an
    AAR via the "android_library" Bazel rule.

    Args:
      name (string): Target name. Outputs will be libname.so.
      deps (List[Label]): Dependent cc_library targets to include in the library.
      manifest (Label): The AndroidManifest.xml file to package with the generated AAR.
      proguard_specs (List[Label]): The sepcifications to use when running the deploy jar through proguard.
      proguard_includes (Label): The specifications to include in the aar for merging with future apks.
      assets (List[Label]): The assets to include in the aar.
      assets_dir (string): The directory to place assets in.
      strip_native (bool): Strip native library .so files in deps that aren't imports.
    """

    if not deps:
        fail("Must specify deps in android_deploy_aar rules.")

    if not manifest:
        fail("Must define manifest in android_deploy_aar rules.")

    manifest_target = name + "-manifest"

    _manifest(
        name = manifest_target,
        application_label = "Dummy App",
        activity_label = "DummyApp",
        min_sdk_version = 21,
        target_sdk_version = 21,
    )
    native.android_binary(
        name = name + "-apk",
        deps = deps,
        manifest = manifest_target,
        assets_dir = assets_dir,
        assets = assets,
        custom_package = "com.the8thwall.apps.client.exploratory.hellounity",
        proguard_specs = proguard_specs,
        **kwargs
    )

    # Create AAR from provided manifest & jar.
    classes_jar = ""

    if not proguard_specs:
        classes_jar = "%s-apk_deploy.jar" % name
    else:
        classes_jar = "%s-apk_proguard.jar" % name

    _generate_plugin_aar(
        name = name + "-aar",
        apk = name + "-apk_unsigned.apk",
        manifest = manifest,
        classes_jar = classes_jar,
        proguard_includes = proguard_includes,
        merge_aars = merge_aars,
        strip_native = strip_native,
    )

    native.genrule(
        name = name,
        srcs = [
            "%s-aar.aar" % name,
        ],
        outs = [
            name + ".aar",
        ],
        cmd = "cp $< $@",
        output_to_bindir = 1,
        **kwargs
    )

    native.aar_import(
        name = name + "-import",
        aar = name,
        **kwargs
    )

def android_cc_deploy_zip(name, srcs = [], deps = [], visibility = None):
    """Bazel rule to build C++ plus optional AAR/JAR code into a single distributable zip file.

      Args:
        name (string): Target name. The output will be name.zip.
        srcs (List[Label]): Source files to include. The rule will auto-sort them in the archive.
        deps (List[Label]): CC dependency targets to include, will be built for each architecture.
    """

    if not deps and not srcs:
        fail("Must specify deps and/or srcs in android_cc_library rules.")

    depsName = None

    if deps:
        depsName = "%s-deps" % name
        android_cc_library(
            name = depsName,
            deps = deps,
        )

    _android_cc_deploy_zip(
        name = name,
        srcs = srcs,
        native = depsName,
        visibility = visibility,
    )

def android_java_library(name, deps = [], srcs = [], visibility = None):
    """Bazel rule to build two targets for the Android sources.
       1) A java_library target with the Android platform jar as an addition dependency.
          The name for this target is built by appending "-java" to the input name.
       2) A android_library target with the same input.
    The extra java_library is built to allow files with Android dependencies to be unit tested. This
    can be done by adding a dependcy to name + "-java" to a java_test rule.

    Args:
      name (string): Target name.
      srcs (List[Label]): Sources files to build.
      deps (List[Label]): Dependent java_library targets needed to build srcs.
      visibility (List[Label]): Visibility targets.
    """
    native.android_library(
        name = name,
        deps = deps,
        srcs = srcs,
        visibility = visibility,
    )
    native.java_import(
        name = "%s-java" % name,
        jars = [
            "lib%s.jar" % name,
        ],
        visibility = visibility,
    )

def android_test(name, deps = [], srcs = [], test_class = "", visibility = None):
    """Bazel rule to run Android unit tests with Robolectric support. Currently only supports "java_*"
      rules as the deps. Use in conjuction with the android_java_library rule to test dependencies
      that would normally need to be compiled with the Android platform jar.

    Args:
      name (string): Target name.
      srcs (List[Label]): Source files to build.
      deps (List[Label]): Dependent java_library targets needed to build srcs.
      visibility (List[Label]): Visibility targets.
    """
    androidDep = [Label("//external:android-platform-jar")]
    robolectricDeps = androidDep + [Label("@robolectric//:robolectric")]
    native.java_test(
        name = name,
        srcs = srcs,
        deps = robolectricDeps + deps,
        test_class = test_class,
        timeout = "moderate",
        jvm_flags = [
            "-Drobolectric.offline=true",
            "-Drobolectric.dependency.dir='external/robolectric/libs'",
        ],
    )
