"""Build rules for openssl Bazel.


This is a quick rule to make Bazel compile openssl.
"""

load(
    "@bazel_tools//tools/build_defs/cc:action_names.bzl",
    "CPP_COMPILE_ACTION_NAME",
    "CPP_LINK_EXECUTABLE_ACTION_NAME",
    "CPP_LINK_STATIC_LIBRARY_ACTION_NAME",
    "C_COMPILE_ACTION_NAME",
)
load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load("@android-sdk//:defines.bzl", "ANDROID_NDK")

openssl_binary_outputs = [
    "libcrypto.a",
    "libssl.a",
    "include/openssl/opensslconf.h",
    "include/openssl/opensslv.h",
]

def _openssl_build_impl(ctx):
    libcrypto = ctx.actions.declare_file("libcrypto.a")
    libssl = ctx.actions.declare_file("libssl.a")
    opensslconf_header = ctx.actions.declare_file("include/openssl/opensslconf.h")
    opensslv_header = ctx.actions.declare_file("include/openssl/opensslv.h")
    outs = [libcrypto, libssl, opensslconf_header, opensslv_header]
    root = ctx.files.srcs[0].dirname

    toolchain = find_cpp_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    # buildifier: disable=unused-variable
    compile_context, compile_outputs = (
        cc_common.compile(
            name = ctx.label.name + "-cc",
            actions = ctx.actions,
            feature_configuration = feature_configuration,
            cc_toolchain = toolchain,
            additional_inputs = ctx.files.srcs,
            public_hdrs = [opensslconf_header],
        )
    )

    cryto_lib = cc_common.create_library_to_link(
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        static_library = libcrypto,
    )

    ssl_lib = cc_common.create_library_to_link(
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        static_library = libssl,
    )

    linker_input = cc_common.create_linker_input(
        owner = ctx.label,
        libraries = depset([cryto_lib, ssl_lib]),
    )

    link_context = cc_common.create_linking_context(
        linker_inputs = depset([linker_input]),
    )

    compile_variables = cc_common.create_compile_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        user_compile_flags = ctx.fragments.cpp.copts + ctx.attr.copts,
    )

    cflags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = C_COMPILE_ACTION_NAME,
        variables = compile_variables,
    )

    cxxflags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = CPP_COMPILE_ACTION_NAME,
        variables = compile_variables,
    )

    link_variables = cc_common.create_link_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        user_link_flags = ctx.fragments.cpp.linkopts,
    )

    ldflags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = CPP_LINK_EXECUTABLE_ACTION_NAME,
        variables = link_variables,
    )

    archiver_variables = cc_common.create_link_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        is_using_linker = False,
        output_file = "outfile",  # By adding this, we find out whether to use '-o' or not.
    )

    arflags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = CPP_LINK_STATIC_LIBRARY_ACTION_NAME,
        variables = archiver_variables,
    )
    arflags = arflags[:-1]  # Drop the stubbed 'outfile'.

    cc = cc_common.get_tool_for_action(feature_configuration = feature_configuration, action_name = C_COMPILE_ACTION_NAME)
    cxx = cc_common.get_tool_for_action(feature_configuration = feature_configuration, action_name = CPP_COMPILE_ACTION_NAME)
    ar = cc_common.get_tool_for_action(feature_configuration = feature_configuration, action_name = CPP_LINK_STATIC_LIBRARY_ACTION_NAME)

    # Remove the C/C++ standard flag.
    cflags_filtered = [cflag for cflag in cflags if not cflag.startswith("-std=c")]
    cxxflags_filtered = [cxxflag for cxxflag in cxxflags if not cxxflag.startswith("-std=c")]

    cenv = cc_common.get_environment_variables(feature_configuration = feature_configuration, action_name = C_COMPILE_ACTION_NAME, variables = compile_variables)
    env = {
        "ANDROID_NDK_HOME": ANDROID_NDK,
        "TOOLCHAIN": ctx.attr.toolchain,
        "OPENSSL_ROOT": root,
        "CONFIGURE_FLAGS": ctx.attr.configure_flags,
        "LIBCRYPTOP_PATH": outs[0].path,
        "LIBSSL_PATH": outs[1].path,
        "OPENSSL_H_PATH": outs[2].path,
        "OPENSSLV_H_PATH": outs[3].path,
        "CC": cc,
        "CFLAGS": " ".join(cflags_filtered),
        "CXX": cxx,
        "CXXFLAGS": " ".join(cxxflags_filtered),
        "AR": ar,
        "ARFLAGS": " ".join(arflags),
        "LDFLAGS": " ".join(ldflags),
        "RANLIB": ":",  # We don't use ranlib, a colon will cause this to be skipped.
        "SOURCE_DATE_EPOCH": "00",  # Set timestamp in static lib. Needs wwo zeroes to evaluate to unix epoch time.
    }
    env.update(cenv)
    cflags = [cflag for cflag in cflags if not cflag.startswith("-std")]

    ctx.actions.run(
        tools = ctx.files._cc_toolchain,
        executable = ctx.file._openssl_build,
        inputs = ctx.files.srcs,
        outputs = outs,
        env = env,
    )

    return [
        DefaultInfo(files = depset(outs)),
        CcInfo(
            compilation_context = compile_context,
            linking_context = link_context,
        ),
    ]

openssl_build = rule(
    implementation = _openssl_build_impl,
    attrs = {
        "copts": attr.string_list(default = []),
        "srcs": attr.label_list(default = [], allow_files = True),
        "toolchain": attr.string(mandatory = True),
        "configure_flags": attr.string(mandatory = True),
        "_openssl_build": attr.label(default = "@the8thwall//third_party/openssl:openssl-build", allow_single_file = True),
        "_cc_toolchain": attr.label(
            default = Label("@bazel_tools//tools/cpp:current_cc_toolchain"),
        ),
    },
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    fragments = ["cpp"],
)
