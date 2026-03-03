load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "action_config",
    "artifact_name_pattern",
    "env_entry",
    "env_set",
    "feature",
    "flag_group",
    "flag_set",
    "tool",
    "tool_path",
    "variable_with_value",
    "with_feature_set",
)
load("@local-env//:vars.bzl", "BAZEL_SH")
load("//bzl/crosstool:toolchain-compiler-flags.bzl", "DBG_COMPILER_FLAGS", "FASTBUILD_COMPILER_FLAGS", "OPT_WITHOUT_SYMBOLS_COMPILER_FLAGS")

# All compilation actions for c/c++.
all_compile_actions = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.clif_match,
    ACTION_NAMES.lto_backend,
]

# Assemble action.
all_assemble_actions = [
    ACTION_NAMES.assemble,
]

# Compilation acctions for c++ only.
all_cpp_compile_actions = [
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.clif_match,
]

# All linking actions.
all_link_actions = [
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]

all_archiver_actions = [
    ACTION_NAMES.cpp_link_static_library,
]

def _impl(ctx):
    msvc_root = ctx.attr.msvc.label.workspace_root
    msvc_toolchain = "BAZEL_OUTPUT_BASE/%s" % msvc_root

    llvminfo = ctx.toolchains["//bzl/llvm:toolchain_type"].llvminfo

    llvm_root = llvminfo.root
    llvm_usr = llvminfo.usr
    llvm_toolchain = "BAZEL_OUTPUT_BASE/%s" % llvm_usr

    bazel_sh_internal_interpreter = BAZEL_SH

    # These directories can be referenced in dependency files without being tracked by bazel.
    # Windows needs a fake disk path, just using forward slash without disk reference will fail
    cxx_builtin_include_directories = [
        "/{{deceive_bazel_header_deps}}/%s/include" % llvminfo.runtime,
        "/{{deceive_bazel_header_deps}}/%s/share" % llvminfo.runtime,
        "/{{deceive_bazel_header_deps}}/%s/x86_64/include/msvc" % msvc_root,
        "/{{deceive_bazel_header_deps}}/%s/x86_64/include/um" % msvc_root,
        "/{{deceive_bazel_header_deps}}/%s/x86_64/include/ucrt" % msvc_root,
        "/{{deceive_bazel_header_deps}}/%s/x86_64/include/shared" % msvc_root,
        "/{{deceive_bazel_header_deps}}/%s/x86_64/include/winrt" % msvc_root,
        "%s/{{deceive_bazel_header_deps}}/%s/include" % (bazel_sh_internal_interpreter, llvminfo.runtime),
        "%s/{{deceive_bazel_header_deps}}/%s/share" % (bazel_sh_internal_interpreter, llvminfo.runtime),
        "%s/{{deceive_bazel_header_deps}}/%s/x86_64/include/msvc" % (bazel_sh_internal_interpreter, msvc_root),
        "%s/{{deceive_bazel_header_deps}}/%s/x86_64/include/um" % (bazel_sh_internal_interpreter, msvc_root),
        "%s/{{deceive_bazel_header_deps}}/%s/x86_64/include/ucrt" % (bazel_sh_internal_interpreter, msvc_root),
        "%s/{{deceive_bazel_header_deps}}/%s/x86_64/include/shared" % (bazel_sh_internal_interpreter, msvc_root),
        "%s/{{deceive_bazel_header_deps}}/%s/x86_64/include/winrt" % (bazel_sh_internal_interpreter, msvc_root),
    ]

    # Define the tools that are used in compilation and linking. Practically,
    # only "gcc" and "ar", i.e., compiling/linking and static linking will
    # actually be used.
    tool_paths = [
        tool_path(name = "gcc", path = ctx.executable.clang_wrapper.basename),
        tool_path(name = "ar", path = ctx.executable.ar_wrapper.basename),
        tool_path(name = "cpp", path = "/usr/bin/false"),  # unused
        tool_path(name = "dwp", path = "/usr/bin/false"),  # unused
        tool_path(name = "gcov", path = "/usr/bin/false"),  # unused
        tool_path(name = "ld", path = "/usr/bin/false"),  # unused
        tool_path(name = "nm", path = "/usr/bin/false"),  # unused
        tool_path(name = "objcopy", path = "/usr/bin/false"),  # unused
        tool_path(name = "objdump", path = "/usr/bin/false"),  # unused
        tool_path(name = "strip", path = "/usr/bin/strip"),  # TODO(mc): Does llvm-strip work for windows?
    ]

    strip_action = action_config(
        action_name = ACTION_NAMES.strip,
        flag_sets = [
            flag_set(
                flag_groups = [
                    flag_group(flags = ["-S"], expand_if_false = "stripopts"),
                    flag_group(
                        flags = ["%{stripopts}"],
                        iterate_over = "stripopts",
                    ),
                    flag_group(flags = ["-o", "%{output_file}"]),
                    flag_group(flags = ["%{input_file}"]),
                ],
            ),
        ],
        tools = [tool(tool = ctx.executable.strip_tool)],
    )

    # These special bazel features are enabled by -c opt and -c dbg (or default
    # of -c fastbuild). Here we declare them so they can be used in rules
    # later.
    opt_feature = feature(name = "opt")
    fastbuild_feature = feature(name = "fastbuild")
    dbg_feature = feature(name = "dbg")

    # These special bazel features communicate certain toolchain functionality to bazel.
    supports_fission_feature = feature(name = "supports_fission", enabled = False)
    supports_gold_linker_feature = feature(name = "supports_gold_linker", enabled = False)
    supports_incremental_linker_feature = feature(name = "supports_incremental_linker", enabled = False)
    supports_interface_shared_objects_feature = feature(name = "supports_interface_shared_objects", enabled = False)
    supports_normalizing_ar_feature = feature(name = "supports_normalizing_ar", enabled = False)
    supports_start_end_lib_feature = feature(name = "supports_start_end_lib", enabled = False)

    # Default flags set cor compilation actions.
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                # C++ compile actions: Compile for C++20 with exceptions enabled.
                actions = all_cpp_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-std=c++20",
                    "-fcxx-exceptions",
                ])],
            ),
            flag_set(
                # C compile Actions: Compile for C17.
                actions = [
                    ACTION_NAMES.c_compile,
                ],
                flag_groups = [flag_group(flags = [
                    "-std=c17",
                ])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(flags = [
                        "-fms-extensions",
                        "-fdelayed-template-parsing",
                        "-fexceptions",
                        "-mthread-model",
                        "posix",
                        "-fno-threadsafe-statics",
                        "-target",
                        "x86_64-pc-win32",
                        "-Wno-msvc-not-found",
                        "-msse4",
                        "-mpopcnt",
                        "-U_FORTIFY_SOURCE",
                        "-D_FORTIFY_SOURCE=1",
                        "-fstack-protector",
                        "-Wall",
                        "-Wthread-safety",
                        "-Wself-assign",
                        "-fno-omit-frame-pointer",
                        "-DWIN32",
                        "-D_WIN32",
                        "-D_MT",
                        "-D_DLL",
                        "-Xclang",
                        "-disable-llvm-verifier",
                        "-Xclang",
                        "--dependent-lib=msvcrt",
                        "-Xclang",
                        "--dependent-lib=ucrt",
                        "-Xclang",
                        "--dependent-lib=oldnames",
                        "-Xclang",
                        "--dependent-lib=vcruntime",
                        "-D_CRT_SECURE_NO_WARNINGS",
                        "-D_CRT_NONSTDC_NO_DEPRECATE",
                        "-fms-compatibility-version=19",
                        "-U__GNUC__",
                        "-U__gnu_linux__",
                        "-U__GNUC_MINOR__",
                        "-U__GNUC_PATCHLEVEL__",
                        "-U__GNUC_STDC_INLINE__",
                        "-fcolor-diagnostics",
                    ]),
                ],
            ),
            flag_set(
                actions = all_assemble_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-m64",  # 64-bit assembly.
                            "--timestamp=0",
                        ],
                    ),
                ],
            ),
        ],
    )

    # If enabled, output a shell script instead of a native executable that
    # runs the executable as a self-extracting archive within wine64.
    winerun_feature = feature(
        name = "winerun",
        env_sets = [
            env_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                ],
                env_entries = [
                    env_entry("WINE64", ctx.file.wine64.path),
                ],
            ),
        ],
    )

    # Pass in the llvm toolchain path as an environment variable.
    default_env_feature = feature(
        name = "default_env",
        enabled = True,
        env_sets = [
            env_set(
                actions = all_compile_actions + all_link_actions + all_archiver_actions + all_assemble_actions,
                env_entries = [
                    env_entry("LLVM_USR", llvminfo.usr),
                    env_entry("WORKSPACE_ENV", ctx.file.workspace_env.path),
                ],
            ),
        ],
    )

    # This long block is essentially recreating the Bazel default behavior for
    # the built-in libraries_to_link feature, but overriding what happens when
    # alwayslink=1 is on for a cc_library that is linked into a dynamic library.
    # Instead of linking lib.a as '-Wl,-whole-archive lib.a -Wl,-no-whole-archive',
    # it uses the windows-compatible syntax '-Wl,-wholearchive:lib.a'.
    libraries_to_link_feature = feature(
        name = "libraries_to_link",
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        iterate_over = "libraries_to_link",
                        flag_groups = [
                            flag_group(
                                flags = ["-Wl,--start-lib"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "object_file_group",
                                ),
                            ),
                            flag_group(
                                flags = ["%{libraries_to_link.object_files}"],
                                iterate_over = "libraries_to_link.object_files",
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "object_file_group",
                                ),
                            ),
                            flag_group(
                                flags = ["%{libraries_to_link.name}"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "object_file",
                                ),
                            ),
                            flag_group(
                                flags = ["%{libraries_to_link.name}"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "interface_library",
                                ),
                            ),
                            flag_group(
                                # Here we use the windows link syntax to say
                                # that all of the public symbols in the static
                                # library should be included in the dynamic
                                # library.
                                flags =
                                    ["-Wl,-wholearchive:%{libraries_to_link.name}"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "static_library",
                                ),
                                expand_if_true = "libraries_to_link.is_whole_archive",
                            ),
                            flag_group(
                                flags = ["%{libraries_to_link.name}"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "static_library",
                                ),
                                expand_if_false = "libraries_to_link.is_whole_archive",
                            ),
                            flag_group(
                                flags = ["-l%{libraries_to_link.name}"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "dynamic_library",
                                ),
                            ),
                            flag_group(
                                flags = ["-l:%{libraries_to_link.name}"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "versioned_dynamic_library",
                                ),
                            ),
                            flag_group(
                                flags = ["-Wl,--end-lib"],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "object_file_group",
                                ),
                            ),
                        ],
                        expand_if_available = "libraries_to_link",
                    ),
                    flag_group(
                        flags = ["-Wl,@%{thinlto_param_file}"],
                        expand_if_true = "thinlto_param_file",
                    ),
                ],
            ),
        ],
    )

    # Override non-deterministic defines to ensure repeatable builds.
    deterministic_compilation_feature = feature(
        name = "deterministic_compilation",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                # Make C++ compilation deterministic. Use linkstamping instead
                # of these compiler symbols.
                flag_groups = [flag_group(flags = [
                    "-Wno-builtin-macro-redefined",
                    "-D__DATE__=\"redacted\"",
                    "-D__TIMESTAMP__=\"redacted\"",
                    "-D__TIME__=\"redacted\"",
                ])],
            ),
        ],
    )

    # Add system includes since clang is not aware of the sysroot and can't
    # make default assumptions about the location of these includes.
    toolchain_include_directories_feature = feature(
        name = "toolchain_include_directories",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-isystem",
                            "BAZEL_OUTPUT_BASE/%s/include" % llvminfo.runtime,
                            "-isystem",
                            "%s/x86_64/include/msvc" % msvc_toolchain,
                            "-isystem",
                            "%s/x86_64/include/um" % msvc_toolchain,
                            "-isystem",
                            "%s/x86_64/include/ucrt" % msvc_toolchain,
                            "-isystem",
                            "%s/x86_64/include/shared" % msvc_toolchain,
                            "-isystem",
                            "%s/x86_64/include/winrt" % msvc_toolchain,
                        ],
                    ),
                ],
            ),
        ],
    )

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-target",
                            "x86_64-pc-win32",
                            "-fuse-ld=lld",
                            "-nostdlib",
                            "-lmsvcrt",
                            "-Wno-msvc-not-found",
                            "-L%s/x86_64/lib/msvc" % msvc_toolchain,
                            "-L%s/x86_64/lib/um" % msvc_toolchain,
                            "-L%s/x86_64/lib/ucrt" % msvc_toolchain,
                            "-fmsc-version=1900",
                            "-Wl,-machine:x64",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            # Link with LTO and --gc-sections to optimize for size and performance in opt build.
                            "-flto",  # LTO needs to be in both compiling and linking flags.
                            "-Wl,--gc-sections",  # Strip the dead code.
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    # Additional link option for debug build
    if "COMPILATION_MODE" in ctx.var:
        if ctx.var["COMPILATION_MODE"] == "dbg":
            default_link_flags_feature.flag_sets.append(
                flag_set(
                    actions = all_link_actions,
                    flag_groups = [
                        flag_group(
                            flags = [
                                "-lucrtd",
                            ],
                        ),
                    ],
                ),
            )

    build_type_feature = feature(
        name = "build_type",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = DBG_COMPILER_FLAGS)],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = FASTBUILD_COMPILER_FLAGS)],
                with_features = [with_feature_set(features = ["fastbuild"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = OPT_WITHOUT_SYMBOLS_COMPILER_FLAGS)],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    include_paths_feature = feature(
        name = "include_paths",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        iterate_over = "quote_include_paths",
                        flags = ["-iquote", "%{quote_include_paths}"],
                    ),
                    flag_group(
                        iterate_over = "include_paths",
                        flags = ["-I%{include_paths}"],
                    ),
                    flag_group(
                        iterate_over = "system_include_paths",
                        flags = ["-isystem", "%{system_include_paths}"],
                    ),
                    flag_group(
                        iterate_over = "framework_include_paths",
                        flags = ["-F%{framework_include_paths}"],
                    ),
                ],
            ),
            flag_set(
                actions = all_assemble_actions,
                flag_groups = [
                    flag_group(
                        iterate_over = "quote_include_paths",
                        flags = ["/I", "%{quote_include_paths}"],
                    ),
                    flag_group(
                        iterate_over = "include_paths",
                        flags = ["/I", "{include_paths}"],
                    ),
                    flag_group(
                        iterate_over = "system_include_paths",
                        flags = ["/I", "%{system_include_paths}"],
                    ),
                ],
            ),
        ],
    )

    preprocessor_defines_feature = feature(
        name = "preprocessor_defines",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        iterate_over = "preprocessor_defines",
                        flags = ["-D%{preprocessor_defines}"],
                    ),
                ],
            ),
            flag_set(
                actions = all_assemble_actions,
                flag_groups = [
                    flag_group(
                        iterate_over = "preprocessor_defines",
                        flags = ["/D", "%{preprocessor_defines}"],
                    ),
                ],
            ),
        ],
    )

    compiler_input_flags_feature = feature(
        name = "compiler_input_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        expand_if_available = "source_file",
                        flags = ["-c", "%{source_file}"],
                    ),
                ],
            ),
            flag_set(
                actions = all_assemble_actions,
                flag_groups = [
                    flag_group(
                        expand_if_available = "source_file",
                        flags = ["/c", "%{source_file}"],
                    ),
                ],
            ),
        ],
    )

    compiler_output_flags_feature = feature(
        name = "compiler_output_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        expand_if_available = "output_assembly_file",
                        flags = ["-S"],
                    ),
                    flag_group(
                        expand_if_available = "output_preprocess_file",
                        flags = ["-E"],
                    ),
                    flag_group(
                        expand_if_available = "output_file",
                        flags = ["-o", "%{output_file}"],
                    ),
                ],
            ),
            flag_set(
                actions = all_assemble_actions,
                flag_groups = [
                    flag_group(
                        expand_if_available = "output_file",
                        flags = ["/Fo", "%{output_file}"],
                    ),
                ],
            ),
        ],
    )

    # Special Bazel feature whose default adds -Wl,-S during linking. Since this is not
    # supported by lld-link for windows, we redefine the feature to have zero flags.
    strip_debug_symbols_feature = feature(
        name = "strip_debug_symbols",
        flag_sets = [
            flag_set(
                actions = all_link_actions,
            ),
        ],
    )

    assemble_action = action_config(
        action_name = ACTION_NAMES.assemble,
        tools = [tool(tool = ctx.executable._ml_tool)],
    )

    # Use windows artifact names.
    artifact_name_patterns = [
        artifact_name_pattern(
            category_name = "object_file",
            prefix = "",
            extension = ".obj",
        ),
        artifact_name_pattern(
            category_name = "static_library",
            prefix = "",
            extension = ".lib",
        ),
        artifact_name_pattern(
            category_name = "alwayslink_static_library",
            prefix = "",
            extension = ".lo.lib",
        ),
        artifact_name_pattern(
            category_name = "executable",
            prefix = "",
            extension = ".exe",
        ),
        artifact_name_pattern(
            category_name = "dynamic_library",
            prefix = "",
            extension = ".dll",
        ),
        artifact_name_pattern(
            category_name = "interface_library",
            prefix = "",
            extension = ".if.lib",
        ),
    ]

    return cc_common.create_cc_toolchain_config_info(
        action_configs = [assemble_action, strip_action],
        ctx = ctx,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        toolchain_identifier = "cc-compiler-x64_windows",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = "x64_windows",
        target_libc = "msvcrt",
        compiler = "compiler",
        abi_version = "local",
        abi_libc_version = "local",
        tool_paths = tool_paths,
        artifact_name_patterns = artifact_name_patterns,
        features = [
            opt_feature,
            fastbuild_feature,
            dbg_feature,
            include_paths_feature,
            compiler_input_flags_feature,
            compiler_output_flags_feature,
            preprocessor_defines_feature,
            supports_fission_feature,
            supports_gold_linker_feature,
            supports_incremental_linker_feature,
            supports_interface_shared_objects_feature,
            supports_normalizing_ar_feature,
            supports_start_end_lib_feature,
            default_compile_flags_feature,
            default_env_feature,
            default_link_flags_feature,
            libraries_to_link_feature,
            deterministic_compilation_feature,
            toolchain_include_directories_feature,
            build_type_feature,
            winerun_feature,
            strip_debug_symbols_feature,
        ],
    )

