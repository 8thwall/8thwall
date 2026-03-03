"""Clang tools skylark rules

This file contains Starlark rules for generating a compile_commands.json file for build targets. 
"""

load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load(
    "@bazel_tools//tools/build_defs/cc:action_names.bzl",
    "CPP_COMPILE_ACTION_NAME",
    "OBJC_COMPILE_ACTION_NAME",
)

CompileCommandsInfo = provider(
    fields = {
        "commands": "depset of compilation commands",
    },
)

CPP_TOOLCHAIN_TYPE = "@bazel_tools//tools/cpp:toolchain_type"

def _genReturnStruct(target, ctx, compile_commands):
    deps = ctx.rule.attr.deps if hasattr(ctx.rule.attr, "deps") else []
    deps = [dep for dep in deps if hasattr(dep, "CompileCommandsInfo")]
    all_compile_commands = depset(compile_commands, transitive = [dep[CompileCommandsInfo].commands for dep in deps])
    compile_commands_file = ctx.actions.declare_file("_cc_{}/{}".format(target.label.name, "compile_commands.json"))
    compile_database = "[\n%s\n]" % (",\n".join(all_compile_commands.to_list()))
    ctx.actions.write(compile_commands_file, compile_database)

    return [
        CompileCommandsInfo(commands = all_compile_commands),
        OutputGroupInfo(compile_commands = depset([compile_commands_file])),
    ]

def _compile_commands_impl(target, ctx):
    if not CcInfo in target or not hasattr(ctx.rule.attr, "srcs"):
        return _genReturnStruct(target, ctx, [])

    toolchain = find_cpp_toolchain(ctx)

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    ccCtx = target[CcInfo].compilation_context
    compile_commands = []

    defines = depset(transitive = [ccCtx.defines, ccCtx.local_defines])
    DISABLE_PROBLEMATIC_ERROR_DIAGNOSTICS = [
        "-Wno-elaborated-enum-base",
    ]
    for srcs in ctx.rule.attr.srcs:
        if "compilation_outputs" not in target[OutputGroupInfo]:
            continue
        for src, out in zip(srcs.files.to_list(), target[OutputGroupInfo].compilation_outputs.to_list()):
            objc = False
            if ctx.rule.kind in ["objc_library", "_nia_objc_library"]:
                objc = True
            built_in_includes = [inc for inc in toolchain.built_in_include_directories if not inc.endswith("/Frameworks")]
            built_in_frameworks = [inc for inc in toolchain.built_in_include_directories if inc.endswith("/Frameworks")]
            cpp_compile_variables = cc_common.create_compile_variables(
                feature_configuration = feature_configuration,
                cc_toolchain = toolchain,
                user_compile_flags = ctx.fragments.cpp.copts + ctx.rule.attr.copts + DISABLE_PROBLEMATIC_ERROR_DIAGNOSTICS,
                source_file = src.path,
                output_file = out.path,
                include_directories = ccCtx.includes,
                quote_include_directories = ccCtx.quote_includes,
                system_include_directories = depset(built_in_includes, transitive = [ccCtx.system_includes]),
                framework_include_directories = depset(transitive = [ccCtx.framework_includes, depset(built_in_frameworks)]),
                preprocessor_defines = defines,
            )
            action_name = OBJC_COMPILE_ACTION_NAME if objc else CPP_COMPILE_ACTION_NAME
            cpp_options = cc_common.get_memory_inefficient_command_line(
                feature_configuration = feature_configuration,
                action_name = action_name,
                variables = cpp_compile_variables,
            )
            args = [
                cc_common.get_tool_for_action(feature_configuration = feature_configuration, action_name = action_name),
            ]
            args += cpp_options
            cmd = " ".join(args)

            compile_commands.append(json.encode_indent({
                "command": cmd,
                "file": src.path,
            }))
    return _genReturnStruct(target, ctx, compile_commands)

compile_commands = aspect(
    implementation = _compile_commands_impl,
    attr_aspects = ["deps"],
    attrs = {
        "_cc_toolchain": attr.label(default = Label("@bazel_tools//tools/cpp:current_cc_toolchain")),
    },
    required_providers = [CcInfo],
    provides = [CompileCommandsInfo],
    fragments = ["cpp"],
    toolchains = [
        CPP_TOOLCHAIN_TYPE,
    ],
)
