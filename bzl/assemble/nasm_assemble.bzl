load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load(
    "@bazel_tools//tools/build_defs/cc:action_names.bzl",
    "ASSEMBLE_ACTION_NAME",
)

def _nasm_assemble_impl(ctx):
    toolchain = find_cpp_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    # Add the src paths to include paths.
    srcFolders = []
    for src in ctx.files.srcs:
        if (src.dirname not in srcFolders):
            srcFolders.append(src.dirname)
    includes = ["-I" + folder for folder in srcFolders]

    os_windows = ctx.attr._windows[platform_common.ConstraintValueInfo]
    output_suffix = ".obj" if ctx.target_platform_has_constraint(os_windows) else ".o"

    outputs = []
    for src in ctx.files.srcs:
        if not src.basename.split(".")[-1] == "asm":
            continue

        # Skip building the assemble in the excludes list.
        if src in ctx.files.excludes:
            continue
        objFile = ctx.actions.declare_file("_nasmobjs/{}{}".format(src.basename, output_suffix))
        outputs.append(objFile)

        # If getting compiler built-in includes is useful.
        built_in_includes = [inc for inc in toolchain.built_in_include_directories if not inc.endswith("/Frameworks")]

        ctx.actions.run(
            inputs = ctx.files.srcs,
            outputs = [objFile],
            progress_message = "Generating assemble objs for " + src.path,
            executable = ctx.executable._nasm,
            arguments = ctx.attr.copts + includes + [
                src.path,
                "-o",
                objFile.path,
            ],
        )

    compilation_outputs = cc_common.create_compilation_outputs(objects = depset(outputs), pic_objects = depset(outputs))
    compilation_context = cc_common.create_compilation_context(
        # More could be added here if you add future support for defines, local_defines, includes, etc.
        system_includes = depset(built_in_includes),
    )
    link_context = cc_common.create_linking_context_from_compilation_outputs(
        name = ctx.label.name,
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        compilation_outputs = compilation_outputs,
        user_link_flags = ctx.fragments.cpp.linkopts,
    )

    link_outputs = cc_common.link(
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        compilation_outputs = compilation_outputs,
        user_link_flags = [],
        linking_contexts = [],
        name = ctx.label.name,
        output_type = "dynamic_library",
        link_deps_statically = True,
    )

    return [
        DefaultInfo(files = depset(link_outputs.library_to_link.pic_static_library)),
        CcInfo(
            compilation_context = compilation_context,
            linking_context = link_context[0],
        ),
    ]

_nasm_assemble = rule(
    implementation = _nasm_assemble_impl,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "copts": attr.string_list(
            default = [],
            mandatory = False,
        ),
        "excludes": attr.label_list(
            mandatory = False,
            allow_files = True,
        ),
        "_nasm": attr.label(
            default = "@nasm//:nasm",
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_windows": attr.label(
            default = "@platforms//os:windows",
            providers = [platform_common.ConstraintValueInfo],
        ),
    },
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    fragments = ["cpp"],
)

def nasm_assemble(name, srcs = [], copts = [], excludes = []):
    _nasm_assemble(
        name = name,
        srcs = srcs,
        copts = copts,
        excludes = excludes,
    )
