load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
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
load("//bzl/crosstool:toolchain-compiler-flags.bzl", "DBG_COMPILER_FLAGS", "FASTBUILD_COMPILER_FLAGS", "OPT_WITH_SYMBOLS_NO_LTO_COMPILER_FLAGS")

# Those action name have been removed by bazel, but we still need them for bzl/apple/objc.bzl rules.
OBJCPP_EXECUTABLE_ACTION_NAME = "objc++-executable"
OBJC_ARCHIVE_ACTION_NAME = "objc-archive"

# All compilation acctions for c/c++.
all_compile_actions = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.objc_compile,
    ACTION_NAMES.objcpp_compile,
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

# Compilation actions for Objective C++ only.
all_objcpp_compile_actions = [
    ACTION_NAMES.objcpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.assemble,
    ACTION_NAMES.clif_match,
]

# All linking actions.
all_link_actions = [
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
    ACTION_NAMES.objc_fully_link,
    ACTION_NAMES.objc_executable,
    OBJCPP_EXECUTABLE_ACTION_NAME,
]

# All archiver actions.
all_archiver_actions = [
    ACTION_NAMES.cpp_link_static_library,
    OBJC_ARCHIVE_ACTION_NAME,
]

APPLE_PLATFORMS = ["macos", "ios", "iossimulator", "tvos", "watchos", "driverkit"]

