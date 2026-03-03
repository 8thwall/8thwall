"""Clang tools skylark rules

This file contains Starlark rules for running clang tools within our bazel
compilation. Currently supports clang-tidy and will generate a file of fixes
for checks that have automated fixes.

# Example Usage for clang-tidy
  # Step 0: Generate clang-apply-replacements wrapper:
    bazel build //bzl/crosstool/llvm:clang-apply-replacements
  # Step 1: Review the checks in //bzl/crosstool/llvm/clang-tools-config.yaml and change as necessary.
  # Step 2: Compile code with the clang_tidy aspect, requesting the 'fixes' that clang-tidy can produce
    # Example:
    bazel build bzl/hellobuild/cc:hello --aspects //bzl/crosstool/llvm:clang-tools.bzl%clang_tidy --output_groups=fixes
  # Step 3: Apply all replacements in the build output directory to modify code.
    bazel-bin/bzl/crosstool/llvm/clang-apply-replacements --format --style=file --style-config=$PWD bazel-bin/bzl/hellobuild/cc
"""

load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load(
    "@bazel_tools//tools/build_defs/cc:action_names.bzl",
    "CPP_COMPILE_ACTION_NAME",
)

CompileCommandsInfo = provider(
    fields = {
        "commands": "depset of compilation commands",
    },
)

def _clang_tidy_impl(target, ctx):
    if not ctx.rule.kind in ["cc_library", "cc_test", "cc_binary"]:
        return [
            CompileCommandsInfo(commands = depset([])),
            OutputGroupInfo(compile_commands = depset([])),
        ]

    if not CcInfo in target:
        return [
            CompileCommandsInfo(commands = depset([])),
            OutputGroupInfo(compile_commands = depset([])),
        ]
    toolchain = find_cpp_toolchain(ctx)

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    # Make sure the rule has a srcs attribute.
    ccCtx = target[CcInfo].compilation_context
    compile_commands = []
    if hasattr(ctx.rule.attr, "srcs"):
        defines = depset(transitive = [ccCtx.defines, ccCtx.local_defines])
        DISABLE_PROBLEMATIC_ERROR_DIAGNOSTICS = [
            "-Wno-elaborated-enum-base",
        ]
        for srcs in ctx.rule.attr.srcs:
            for src, out in zip(srcs.files.to_list(), target[OutputGroupInfo].compilation_outputs.to_list()):
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
                cpp_options = cc_common.get_memory_inefficient_command_line(
                    feature_configuration = feature_configuration,
                    action_name = CPP_COMPILE_ACTION_NAME,
                    variables = cpp_compile_variables,
                )
                args = [
                    cc_common.get_tool_for_action(feature_configuration = feature_configuration, action_name = CPP_COMPILE_ACTION_NAME),
                ]
                args += cpp_options
                cmd = " ".join(args)

                compile_commands.append(json.encode_indent({
                    "directory": "/private/var/tmp/_bazel_mc/c4f4f6f74f33415f35a1ab44963d1e4f/execroot/code8",
                    "command": cmd,
                    "file": src.path,
                }))
        all_compile_commands = depset(compile_commands, transitive = [dep[CompileCommandsInfo].commands for dep in ctx.rule.attr.deps])
        compile_commands_file = ctx.actions.declare_file("_cc_{}/{}".format(target.label.name, "compile_commands.json"))
        compile_database = "[\n%s\n]" % (",\n".join(all_compile_commands.to_list()))
        ctx.actions.write(compile_commands_file, compile_database)

        fixes = []
        for srcs in ctx.rule.attr.srcs:
            for src in srcs.files.to_list():
                if src.path.startswith("external/") or src.extension not in ["c", "cc", "cpp", "cxx", "c++", "C"]:
                    continue
                fix_yaml = ctx.actions.declare_file("{}-replacement.yaml".format(src.basename))
                ctx.actions.run_shell(
                    inputs = depset([ctx.file._config, compile_commands_file, src], transitive = [ccCtx.headers]),
                    outputs = [fix_yaml],
                    tools = [ctx.file._clang_tidy],
                    command = " ".join([
                        ctx.file._clang_tidy.path,
                        "--quiet",
                        "--export-fixes={}".format(fix_yaml.path),
                        "--config-file={}".format(ctx.file._config.path),
                        "-p={}".format(compile_commands_file.dirname),
                        src.path,
                        "&&",
                        "touch",
                        fix_yaml.path,
                    ]),
                )
                fixes.append(fix_yaml)

    return [
        CompileCommandsInfo(commands = all_compile_commands),
        OutputGroupInfo(fixes = depset(fixes), compile_commands = depset([compile_commands_file])),
    ]

clang_tidy = aspect(
    implementation = _clang_tidy_impl,
    attr_aspects = ["deps"],
    attrs = {
        "_cc_toolchain": attr.label(default = Label("@bazel_tools//tools/cpp:current_cc_toolchain")),
        "_clang_tidy": attr.label(default = Label("//bzl/crosstool/llvm:clang-tidy"), allow_single_file = True),
        "_config": attr.label(default = Label("//bzl/crosstool/llvm:clang-tidy-config"), allow_single_file = True),
    },
    fragments = ["cpp"],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
)
