"""module
Toolchain configuration file for v2-linux
Info at https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/l/cp/GfmbLe0S
"""

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
load("//bzl/cpu-microarch:cpu-microarch-allowed_target.bzl", "ALLOWED_MARCH_TARGETS")

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

# Compilation actions for c++ only.
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
    target = "x86_64-niantic_v2.0-linux-gnu"
    toolchain = "BAZEL_OUTPUT_BASE/%s" % ctx.attr.linux.label.workspace_root
    sysroot = "%s/%s/%s" % (toolchain, target, target)
    origin = "$ORIGIN"

    # Toolchain headers live outside the sandbox, so we have to add them to cxx_builtin_include_directories
    # The absolute path to these headers varies across computers, but we want to have good
    # build cache behavior, so we normalize the absolute paths with a placeholder
    # string in bazel that is filled in during the build-scripts with the absolute location.

    # Bazel will check for all the geneated header dependency files to be sure that he track all of them
    # Unfortunately with cross compilation toolchain he cannot track the toolchain headers, because those are outside
    # the compilation sandbox (https://bit.ly/3XSxvG8) so we are telling to the bazel to not complain for headers that
    # are in this location. Not because bazel cache would complain with path in different cache, we have to uniform this.
    # So we put the path relative to a fictitious value to deceive bazel header that we replace in the clang-v*-linux-wrapper.sh
    # clang-v*-linux-wrapper.sh script
    cxx_builtin_include_directories = [
        "/{{deceive_bazel_header_deps}}/external/v2-linux/x86_64-niantic_v2.0-linux-gnu/x86_64-niantic_v2.0-linux-gnu/sysroot/usr/include/",
        "/{{deceive_bazel_header_deps}}/external/v2-linux/x86_64-niantic_v2.0-linux-gnu/x86_64-niantic_v2.0-linux-gnu/include/c++/11.2.0",
        "/{{deceive_bazel_header_deps}}/external/v2-linux/x86_64-niantic_v2.0-linux-gnu/include",
        "/{{deceive_bazel_header_deps}}/external/v2-linux/x86_64-niantic_v2.0-linux-gnu/lib/gcc/x86_64-niantic_v2.0-linux-gnu/11.2.0/include",
        "/{{deceive_bazel_header_deps}}/external/v2-linux/x86_64-niantic_v2.0-linux-gnu/lib/gcc/x86_64-niantic_v2.0-linux-gnu/11.2.0/include-fixed",
        "/{{deceive_bazel_header_deps}}/external/v2-linux/x86_64-niantic_v2.0-linux-gnu/lib/gcc/x86_64-niantic_v2.0-linux-gnu/11.2.0/install-tools/include",
        "/{{{{deceive_bazel_header_deps}}}}/{runtime}".format(runtime = llvminfo.runtime),
        "/{{deceive_bazel_header_deps}}/external/v1-cuda-triplet",
    ]

    # Define the tools that are used in compilation and linking. Practically, only "gcc" and "ar",
    # i.e., compiling/linking and static linking will actually be used.
    tool_paths = [
        tool_path(name = "gcc", path = "clang-v2-linux-wrapper.sh"),
        tool_path(name = "ar", path = "ar-linux-wrapper.sh"),
        tool_path(name = "cpp", path = "/usr/bin/false"),  # unused
        tool_path(name = "dwp", path = "/usr/bin/false"),  # unused
        tool_path(name = "gcov", path = "/usr/bin/false"),  # unused
        tool_path(name = "ld", path = "/usr/bin/false"),  # unused
        tool_path(name = "nm", path = "/usr/bin/false"),  # unused
        tool_path(name = "objcopy", path = "/usr/bin/false"),  # unused
        tool_path(name = "objdump", path = "/usr/bin/false"),  # unused
        tool_path(name = "strip", path = "/usr/bin/false"),  # Unused, see strip_action below.
        tool_path(name = "nvcc", path = "/usr/bin/false"),  # Unused, see strip_action below.
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
                            "%s/sysroot/usr/include" % sysroot,
                            "-isystem",
                            "%s/include/c++/11.2.0" % sysroot,
                            "-isystem",
                            "%s/include/c++/11.2.0/%s" % (sysroot, target),
                            "-isystem",
                            "BAZEL_OUTPUT_BASE/%s/include" % llvminfo.runtime,
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
                flag_groups = [flag_group(flags = [
                    "-g2",
                    "-O0",
                    "-D_DEBUG",
                ])],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-g2",
                    "-O3",
                    "-DNDEBUG",
                ])],
                with_features = [with_feature_set(not_features = ["dbg"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            # Build with LTO and other flags to optimize for size and performance.
                            "-g2",
                            "-O3",
                            "-fvisibility=hidden",
                            "-fdata-sections",
                            "-ffunction-sections",
                            "-flto",  # LTO needs to be in both compiling and linking flags
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    default_compile_cpp_flags_list = [
        "-std=c++20",
        "-fcxx-exceptions",
    ]

    march = ctx.attr.cpu_microarch.label.name
    if march in ALLOWED_MARCH_TARGETS:
        default_compile_cpp_flags_list.append("-march=%s" % march)

    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_cpp_compile_actions,
                flag_groups = [flag_group(flags = default_compile_cpp_flags_list)],
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
                    "--sysroot=%s/sysroot" % sysroot,
                ])],
            ),
            flag_set(
                actions = all_cpp_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-mcrc32",
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
                            "-L%s/lib64" % sysroot,
                            "-L%s/lib" % sysroot,
                            "-L%s/../lib/gcc/x86_64-niantic_v2.0-linux-gnu/11.2.0" % sysroot,
                            "-stdlib=libstdc++",
                            "-lstdc++",
                            "-lm",
                            "-Wl,--whole-archive",
                            "-Wl,-rpath=%s" % origin,
                            "-Wl,-rpath=%s/lib64" % sysroot,
                            "-Wl,-rpath=%s/lib" % sysroot,
                            "-Wl,--no-whole-archive",
                            "--sysroot=%s/sysroot" % sysroot,
                            "-lpthread",
                            "-ldl",
                            "--unwindlib=libgcc",
                            "-LBAZEL_OUTPUT_BASE/%s/lib/x86_64-unknown-linux-gnu" % llvminfo.runtime,
                        ],
                    ),
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
        host_system_name = "x86_64-niantic_v2.0-linux-gnu",
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
        ],
    )

_v2_linux_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "linux": attr.label(mandatory = True),
        "strip_tool": attr.label(default = "//bzl/crosstool:strip-v2-linux-wrapper", executable = True, cfg = "exec"),
        "cpu_microarch": attr.label(mandatory = True),
    },
    toolchains = [
        "//bzl/llvm:toolchain_type",
    ],
    provides = [CcToolchainConfigInfo],
)

def v2_linux_cc_toolchain(name, linux, cpu_microarch, tools, **kwargs):
    """Macro to create a bazel cc_toolchain that targets V2 Linux machines."""
    config_name = "{}-config".format(name)
    _v2_linux_cc_toolchain_config(
        name = config_name,
        linux = linux,
        cpu_microarch = cpu_microarch,
    )
    native.cc_toolchain(
        name = name,
        toolchain_config = config_name,
        strip_files = tools,
        **kwargs
    )
