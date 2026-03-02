"""
The Android CC toolchain.
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
load("//bzl/android:android-version.bzl", "ANDROID_NDK_VERSION")
load("//bzl/crosstool:toolchain-compiler-flags.bzl", "DBG_COMPILER_FLAGS", "FASTBUILD_COMPILER_FLAGS", "OPT_WITH_SYMBOLS_COMPILER_FLAGS")

NDK_RELATIVE = "external/androidsdk/ndk/{version}".format(version = ANDROID_NDK_VERSION)
LLVM_VERSION = "14.0.1"

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

all_cpp_compile_actions = [
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.clif_match,
]

preprocessor_compile_actions = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.clif_match,
]

codegen_compile_actions = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.assemble,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.cpp_module_codegen,
    ACTION_NAMES.lto_backend,
]

all_link_actions = [
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]

all_archiver_actions = [
    ACTION_NAMES.cpp_link_static_library,
]

# Valid NDK triples as defined here:
# https://developer.android.com/ndk/guides/other_build_systems
ANDROID_TARGET_TRIPLES = {
    "armeabi-v7a": "armv7a-linux-androideabi",
    "arm64-v8a": "aarch64-linux-android",
    "x86": "i686-linux-android",
    "x86_64": "x86_64-linux-android",
}

def _impl(ctx):
    # Validate target attribute.
    if ctx.attr.target not in ANDROID_TARGET_TRIPLES.keys():
        fail("target must be one of ({})".format(ANDROID_TARGET_TRIPLES.keys()))

    llvm_prebuilt_relative = "{ndk}/toolchains/llvm/prebuilt/{exec_prebuilt_dir}".format(ndk = NDK_RELATIVE, exec_prebuilt_dir = ctx.attr.exec_prebuilt_dir)
    compiler_rt = "{llvm}/lib64/clang/{llvm_ver}".format(llvm = llvm_prebuilt_relative, llvm_ver = LLVM_VERSION)
    target_triple = ANDROID_TARGET_TRIPLES[ctx.attr.target]

    # From NDK documentation: "For 32-bit ARM, the compiler is prefixed with
    # armv7a-linux-androideabi, but the binutils tools are prefixed with
    # arm-linux-androideabi. For other architectures, the prefixes are the same
    # for all tools."
    binutils = "arm-linux-androideabi" if target_triple == "armv7a-linux-androideabi" else target_triple

    # Directories which the compiler will automatically include.
    cxx_builtin_include_directories = [
        "/{{{{toolchain}}}}/{llvm}/sysroot/usr".format(llvm = llvm_prebuilt_relative),
        "/{{{{toolchain}}}}/{compiler_rt}/include".format(compiler_rt = compiler_rt),
        "/{{{{toolchain}}}}/{compiler_rt}/share".format(compiler_rt = compiler_rt),
        "/{{{{toolchain}}}}/{ndk}/sources/android/support/include".format(ndk = NDK_RELATIVE),
    ]

    # Define the tools that are used in compilation and linking. Practically,
    # only "gcc" and "ar", i.e., compiling/linking and static linking will
    # actually be used.
    tool_paths = [
        tool_path(name = "ar", path = "ar-android-wrapper.sh"),
        tool_path(name = "gcc", path = "clang-android-wrapper.sh"),
        tool_path(name = "cpp", path = "%s/bin/clang-cpp" % llvm_prebuilt_relative),
        tool_path(name = "dwp", path = "%s/bin/llvm-dwp" % llvm_prebuilt_relative),
        tool_path(name = "gcov", path = "%s/bin/llvm-cov" % llvm_prebuilt_relative),
        tool_path(name = "ld", path = "%s/bin/lld" % llvm_prebuilt_relative),
        tool_path(name = "nm", path = "%s/bin/llvm-nm" % llvm_prebuilt_relative),
        tool_path(name = "objcopy", path = "/usr/bin/objcopy"),
        tool_path(name = "objdump", path = "%s/bin/llvm-objdump" % llvm_prebuilt_relative),
        tool_path(name = "strip", path = "/usr/bin/strip"),  # Unused, see strip_action below.
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

    # Add target triple and toolchain. This specifies the arch (armv7/arm64),
    # the vendor (linux), the platform (android), and the minimum OS version.
    target_arch_flags = [
        "-target",
        "{target}{min_version}".format(target = target_triple, min_version = ctx.attr.min_version),
    ]

    default_compile_flag_groups = [
        flag_group(flags = target_arch_flags),
        flag_group(flags = [
            "-Wno-invalid-command-line-argument",
            "-Wno-unused-command-line-argument",
            "-ffunction-sections",
            "-funwind-tables",
            "-fstack-protector-strong",
            "-no-canonical-prefixes",
            "-fsigned-char",
            "-DANDROID",
            "-D__BIONIC__",
            "-fcolor-diagnostics",
        ]),
    ]

    default_link_flag_groups = [
        flag_group(flags = target_arch_flags),
        flag_group(flags = [
            "-fuse-ld=lld",
            "-L{ndk}/sources/cxx-stl/llvm-libc++/libs/{arch}".format(ndk = NDK_RELATIVE, arch = ctx.attr.target),
            "-L{llvm}/sysroot/usr/lib/{binutils}/{version}".format(llvm = llvm_prebuilt_relative, binutils = binutils, version = ctx.attr.min_version),
            "-lm",
            "-ldl",
            "-stdlib=libc++",
            "-lc++abi",
            "-lc++_static",
            "-Wl,--build-id",  # enables the debugger to find symbols
            "-fcolor-diagnostics",
        ]),
    ]

    if ctx.attr.target == "armeabi-v7a":
        default_compile_flag_groups.append(
            flag_group(flags = [
                "-mthumb",
                "-march=armv7-a",
                "-mfloat-abi=softfp",
                "-mfpu=neon",
            ]),
        )
        default_link_flag_groups.append(
            flag_group(flags = [
                "-Wl,--fix-cortex-a8",
            ]),
        )

    full_lto_feature = feature(
        name = "full_lto",
        enabled = True,
        flag_sets = [flag_set(
            actions = all_compile_actions + all_link_actions,
            flag_groups = [
                flag_group(
                    flags = [
                        # Link with LTO in opt builds.
                        "-flto",
                    ],
                ),
            ],
            with_features = [with_feature_set(features = ["opt"])],
        )],
    )

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    # All link actions except the 'cpp_link_nodeps_dynamic_library' action which links without dependencies.
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                ],
                flag_groups = [flag_group(flags = [
                    # Require all symbols when linking final binaries and shared libraries.
                    "--no-undefined",
                ])],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = default_link_flag_groups,
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            # Link with --gc-sections to optimize for size in opt build.
                            "-Wl,--gc-sections",  # Strip dead code. Reduces size with with full_lto.
                            # Specify 16kb page size. Note that NDK version r28 and higher compiles
                            # with this by default, so we can remove this in the future. At the time
                            # of writing, the LTS NDK version is r27, so we choose to use this flag
                            # instead of updating NDK. For more information, see:
                            # - https://developer.android.com/guide/practices/page-sizes
                            # - https://android-developers.googleblog.com/2024/08/adding-16-kb-page-size-to-android.html
                            "-Wl,-z,max-page-size=16384",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    # Environment variables to set for compile and link actions.
    default_env_feature = feature(
        name = "default_env_feature",
        enabled = True,
        env_sets = [
            env_set(
                actions = all_compile_actions + all_link_actions + all_archiver_actions,
                env_entries = [
                    env_entry("LLVM_PREBUILT", llvm_prebuilt_relative),
                    env_entry("WORKSPACE_ENV", ctx.file.workspace_env.path),
                ],
            ),
            env_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                env_entries = [
                    env_entry("ADB", ctx.file.adb.path),
                ],
            ),
        ],
    )

    opt_feature = feature(name = "opt")
    fastbuild_feature = feature(name = "fastbuild")
    dbg_feature = feature(name = "dbg")
    static_link_cpp_runtimes_feature = feature(
        name = "static_link_cpp_runtimes",
        enabled = True,
    )
    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)
    supports_pic_feature = feature(name = "supports_pic", enabled = True)

    pic_feature = feature(
        name = "pic",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.assemble,
                    ACTION_NAMES.preprocess_assemble,
                    ACTION_NAMES.linkstamp_compile,
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_module_codegen,
                    ACTION_NAMES.cpp_module_compile,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["-fPIC"],
                        # Always expand here, rather than require --force_pic.
                        # expand_if_available = "pic",
                    ),
                ],
            ),
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.lto_index_for_executable,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["-fPIE", "-pie"],
                        # Always expand here, rather than require --force_pic.
                        # expand_if_available = "pic",
                    ),
                ],
            ),
        ],
    )

    # Instead of a raw executable binary, build a shell script that uploads the
    # binary to a device through adb and executes the binary on that device.
    adbrun_feature = feature(
        name = "adbrun",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                ],
                flag_groups = [flag_group(flags = ["--run-with-adb"])],
            ),
        ],
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
                    "-stdlib=libc++",
                ])],
            ),
            flag_set(
                actions = [ACTION_NAMES.c_compile],
                flag_groups = [flag_group(flags = [
                    "-std=c11",
                    # "-pedantic-errors", # Enable this will fail the libarchive build
                ])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = default_compile_flag_groups,
            ),
        ],
    )

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        toolchain_identifier = ctx.attr.name,
        host_system_name = "local",
        target_system_name = ctx.attr.target,
        target_cpu = ctx.attr.target,
        target_libc = "local",
        compiler = "clang",
        abi_version = ctx.attr.target,
        abi_libc_version = "local",
        tool_paths = tool_paths,
        action_configs = [strip_action],
        features = [
            opt_feature,
            fastbuild_feature,
            dbg_feature,
            default_env_feature,
            no_canonical_prefixes_feature,
            default_compile_flags_feature,
            deterministic_compilation_feature,
            build_type_feature,
            default_link_flags_feature,
            full_lto_feature,
            static_link_cpp_runtimes_feature,
            supports_dynamic_linker_feature,
            supports_pic_feature,
            pic_feature,
            adbrun_feature,
        ],
    )

_android_cc_toolchain_config = rule(
    provides = [CcToolchainConfigInfo],
    attrs = {
        "target": attr.string(mandatory = True),
        "min_version": attr.string(mandatory = True),
        "exec_prebuilt_dir": attr.string(mandatory = True),
        "adb": attr.label(mandatory = True, allow_single_file = True),
        "workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "strip_tool": attr.label(default = "//bzl/crosstool:strip-android-wrapper", executable = True, cfg = "exec"),
    },
    implementation = _impl,
)

def android_cc_toolchain(name, target, min_version, adb, exec_prebuilt_dir, **kwargs):
    """Macro to create a bazel cc_toolchain that uses the Android NDK."""
    config_name = "{}-config".format(name)
    _android_cc_toolchain_config(
        name = config_name,
        target = target,
        min_version = min_version,
        exec_prebuilt_dir = exec_prebuilt_dir,
        adb = adb,
    )
    native.cc_toolchain(
        name = name,
        toolchain_config = config_name,
        dynamic_runtime_lib = "//bzl/android:{arch}-dynamic-runtime-libraries".format(arch = target, name = name),
        static_runtime_lib = "//bzl/android:{arch}-static-runtime-libraries".format(arch = target, name = name),
        **kwargs
    )

def android_toolchain(name, platform, tools, adb, exec_os, min_version, target, exec_compatible_with, target_compatible_with):
    """
    Bazel Macro to create a cc_toolchain and toolchain for an Android toolchain.

    Args:
        name: The name of the toolchain.
        platform: The platform of the toolchain.
        tools: The tools of the toolchain.
        adb: The adb tool.
        exec_os: The operating system of the toolchain.
        min_version: The minimum version of the toolchain.
        target: The target of the toolchain.
        exec_compatible_with: The compatible with of the toolchain.
        target_compatible_with: The compatible with of the toolchain.
    """
    PREBUILT_MAP = {
        "osx": "darwin-x86_64",
        "windows": "windows-x86_64",
        "linux": "linux-x86_64",
    }
    if exec_os not in PREBUILT_MAP.keys():
        fail("exec_os for {} must be one of ({})".format(name, PREBUILT_MAP.keys()))

    prebuilt_dir = PREBUILT_MAP[exec_os]

    cc_toolchain_name = "cc-compiler-{platform}-exec-{exec_os}".format(platform = platform, exec_os = exec_os)
    toolchain_name = "cc-toolchain-{platform}-exec-{exec_os}".format(platform = platform, exec_os = exec_os)

    android_cc_toolchain(
        name = cc_toolchain_name,
        adb = adb,
        all_files = tools,
        ar_files = tools,
        compiler_files = tools,
        dwp_files = ":empty",
        exec_prebuilt_dir = prebuilt_dir,
        linker_files = tools,
        min_version = min_version,
        objcopy_files = ":empty",
        strip_files = tools,
        supports_param_files = 0,
        target = target,
    )

    native.toolchain(
        name = toolchain_name,
        exec_compatible_with = exec_compatible_with,
        target_compatible_with = target_compatible_with,
        toolchain = cc_toolchain_name,
        toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
    )
