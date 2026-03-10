load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "env_entry",
    "env_set",
    "feature",
    "flag_group",
    "flag_set",
    "tool_path",
    "with_feature_set",
)
load("//bzl/crosstool:toolchain-compiler-flags.bzl", "DBG_COMPILER_FLAGS", "FASTBUILD_COMPILER_FLAGS", "OPT_WITHOUT_SYMBOLS_COMPILER_FLAGS")

LLVM_USR = "external/emscripten/upstream"

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

def _impl(ctx):
    cxx_builtin_include_directories = [
        "/{{toolchain}}/external/emscripten-cache/cache/sysroot/include",
        "/{{toolchain}}/external/emscripten-cache/cache/sysroot/include/libcxx",
        "/{{toolchain}}/external/emscripten-cache/cache/sysroot/include/libc",
        "/{{toolchain}}/external/emscripten-cache/cache/sysroot/lib/libcxxabi/include",
        "/{{toolchain}}/external/emscripten-cache/cache/sysroot/lib/libc/musl/arch/emscripten",
        "/{{toolchain}}/external/emscripten/upstream/lib/clang/16.0.0/include",
        "/{{toolchain}}/external/emscripten/upstream/lib/clang/16.0.0/share",
    ]

    tool_paths = [
        tool_path(name = "ar", path = "emar-wrapper.sh"),
        tool_path(name = "cpp", path = "emscripten/clang/current/clang-cpp"),
        tool_path(name = "dwp", path = "emscripten/clang/current/llvm-dwp"),
        tool_path(name = "gcc", path = "emcc-wrapper.sh"),
        tool_path(name = "gcov", path = "emscripten/clang/current/llvm-cov"),
        tool_path(name = "ld", path = "emscripten/clang/current/lld"),
        tool_path(name = "nm", path = "emscripten/clang/current/llvm-nm"),
        tool_path(name = "objcopy", path = "/usr/bin/objcopy"),
        tool_path(name = "objdump", path = "emscripten/clang/current/llvm-objdump"),
        tool_path(name = "strip", path = "/usr/bin/strip"),
    ]

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

    nodejs_feature = feature(
        # Enabled with --features=nodejs. Create a shell-script to run the wasm binary in node.
        name = "nodejs",
        env_sets = [
            env_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                env_entries = [
                    env_entry("RUN_WRAPPER", "nodejs"),
                ],
            ),
        ],
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                flag_groups = [
                    flag_group(flags = ctx.attr._flags[BuildSettingInfo].value),
                    flag_group(flags = [
                        "-sNODERAWFS",
                        "-sSINGLE_FILE=1",
                    ]),
                ],
            ),
        ],
    )

    emrum_feature = feature(
        # Enabled with --features=emrun. Create a shell-script to run the wasm binary through emrun.
        name = "emrun",
        env_sets = [
            env_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                env_entries = [
                    env_entry("RUN_WRAPPER", "emrun"),
                    env_entry("BROWSER", ctx.attr._browser[BuildSettingInfo].value),
                ],
            ),
        ],
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                flag_groups = [
                    flag_group(flags = ctx.attr._flags[BuildSettingInfo].value),
                    flag_group(flags = [
                        "--emrun",
                        "-sSINGLE_FILE=1",
                    ]),
                ],
            ),
        ],
    )

    simd_feature = feature(
        name = "simd",
        # Enabled with --features=simd for SIMD builds.
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(
                    flags = ["-msimd128", "-DC8_WASM_SIMD128"],
                )],
            ),
        ],
    )

    pthread_feature = feature(
        name = "pthread",
        # Enabled with --features=pthread for threading builds.
        flag_sets = [
            # https://emscripten.org/docs/porting/pthreads.html#compiling-with-pthreads-enabled
            flag_set(
                actions = all_compile_actions + all_link_actions,
                flag_groups = [flag_group(
                    flags = ["-pthread"],
                )],
            ),
        ],
        env_sets = [
            env_set(
                actions = all_compile_actions + all_link_actions,
                env_entries = [
                    env_entry("NIA_WASM_PTHREAD", "1"),
                ],
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
                    env_entry("LLVM_TOOLCHAIN", LLVM_USR),
                    env_entry("EMSCRIPTEN_TOOLCHAIN", (ctx.attr.toolchain.label.workspace_root)),
                    env_entry("WORKSPACE_ENV", ctx.file._workspace_env.path),
                ],
            ),
        ],
    )

    opt_feature = feature(name = "opt")
    fastbuild_feature = feature(name = "fastbuild")
    dbg_feature = feature(name = "dbg")

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

    build_type_feature = feature(
        name = "build_type",
        enabled = True,
        flag_sets = [
            flag_set(
                # In '-c dbg' mode build with symbols, source-maps and a _DEBUG macro.
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = DBG_COMPILER_FLAGS)],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [flag_group(flags = [
                    "-gsource-map",
                ])],
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
            # TODO(mc): Confirm whether -Wl,--gc-sections can be enabled for wasm builds to
            # remove dead code following link-time optimization.
            # flag_set(
            #    # Link with --gc-sections to reduce size in opt build.
            #    actions = all_link_actions,
            #    flag_groups = [flag_group(flags = [
            #        "-Wl,--gc-sections",  # Strip dead code. Reduces size with with full_lto.
            #    ])],
            #    with_features = [with_feature_set(features = ["opt"])],
            # ),
            flag_set(
                # Ignore warning about limited postlink optimizations for non-release builds.
                actions = all_compile_actions + all_link_actions,
                flag_groups = [flag_group(flags = ["-Wno-limited-postlink-optimizations"])],
                with_features = [with_feature_set(not_features = ["opt"])],
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
                ])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-DJAVASCRIPT",
                    "-DC8_NO_EXCEPT",
                    "-DKJ_NO_EXCEPTIONS",
                    "-fno-exceptions",
                    "-fno-threadsafe-statics",
                    "-Wall",  # All warnings are enabled.
                    "-Wthread-safety",  # Enable additional warnings not part of -Wall.
                    "-Wself-assign",  # Enable additional warnings not part of -Wall.
                    "-fno-omit-frame-pointer",  # Keep stack frames for debugging, even in opt mode.
                    "-fcolor-diagnostics",  # Enable coloring.
                ])],
            ),
        ],
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
                            "BAZEL_OUTPUT_BASE/external/emscripten-cache/cache/sysroot/include/SDL",
                            "-isystem",
                            "BAZEL_OUTPUT_BASE/external/emscripten-cache/cache/sysroot/include/compat",
                            "-isystem",
                            "BAZEL_OUTPUT_BASE/external/emscripten-cache/cache/sysroot/include/c++/v1",
                            "-isystem",
                            "BAZEL_OUTPUT_BASE/external/emscripten/upstream/lib/clang/16.0.0/include",
                            "-isystem",
                            "BAZEL_OUTPUT_BASE/external/emscripten-cache/cache/sysroot/include",
                        ],
                    ),
                ],
            ),
        ],
    )

    # Define this default feature as empty to prevent the crosstool from adding
    # '-rpath' specifiers that wasm-ld doesn't understand. We static link everything.
    runtime_library_search_directories_feature = feature(
        name = "runtime_library_search_directories",
    )

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        toolchain_identifier = "cc-compiler-js",
        host_system_name = "i686-unknown-linux-gnu",
        target_system_name = "wasm-unknown-emscripten",
        target_cpu = "wasm",
        target_libc = "unknown",
        compiler = "emscripten",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        features = [
            default_env_feature,
            opt_feature,
            fastbuild_feature,
            full_lto_feature,
            dbg_feature,
            no_canonical_prefixes_feature,
            simd_feature,
            pthread_feature,
            toolchain_include_directories_feature,
            default_compile_flags_feature,
            deterministic_compilation_feature,
            build_type_feature,
            runtime_library_search_directories_feature,
            nodejs_feature,
            emrum_feature,
        ],
    )

_wasm_cc_toolchain_config = rule(
    attrs = {
        "toolchain": attr.label(mandatory = True, allow_files = True),
        "_workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "_flags": attr.label(default = "//bzl/wasm:flag", providers = [BuildSettingInfo]),
        "_browser": attr.label(default = "//bzl/wasm:browser", providers = [BuildSettingInfo]),
    },
    provides = [CcToolchainConfigInfo],
    implementation = _impl,
)

def wasm_cc_toolchain(name, toolchain, **kwargs):
    """Macro to create a bazel cc_toolchain that uses targets WebAssembly."""
    config_name = "{}-config".format(name)
    _wasm_cc_toolchain_config(
        name = config_name,
        toolchain = toolchain,
    )
    native.cc_toolchain(
        name = name,
        toolchain_config = config_name,
        **kwargs
    )
