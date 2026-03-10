load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "action_config",
    "env_entry",
    "env_set",
    "feature",
    "flag_group",
    "flag_set",
    "tool",
    "tool_path",
    "with_feature_set",
)
load("//bzl/crosstool:toolchain-compiler-flags.bzl", "DBG_COMPILER_FLAGS", "FASTBUILD_COMPILER_FLAGS", "OPT_WITH_SYMBOLS_COMPILER_FLAGS")

# All compilation actions for c/c++.
all_compile_actions = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.assemble,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.clif_match,
    ACTION_NAMES.lto_backend,
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
    llvminfo = ctx.toolchains["//bzl/llvm:toolchain_type"].llvminfo
    target = "x86_64-redhat-linux"
    toolchain = "BAZEL_OUTPUT_BASE/%s" % ctx.attr.linux.label.workspace_root
    sysroot = "%s/%s/sysroot" % (toolchain, target)

    cxx_builtin_include_directories = [
        "/{{toolchain}}/external/amazonlinux/x86_64-redhat-linux/sysroot/local/include",
        "/{{toolchain}}/external/amazonlinux/x86_64-redhat-linux/sysroot/local/include/c++/v1",
        "/{{toolchain}}/external/amazonlinux/x86_64-redhat-linux/sysroot/include",
        "/{{{{toolchain}}}}/{runtime}".format(runtime = llvminfo.runtime),
    ]

    # Define the tools that are used in compilation and linking. Practically, only "gcc" and "ar",
    # i.e., compiling/linking and static linking will actually be used.
    tool_paths = [
        tool_path(name = "gcc", path = "clang-amazonlinux-wrapper.sh"),
        tool_path(name = "ar", path = "ar-linux-wrapper.sh"),
        tool_path(name = "cpp", path = "/usr/bin/false"),  # unused
        tool_path(name = "dwp", path = "/usr/bin/false"),  # unused
        tool_path(name = "gcov", path = "/usr/bin/false"),  # unused
        tool_path(name = "ld", path = "/usr/bin/false"),  # unused
        tool_path(name = "nm", path = "/usr/bin/false"),  # unused
        tool_path(name = "objcopy", path = "/usr/bin/false"),  # unused
        tool_path(name = "objdump", path = "/usr/bin/false"),  # unused
        tool_path(name = "strip", path = "/usr/bin/false"),  # Unused, see strip_action below.
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

    toolchain_include_directories_feature = feature(
        name = "toolchain_include_directories",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-nostdinc",
                            "-isystem",
                            "%s/local/include" % sysroot,
                            "-isystem",
                            "%s/local/include/c++/v1" % sysroot,
                            "-isystem",
                            "BAZEL_OUTPUT_BASE/%s/include" % llvminfo.runtime,
                            "-isystem",
                            "%s/include" % sysroot,
                        ],
                    ),
                ],
            ),
        ],
    )

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

    no_canonical_prefixes_feature = feature(
        name = "no_canonical_prefixes",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions + all_link_actions,
                flag_groups = [flag_group(flags = ["-no-canonical-prefixes"])],
            ),
        ],
    )

    opt_feature = feature(name = "opt")
    fastbuild_feature = feature(name = "fastbuild")
    dbg_feature = feature(name = "dbg")

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
                flag_groups = [flag_group(flags = OPT_WITH_SYMBOLS_COMPILER_FLAGS)],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_cpp_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-std=c++20",
                    "-fcxx-exceptions",
                ])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-fPIC",
                    "-target",
                    target,
                    "--gcc-toolchain=%s" % toolchain,
                    "-fexceptions",
                    "-fno-threadsafe-statics",
                    "-Wall",  # All warnings are enabled.
                    "-Wthread-safety",  # Enable additional warnings not part of -Wall.
                    "-Wself-assign",  # Enable additional warnings not part of -Wall.
                    "-fno-omit-frame-pointer",  # Keep stack frames for debugging, even in opt mode.
                    "-fcolor-diagnostics",  # Enable coloring.
                    "-DLINUX",
                    "-DC8_HAS_X11",
                    "--sysroot=%s" % sysroot,
                ])],
            ),
        ],
    )
    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-target",
                            target,
                            "-fuse-ld=lld",
                            "-L%s/local/lib64" % sysroot,
                            "-L%s/local/lib" % sysroot,
                            "-L%s/lib64" % sysroot,
                            "-L%s/lib" % sysroot,
                            "-L%s/lib/gcc/x86_64-redhat-linux/7" % sysroot,
                            "-stdlib=libc++",
                            "-lc++abi",
                            "-lc++",
                            "-lm",
                            "-Wl,--whole-archive",
                            "-lpthread",
                            "-Wl,--no-whole-archive",
                            "--sysroot=%s" % sysroot,
                        ],
                    ),
                ],
            ),
        ],
    )

    dockerrun_feature = feature(
        name = "dockerrun",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                ],
                flag_groups = [flag_group(flags = ["-dockerrun"])],
            ),
        ],
    )

    # TODO(Nathan): get xvfb working.
    xvfb_feature = feature(
        name = "xvfb",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                ],
                flag_groups = [flag_group(flags = ["--xvfb"])],
            ),
        ],
    )

    # Pass in the llvm toolchain path as an environment variable.
    default_env_feature = feature(
        name = "default_env",
        enabled = True,
        env_sets = [
            env_set(
                actions = all_compile_actions + all_link_actions + all_archiver_actions,
                env_entries = [
                    env_entry("LLVM_USR", llvminfo.usr),
                    env_entry("WORKSPACE_ENV", ctx.file.workspace_env.path),
                ],
            ),
        ],
    )

    # Directly copied over from previous linux.
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        toolchain_identifier = "cc-compiler-linux",
        host_system_name = "i686-unknown-linux-gnu",
        target_system_name = target,
        target_cpu = "x86_64",
        target_libc = "glibc",
        compiler = "llvm",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        action_configs = [strip_action],
        features = [
            opt_feature,
            fastbuild_feature,
            dbg_feature,
            no_canonical_prefixes_feature,
            default_compile_flags_feature,
            default_link_flags_feature,
            deterministic_compilation_feature,
            build_type_feature,
            default_env_feature,
            toolchain_include_directories_feature,
            dockerrun_feature,
            xvfb_feature,
        ],
    )

_amazonlinux_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "linux": attr.label(mandatory = True),
        "strip_tool": attr.label(default = "//bzl/crosstool:strip-amazonlinux-wrapper", executable = True, cfg = "exec"),
    },
    toolchains = [
        "//bzl/llvm:toolchain_type",
    ],
    provides = [CcToolchainConfigInfo],
)

def amazonlinux_cc_toolchain(name, linux, tools, **kwargs):
    """Macro to create a bazel cc_toolchain that targets Amazon Linux machines."""
    config_name = "{}-config".format(name)
    _amazonlinux_cc_toolchain_config(
        name = config_name,
        linux = linux,
    )
    native.cc_toolchain(
        name = name,
        toolchain_config = config_name,
        strip_files = tools,
        **kwargs
    )