_windows_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "workspace_env": attr.label(default = "//bzl/crosstool:workspace-env-for-clang-windows-wrapper", allow_single_file = True, cfg = "exec"),
        "wine64": attr.label(default = "@local-wine64//:wine64", allow_single_file = True, executable = True, cfg = "exec"),
        "msvc": attr.label(mandatory = True, cfg = "exec"),
        "_ml_tool": attr.label(default = "//bzl/crosstool:ml-windows-wrapper", executable = True, cfg = "exec"),
        "strip_tool": attr.label(default = "//bzl/crosstool:strip-windows-wrapper", executable = True, cfg = "exec"),
        "clang_wrapper": attr.label(default = "//bzl/crosstool:clang-windows-wrapper", executable = True, cfg = "exec"),
        "ar_wrapper": attr.label(default = "//bzl/crosstool:ar-windows-wrapper", executable = True, cfg = "exec"),
    },
    toolchains = [
        "//bzl/llvm:toolchain_type",
    ],
    provides = [CcToolchainConfigInfo],
)

def windows_cc_toolchain(name, wine64, msvc, strip_tool, **kwargs):
    """Macro to create a bazel cc_toolchain that uses targets Windows."""
    config_name = "{}-config".format(name)
    _windows_cc_toolchain_config(
        name = config_name,
        wine64 = wine64,
        msvc = msvc,
        strip_tool = strip_tool,
    )
    native.cc_toolchain(
        name = name,
        toolchain_config = config_name,
        **kwargs
    )
