load(
    "@build_bazel_rules_apple//apple/internal:resources.bzl",
    "resources",
)
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

def _extract_xcode_version_from_path(path):
    # Example: "external/xcode16" -> "16"
    parts = path.split("/")
    for part in parts:
        if part.startswith("xcode"):
            return part[len("xcode"):]

    fail("Could not extract Xcode version from path: {}".format(path))

def _metal_compiler_args(ctx, src, obj, platform, copts, diagnostics, deps_dump):
    """Returns arguments for metal compiler."""
    apple_fragment = ctx.fragments.apple

    ios_version_min_str = ""
    if platform == "ios":
        ios_version_min_str = "-mios-version-min=" + ctx.attr._ios_min_version[BuildSettingInfo].value

    # Convert .metal->.air->.metallib.
    args = copts + [
        "-arch",
        "air64",
        "-emit-llvm",
        "-c",
        "-gline-tables-only",
        "-ffast-math",
        "-serialize-diagnostics",
        diagnostics.path,
        "-o",
        obj.path,
        ios_version_min_str,
        "",
        src.path,
        "-MMD",
        "-MT",
        "dependencies",
        "-MF",
        deps_dump.path,
    ]
    return args

def _metal_compiler_inputs(srcs, hdrs, deps = []):
    """Determines the list of inputs required for a compile action."""

    cc_infos = [dep[CcInfo] for dep in deps if CcInfo in dep]

    dep_headers = depset(transitive = [
        cc_info.compilation_context.headers
        for cc_info in cc_infos
    ])

    return depset(srcs + hdrs, transitive = [dep_headers])

def _metal_library_impl(ctx):
    """Implementation for metal_library Starlark rule."""

    # A unique path for rule's outputs.
    objs_outputs_path = "{}.objs/".format(ctx.label.name)

    metal_folder = ctx.attr._xcode.label.workspace_root + "/Xcode_app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/metal"

    xcode_version = _extract_xcode_version_from_path(ctx.attr._xcode.label.workspace_root)

    os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]
    os_osx = ctx.attr._os_osx[platform_common.ConstraintValueInfo]

    sdk = None
    platform = None
    if ctx.target_platform_has_constraint(os_ios):
        platform = "ios"
        sdk = "iPhoneOS"
    elif ctx.target_platform_has_constraint(os_osx):
        platform = "macos"
        sdk = "MacOSX"
    else:
        fail("Unsupported platform selected")

    output_objs = []
    for src in ctx.files.srcs:
        basename = src.basename
        obj = ctx.actions.declare_file(objs_outputs_path + basename + ".air")
        output_objs.append(obj)
        diagnostics = ctx.actions.declare_file(objs_outputs_path + basename + ".dia")
        deps_dump = ctx.actions.declare_file(objs_outputs_path + basename + ".dat")

        args = _metal_compiler_args(
            ctx,
            src,
            obj,
            platform,
            ctx.attr.copts,
            diagnostics,
            deps_dump,
        )

        ctx.actions.run(
            inputs = _metal_compiler_inputs(ctx.files.srcs, ctx.files.hdrs, ctx.attr.deps),
            outputs = [obj, diagnostics, deps_dump],
            executable = ctx.executable.metalbuild,
            env = {
                "METAL_FOLDER": metal_folder,
                "WORKSPACE_ENV": ctx.file._workspace_env.path,
                "PLATFORM": platform,
                "SDK": sdk,
                "WORKSPACE_ROOT": ctx.attr._xcode.label.workspace_root,
                "XCODE_VERSION": xcode_version,
            },
            tools = ctx.files._xcode + [
                ctx.file._workspace_env,
            ],
            arguments = args,
            mnemonic = "MetalCompile",
            progress_message = ("Compiling Metal shader %s" % (basename)),
        )

    output_lib = ctx.actions.declare_file(ctx.label.name + ".metallib")
    args = [
        "-split-module",
        "-o",
        output_lib.path,
    ] + [x.path for x in output_objs]

    ctx.actions.run(
        inputs = output_objs,
        outputs = (output_lib,),
        mnemonic = "MetalLink",
        executable = ctx.executable.metallibbuild,
        arguments = args,
        env = {
            "METAL_FOLDER": metal_folder,
            "PLATFORM": platform,
            "WORKSPACE_ENV": ctx.file._workspace_env.path,
            "XCODE_VERSION": xcode_version,
        },
        tools = ctx.files._xcode + [
            ctx.file._workspace_env,
        ],
        progress_message = (
            "Linking Metal library %s" % ctx.label.name
        ),
    )

    objc_provider = apple_common.new_objc_provider(
        providers = [x[apple_common.Objc] for x in ctx.attr.deps if apple_common.Objc in x],
    )

    cc_infos = [dep[CcInfo] for dep in ctx.attr.deps if CcInfo in dep]
    if ctx.files.hdrs:
        cc_infos.append(
            CcInfo(
                compilation_context = cc_common.create_compilation_context(
                    headers = depset([f for f in ctx.files.hdrs]),
                ),
            ),
        )

    return [
        DefaultInfo(files = depset([output_lib]), runfiles = ctx.runfiles(files = [output_lib])),
        objc_provider,
        cc_common.merge_cc_infos(cc_infos = cc_infos),
        # Return the provider for the new bundling logic of rules_apple.
        resources.bucketize_typed([output_lib], "unprocessed"),
    ]

METAL_LIBRARY_ATTRS = {
    "srcs": attr.label_list(allow_files = [".metal"], allow_empty = False),
    "hdrs": attr.label_list(allow_files = [".h"]),
    "deps": attr.label_list(providers = [["objc", CcInfo], [apple_common.Objc, CcInfo]]),
    "copts": attr.string_list(),
    "metalbuild": attr.label(
        default = Label("//bzl/gpu:metal-build"),
        allow_single_file = True,
        executable = True,
        cfg = "exec",
    ),
    "metallibbuild": attr.label(
        default = Label("//bzl/gpu:metallib-build"),
        allow_single_file = True,
        executable = True,
        cfg = "exec",
    ),
    "_workspace_env": attr.label(
        default = "@workspace-env//:workspace-env",
        allow_single_file = True,
    ),
    "_xcode": attr.label(
        default = "//bzl/xcode:stub",
        allow_files = True,
    ),
    "_os_ios": attr.label(
        default = "@platforms//os:ios",
        providers = [platform_common.ConstraintValueInfo],
    ),
    "_os_osx": attr.label(
        default = "@platforms//os:osx",
        providers = [platform_common.ConstraintValueInfo],
    ),
    "_ios_min_version": attr.label(
        default = Label("//bzl/xcode:ios-min-version"),
        providers = [BuildSettingInfo],
    ),
}

metal_library = rule(
    implementation = _metal_library_impl,
    attrs = METAL_LIBRARY_ATTRS,
    fragments = ["apple", "objc", "swift"],
    output_to_genfiles = True,
)