def _impl(ctx):
    """Implementation for _xcode_cc_toolchain_config rule."""
    if ctx.attr.platform not in APPLE_PLATFORMS:
        fail("platform must be one of ({platforms})".format(platforms = APPLE_PLATFORMS))

    xcode_version = ctx.attr._xcode_version[BuildSettingInfo].value
    external_xcode_root = "external/xcode{}".format(xcode_version)

    llvminfo = ctx.toolchains["//bzl/llvm:toolchain_type"].llvminfo

    # Map clang platform string to XCode sdk identifier.
    sdk = {
        "macos": "MacOSX",
        "ios": "iPhoneOS",
        "iossimulator": "iPhoneSimulator",
        "tvos": "AppleTVOS",
        "watchos": "WatchOS",
        "driverkit": "DriverKit",
    }[ctx.attr.platform]

    sysroot = "{external_xcode_root}/Xcode_app/Contents/Developer/Platforms/{sdk}.platform/Developer/SDKs/{sdk}.sdk".format(sdk = sdk, external_xcode_root = external_xcode_root)

    # These directories can be referenced in dependency files without being tracked by bazel.
    cxx_builtin_include_directories = [
        "/{{{{toolchain}}}}/{external_xcode_root}/Xcode_app/Contents/Developer/Platforms/{sdk}.platform/Developer/SDKs".format(sdk = sdk, external_xcode_root = external_xcode_root),
        "/{{{{toolchain}}}}/{runtime}".format(runtime = llvminfo.runtime),
    ]

    # Define the tools that are used in compilation and linking. For our use,
    # only "gcc" and "ar", i.e., compiling/linking and static linking will
    # actually be used.
    tool_paths = [
        tool_path(name = "ar", path = "libtool-xcwrapper.sh"),
        tool_path(name = "cpp", path = "/usr/bin/cpp"),  # unused
        tool_path(name = "dwp", path = "/usr/bin/dwp"),  # unused
        tool_path(name = "gcc", path = "clang-xcwrapper.sh"),
        tool_path(name = "gcov", path = "/usr/vin/gcov"),  # unused
        tool_path(name = "ld", path = "/usr/bin/ld"),  # unused
        tool_path(name = "nm", path = "/usr/bin/nm"),  # unused
        tool_path(name = "objcopy", path = "/usr/bin/objcopy"),  # unused
        tool_path(name = "objdump", path = "/usr/bin/objdump"),  # unused
        tool_path(name = "strip", path = "/usr/bin/strip"),  # Unused, see strip_ac below.
    ]

    strip_ac = action_config(
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
    #
    # On OSX, when linking, the debug information isn’t included in the final binary. Instead, the
    # DWARF data remains in the object files until it is processed by the dsymutil tool.
    # When you build without LTO, your object files will be Mach-O files:
    #   ~/repo/code8 file bazel-out/darwin_arm64-opt/bin/bzl/examples/_objs/hello-cc-test-example/hello-cc-test-example.o
    #   bazel-out/darwin_arm64-opt/bin/bzl/examples/_objs/hello-cc-test-example/hello-cc-test-example.o: Mach-O 64-bit object arm64
    # But when you run with LTO, your object files are LLVM bitcode:
    #   ~/repo/code8 file bazel-out/darwin_arm64-opt/bin/bzl/examples/_objs/hello-cc-test-example/hello-cc-test-example.o
    #   bazel-out/darwin_arm64-opt/bin/bzl/examples/_objs/hello-cc-test-example/hello-cc-test-example.o: LLVM bitcode, wrapper
    # This is unusable by dsymutil. This means you don't get symbols and using lldb is much less
    # useful. To fix this, pass: `--linkopt=-Wl,-object_path_lto,$(pwd)/bazel-out/lto.o`.
    # TODO(paris): Instead of having people manually pass object_path_lto, we could do it
    # automatically. The complication is that the linker assigns simple sequential numbers to these
    # files and doesn't handle conflicts when multiple linkers are running at once. In projects with
    # several binaries, object files from earlier links could be overwritten by those from later
    # ones. To avoid this, we place object files in a subdirectory named after the binary being
    # linked. One approach may be to use `%{output_execpath}` (and `expand_if_available`).
    opt_feature = feature(name = "opt")
    full_lto_feature = feature(
        name = "full_lto",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions + all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            # Link with LTO in opt build.
                            "-flto",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            # NOTE(paris): This was added when building node native addons with LTO.
                            # Doing so would result in a EXC_BAD_ACCESS (code=1, address=0x0) in
                            # napi_define_properties() in the node addon native code. This fix is from
                            # node:
                            # - https://github.com/nodejs/node/blob/782c2e0fe4c3b7fd77a6fc63034bdc39ebba4a67/node.gyp#L632
                            # man ld -export_dynamic:
                            #   Preserves all global symbols in main executables during LTO.
                            #   Without this option, Link Time Optimization is allowed to
                            #   inline and remove global functions. This option is used when
                            #   a main executable may load a plug-in which requires certain
                            #   symbols from the main executable.
                            "-Wl,-export_dynamic",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )
    fastbuild_feature = feature(name = "fastbuild")
    dbg_feature = feature(name = "dbg")
    dbg_coverage_feature = feature(name = "dbg_coverage", flag_sets = [flag_set(actions = all_compile_actions + all_link_actions, flag_groups = [flag_group(flags = ["--coverage"])])])

    # These special bazel features communicate certain toolchain functionality to bazel.
    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)
    supports_pic_feature = feature(name = "supports_pic", enabled = True)
    supports_fission_feature = feature(name = "supports_fission", enabled = False)
    supports_gold_linker_feature = feature(name = "supports_gold_linker", enabled = False)
    supports_incremental_linker_feature = feature(name = "supports_incremental_linker", enabled = False)
    supports_interface_shared_objects_feature = feature(name = "supports_interface_shared_objects", enabled = False)
    supports_normalizing_ar_feature = feature(name = "supports_normalizing_ar", enabled = False)
    supports_start_end_lib_feature = feature(name = "supports_start_end_lib", enabled = False)

    # When PIC (Position-independent Code) is being used, add -fPIC flag.
    pic_feature = feature(
        name = "pic",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(flags = ["-fPIC"], expand_if_available = "pic"),
                ],
            ),
        ],
    )

    # When PIC is forced for an executable, make it a position-independent executable (PIE).
    force_pic_flags_feature = feature(
        name = "force_pic_flags",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                flag_groups = [
                    flag_group(
                        flags = ["-Wl,-pie"],
                        expand_if_available = "force_pic",
                    ),
                ],
            ),
        ],
    )

    # Override the archiver_flags defaults, since llvm-link-darwin doesn't
    # support the '-s' option which is supplied by default bazel.
    archiver_flags_feature = feature(
        name = "archiver_flags",
        flag_sets = [
            flag_set(
                actions = all_archiver_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-no_warning_for_no_symbols",
                            "-static",
                        ],
                    ),
                    flag_group(
                        flags = [
                            "-o",
                            "%{output_execpath}",
                        ],
                        expand_if_available = "output_execpath",
                    ),
                    flag_group(
                        iterate_over = "libraries_to_link",
                        flag_groups = [
                            flag_group(
                                iterate_over = "libraries_to_link.object_files",
                                flag_groups = [
                                    flag_group(
                                        flags = ["%{libraries_to_link.object_files}"],
                                    ),
                                ],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "object_file_group",
                                ),
                            ),
                            flag_group(
                                flag_groups = [
                                    flag_group(
                                        flags = ["%{libraries_to_link.name}"],
                                    ),
                                ],
                                expand_if_equal = variable_with_value(
                                    name = "libraries_to_link.type",
                                    value = "object_file",
                                ),
                            ),
                        ],
                        expand_if_available = "libraries_to_link",
                    ),
                ],
            ),
        ],
    )

    # Environment variables to set for compile and link actions. These affect
    # the behavior of 'xcrun' and tell it the location of the XCode Platform
    # SDKs and the LLVM toolchain to use.
    default_env_feature = feature(
        name = "default_env_feature",
        enabled = True,
        env_sets = [
            env_set(
                actions = all_compile_actions + all_link_actions + all_archiver_actions,
                env_entries = [
                                  env_entry("RELATIVE_DEVELOPER_DIR", "{external_xcode_root}/Xcode_app/Contents/Developer".format(external_xcode_root = external_xcode_root)),
                                  env_entry("RELATIVE_EXTERNAL_TOOLCHAINS_DIR", "{}/Toolchains".format(llvminfo.root) if llvminfo.xcode_xctoolchain else llvminfo.root),
                                  # env_entry("TOOLCHAINS", llvminfo.xcode_toolchain_id),
                                  # env_entry("XCTOOLCHAIN", llvminfo.xcode_xctoolchain),
                                  env_entry("SDK", "{sdk}".format(sdk = sdk.lower())),
                                  env_entry("PLATFORM_SYSROOT", sysroot),
                                  env_entry("WORKSPACE_ENV", ctx.file.workspace_env.path),
                                  env_entry("LLVM_USR", llvminfo.usr),
                              ] + ([env_entry("TOOLCHAINS", llvminfo.xcode_toolchain_id)] if llvminfo.xcode_toolchain_id else []) +
                              ([env_entry("XCTOOLCHAIN", llvminfo.xcode_xctoolchain)] if llvminfo.xcode_xctoolchain else []),
            ),
        ],
    )

    min_version = ctx.attr._ios_min_version[BuildSettingInfo].value
    if ctx.attr.platform == "macos":
        min_version = ctx.attr._osx_min_version[BuildSettingInfo].value

    # Add target triple with first arch. This specifies the arch
    # (x86_64/arm64), the vendor (apple), the platform (macos/ios), and the
    # minimum OS version.
    target_arch_flags = [
        "-target",
        "{cpu}-apple-{target_platform}{min_version}{target_abi}".format(
            cpu = ctx.attr.cpus[0],
            target_platform = "ios" if ctx.attr.platform == "iossimulator" else ctx.attr.platform,  # For ios simulator target is still ios
            min_version = min_version,
            target_abi = "-simulator" if ctx.attr.platform == "iossimulator" else "",
        ),  # for ios simulator -simulator as target abi (as per documentation)
    ]

    # If there are multiple CPUs, specifiy all architectures with -arch flags which will tell LLVM to build a multi-arch binary.
    if len(ctx.attr.cpus) > 1:
        for cpu in ctx.attr.cpus:
            target_arch_flags.extend([
                "-arch",
                cpu,
            ])

    default_compile_flag_groups = [flag_group(flags = target_arch_flags)]

    # Add general compile flags.
    default_compile_flag_groups.append(flag_group(flags = [
        "-U_FORTIFY_SOURCE",
        "-D_FORTIFY_SOURCE=1",
        "-fstack-protector",
        "-Wall",
        "-Wthread-safety",
        "-Wself-assign",
        "-fno-omit-frame-pointer",
        "-fcolor-diagnostics",
    ]))

    # Add popcnt instructions for x86_64 arch only.
    if "x86_64" in ctx.attr.cpus:
        if len(ctx.attr.cpus) > 1:
            default_compile_flag_groups.append(flag_group(flags = [
                "-Xarch_x86_64",
                "-mpopcnt",
            ]))
        else:
            default_compile_flag_groups.append(flag_group(flags = [
                "-mpopcnt",
            ]))

    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                # C++ compile actions: Compile for C++20 with exceptions enabled.
                actions = all_cpp_compile_actions + all_objcpp_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-std=c++20",
                    "-fcxx-exceptions",
                    "-stdlib=libc++",
                ])],
            ),
            flag_set(
                # C compile Actions: Compile for C17.
                actions = [
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.objc_compile,
                ],
                flag_groups = [flag_group(flags = [
                    "-std=c17",
                ])],
            ),
            flag_set(
                # C and C++ compile actions.
                actions = all_compile_actions,
                flag_groups = default_compile_flag_groups,
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
                    flag_group(flags = target_arch_flags),
                    flag_group(flags = [
                        "-fuse-ld=lld",  # Use the llvm lld linker, which drives ld64.lld for Mac.
                        "-lc++",  # Use llvm's libc++ as the std library.
                        "-lm",  # Link libm (math library)
                        "-headerpad_max_install_names",  # Support large rpaths/install_nams
                        # If we do not manually specify the number of threads then when LLVM's ld
                        # calculates LC_UUID in Writer::writeUuid(), it will split up the file into
                        # separate chunks based on parallel::strategy.compute_thread_count(), which
                        # is based on the number of cores. So the LC_UUID in the linker output will
                        # be different based on the number of cores on the host machine.
                        "-Wl,--threads=10",
                    ]),
                    # If you have having issues with missing symbols, you can try disabling this flag
                    # to enable link time symbol checking. But do not check the change in. If you want
                    # to you can add a feature flag for this instead.
                    flag_group(flags = ["-undefined", "dynamic_lookup"]),  # Allow runtime lookup of dynamic symbols.
                ],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            # Link with -dead_strip to optimize for size in opt build.
                            "-Wl,-dead_strip",  # Strip the dead code.
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
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
                flag_groups = [flag_group(flags = [
                    "-Wno-builtin-macro-redefined",
                    "-D__DATE__=\"redacted\"",
                    "-D__TIMESTAMP__=\"redacted\"",
                    "-D__TIME__=\"redacted\"",
                ])],
            ),
        ],
    )

    objc_defines_feature = feature(
        name = "objc_defines",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.objc_compile,
                    ACTION_NAMES.objcpp_compile,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["-D%{preprocessor_defines}"],
                        iterate_over = "preprocessor_defines",
                    ),
                ],
            ),
        ],
    )

    objc_arc_feature = feature(
        name = "objc_arc",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_module_compile,
                    ACTION_NAMES.cpp_header_parsing,
                    "c++-header-preprocessing",
                    ACTION_NAMES.assemble,
                    ACTION_NAMES.preprocess_assemble,
                    ACTION_NAMES.objc_compile,
                    ACTION_NAMES.objcpp_compile,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["-fobjc-arc"],
                        expand_if_available = "objc_arc",
                    ),
                ],
            ),
        ],
    )

    no_objc_arc_feature = feature(
        name = "no_objc_arc",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_module_compile,
                    ACTION_NAMES.cpp_header_parsing,
                    "c++-header-preprocessing",
                    ACTION_NAMES.assemble,
                    ACTION_NAMES.preprocess_assemble,
                    ACTION_NAMES.objc_compile,
                    ACTION_NAMES.objcpp_compile,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["-fno-objc-arc"],
                        expand_if_available = "no_objc_arc",
                    ),
                ],
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
                flag_groups = [flag_group(flags = OPT_WITH_SYMBOLS_NO_LTO_COMPILER_FLAGS)],
                with_features = [with_feature_set(features = ["opt"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = [
                    # Build with function-sections and data-sections in optimized, non-bitcode builds.
                    "-ffunction-sections",
                    "-fdata-sections",
                ])],
                with_features = [with_feature_set(not_features = ["dbg", "bitcode_embedded"])],
            ),
        ],
    )

    # Static linking happens with libtool or llvm-libtool-darwin. This syntax
    # is different than 'ar' and the flags needed are indicated below.
    default_static_link_flags_feature = feature(
        name = "default_static_link_flags",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_static_library,
                ],
                flag_groups = [flag_group(flags = [
                    "-static",
                    "-s",
                    "-o",
                ])],
            ),
        ],
    )

    # Configurable feature to enable Apple bitcode.
    bitcode_embedded_feature = feature(
        name = "bitcode_embedded",
        flag_sets = [
            flag_set(
                actions = all_compile_actions + all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = ["-fembed-bitcode"],
                    ),
                ],
            ),
        ],
    )

    # Configurable feature to enable Apple bitcode markers.
    bitcode_embedded_markers_feature = feature(
        name = "bitcode_embedded_markers",
        flag_sets = [
            flag_set(
                actions = all_compile_actions + all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = ["-fembed-bitcode-marker"],
                    ),
                ],
            ),
        ],
    )

    objc_compile_ac = action_config(
        action_name = ACTION_NAMES.objc_compile,
        tools = [
            tool(
                path = "clang-xcwrapper.sh",
            ),
        ],
        implies = [
            "user_compile_flags",
            "objc_actions",
            "default_compile_flags",
            "objc_arc",
            "no_objc_arc",
            "objc_defines",
        ],
    )
    objcpp_compile_ac = action_config(
        action_name = ACTION_NAMES.objcpp_compile,
        tools = [
            tool(
                path = "clang-xcwrapper.sh",
            ),
        ],
        implies = [
            "user_compile_flags",
            "default_compile_flags",
            "objc_arc",
            "no_objc_arc",
            "objc_defines",
        ],
    )
    objc_archive_ac = action_config(
        action_name = OBJC_ARCHIVE_ACTION_NAME,
        tools = [
            tool(
                path = "libtool-xcwrapper.sh",
            ),
        ],
    )
    objc_executable_ac = action_config(
        action_name = ACTION_NAMES.objc_executable,
        tools = [
            tool(
                path = "clang-xcwrapper.sh",
            ),
        ],
        flag_sets = [
            flag_set(
                flag_groups = [
                    flag_group(
                        flags = [
                            "-Xlinker",
                            "-objc_abi_version",
                            "-Xlinker",
                            "2",
                            "-Xlinker",
                            "-rpath",
                            "-Xlinker",
                            "@executable_path/Frameworks",
                            "-fobjc-link-runtime",
                            "-ObjC",
                        ],
                    ),
                    flag_group(
                        flags = ["-framework %{framework_names}"],
                        iterate_over = "framework_names",
                    ),
                    flag_group(
                        flags = ["-weak_framework %{weak_framework_names}"],
                        iterate_over = "weak_framework_names",
                    ),
                    flag_group(
                        flags = ["-l%{library_names}"],
                        iterate_over = "library_names",
                    ),
                    flag_group(
                        flags = ["-filelist", "%{filelist}"],
                    ),
                    flag_group(
                        flags = ["-o", "%{linked_binary}"],
                    ),
                    flag_group(
                        flags = ["-force_load %{force_load_exec_paths}"],
                        iterate_over = "force_load_exec_paths",
                    ),
                    flag_group(
                        flags = ["%{dep_linkopts}"],
                        iterate_over = "dep_linkopts",
                    ),
                    flag_group(
                        flags = ["-Wl,%{attr_linkopts}"],
                        iterate_over = "attr_linkopts",
                    ),
                ],
            ),
        ],
    )
    objcpp_executable_ac = action_config(
        action_name = OBJCPP_EXECUTABLE_ACTION_NAME,
        tools = [
            tool(
                path = "clang-xcwrapper.sh",
            ),
        ],
        flag_sets = [
            flag_set(
                flag_groups = [
                    flag_group(
                        flags = [
                            "-stdlib=libc++",
                            "-std=c++17",
                            "-fno-aligned-allocation",
                        ],
                    ),
                    flag_group(
                        flags = [
                            "-Xlinker",
                            "-objc_abi_version",
                            "-Xlinker",
                            "2",
                            "-Xlinker",
                            "-rpath",
                            "-Xlinker",
                            "@executable_path/Frameworks",
                            "-fobjc-link-runtime",
                            "-ObjC",
                        ],
                    ),
                    flag_group(
                        flags = ["-framework %{framework_names}"],
                        iterate_over = "framework_names",
                    ),
                    flag_group(
                        flags = ["-weak_framework %{weak_framework_names}"],
                        iterate_over = "weak_framework_names",
                    ),
                    flag_group(
                        flags = ["-l%{library_names}"],
                        iterate_over = "library_names",
                    ),
                    flag_group(
                        flags = ["-filelist", "%{filelist}"],
                    ),
                    flag_group(
                        flags = ["-o", "%{linked_binary}"],
                    ),
                    flag_group(
                        flags = ["-force_load %{force_load_exec_paths}"],
                        iterate_over = "force_load_exec_paths",
                    ),
                    flag_group(
                        flags = ["%{dep_linkopts}"],
                        iterate_over = "dep_linkopts",
                    ),
                    flag_group(
                        flags = ["-Wl,%{attr_linkopts}"],
                        iterate_over = "attr_linkopts",
                    ),
                ],
            ),
        ],
        implies = [
            "default_link_flags",
        ],
    )
    objc_fully_link_ac = action_config(
        action_name = "objc-fully-link",
        tools = [
            tool(
                path = "clang-xcwrapper.sh",
            ),
        ],
        flag_sets = [
            flag_set(
                flag_groups = [
                    flag_group(
                        flags = [
                            "-v",
                            "-static",
                            "-o",
                            "%{fully_linked_archive_path}",
                        ],
                    ),
                    flag_group(
                        flags = ["%{objc_library_exec_paths}"],
                        iterate_over = "objc_library_exec_paths",
                    ),
                    flag_group(
                        flags = ["%{cc_library_exec_paths}"],
                        iterate_over = "cc_library_exec_paths",
                    ),
                    flag_group(
                        flags = ["%{imported_library_exec_paths}"],
                        iterate_over = "imported_library_exec_paths",
                    ),
                ],
            ),
        ],
    )

    # Define objc actions.
    objc_actions_feature = feature(
        name = "objc_actions",
        implies = [
            ACTION_NAMES.objc_compile,
            ACTION_NAMES.objcpp_compile,
            ACTION_NAMES.objc_fully_link,
            OBJC_ARCHIVE_ACTION_NAME,
            ACTION_NAMES.objc_executable,
            OBJCPP_EXECUTABLE_ACTION_NAME,
            ACTION_NAMES.assemble,
            ACTION_NAMES.preprocess_assemble,
            ACTION_NAMES.c_compile,
            ACTION_NAMES.cpp_compile,
            ACTION_NAMES.cpp_link_static_library,
            ACTION_NAMES.cpp_link_dynamic_library,
            ACTION_NAMES.cpp_link_nodeps_dynamic_library,
            ACTION_NAMES.cpp_link_executable,
        ],
    )

    user_compile_flags_feature = feature(
        name = "user_compile_flags",
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = ["%{user_compile_flags}"],
                        iterate_over = "user_compile_flags",
                        expand_if_available = "user_compile_flags",
                    ),
                ],
            ),
        ],
    )

    # Apple uses .dylib as the suffix for dynamic libraries.
    artifact_name_patterns = [
        artifact_name_pattern(
            category_name = "dynamic_library",
            prefix = "lib",
            extension = ".dylib",
        ),
    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        toolchain_identifier = "cc-compiler",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = ":".join(ctx.attr.cpus),
        # This needs to be exactly 'macosx' to trigger apple-specific bazel
        # built-ins for default linking flags. It doesn't impact whether this is an iOS or Mac build.
        target_libc = "macosx",
        compiler = "compiler",
        abi_version = "local",
        abi_libc_version = "local",
        tool_paths = tool_paths,
        artifact_name_patterns = artifact_name_patterns,
        action_configs = [
            objc_compile_ac,
            objcpp_compile_ac,
            objc_fully_link_ac,
            objc_archive_ac,
            objc_executable_ac,
            objcpp_executable_ac,
            strip_ac,
        ],
        features = [
            opt_feature,
            full_lto_feature,
            fastbuild_feature,
            dbg_feature,
            dbg_coverage_feature,
            supports_dynamic_linker_feature,
            force_pic_flags_feature,
            supports_pic_feature,
            supports_fission_feature,
            supports_gold_linker_feature,
            objc_actions_feature,
            supports_incremental_linker_feature,
            supports_interface_shared_objects_feature,
            supports_normalizing_ar_feature,
            supports_start_end_lib_feature,
            bitcode_embedded_feature,
            bitcode_embedded_markers_feature,
            default_compile_flags_feature,
            default_env_feature,
            default_link_flags_feature,
            deterministic_compilation_feature,
            build_type_feature,
            default_static_link_flags_feature,
            pic_feature,
            archiver_flags_feature,
            objc_arc_feature,
            no_objc_arc_feature,
            user_compile_flags_feature,
            objc_defines_feature,
        ],
    )

_xcode_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "cpus": attr.string_list(mandatory = True),
        "platform": attr.string(mandatory = True),
        "workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "strip_tool": attr.label(default = "//bzl/crosstool:strip-xcwrapper", executable = True, cfg = "exec"),
        "_xcode_version": attr.label(default = "//bzl/xcode:version"),
        # We use the value of one of these, depending on whether we're building for iOS or OSX.
        "_ios_min_version": attr.label(default = Label("//bzl/xcode:ios-min-version"), providers = [BuildSettingInfo]),
        "_osx_min_version": attr.label(default = Label("//bzl/xcode:osx-min-version"), providers = [BuildSettingInfo]),
    },
    toolchains = [
        "//bzl/llvm:toolchain_type",
    ],
    provides = [CcToolchainConfigInfo],
)

def xcode_cc_toolchain(name, platform, cpus, **kwargs):
    """Macro to create a bazel cc_toolchain that uses XCode platform SDKs."""
    config_name = "{}-config".format(name)
    _xcode_cc_toolchain_config(
        name = config_name,
        platform = platform,
        cpus = cpus,
    )

    native.cc_toolchain(
        name = name,
        toolchain_config = config_name,
        **kwargs
    )
