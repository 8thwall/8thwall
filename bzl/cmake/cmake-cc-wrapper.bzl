"""
This module defines Bazel rules for creating a cmake config file for a cc_library.
"""

#load(
#    "@bazel_tools//tools/build_defs/cc:action_names.bzl",
#    "CPP_COMPILE_ACTION_NAME",
#    "CPP_LINK_EXECUTABLE_ACTION_NAME",
#    "CPP_LINK_STATIC_LIBRARY_ACTION_NAME",
#    "C_COMPILE_ACTION_NAME",
#)
#load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

CmakeConfigInfo = provider(
    "Pass information about libraries and headers in a cmake config file.",
    fields = {
        "files": "path to the cmake config file(s).",
    },
)
#export CmakeConfigInfo

def _cmake_cc_wrapper_impl(ctx):
    compile_ctx = ctx.attr.dep[CcInfo].compilation_context

    static_libs = []

    link_ctx = ctx.attr.dep[CcInfo].linking_context
    for linker_input in link_ctx.linker_inputs.to_list():
        for library in linker_input.libraries:
            if library.pic_static_library:
                static_libs.append(library.pic_static_library)
            elif library.static_library:
                static_libs.append(library.static_library)
            else:
                fail("Missing static library")

    includes = depset(transitive = [compile_ctx.quote_includes, compile_ctx.includes, compile_ctx.system_includes])

    pkg_prefix = ctx.attr.package_name.upper()
    config_name = "Find{}.cmake".format(ctx.attr.package_name)
    cmake_config = ctx.actions.declare_file(config_name)
    cmake_config_content = [
        "set ({pkg}_INCLUDE_DIRS {includes})".format(pkg = pkg_prefix, includes = " ".join(["${{BAZEL_EXECROOT}}/{}".format(inc) for inc in includes.to_list()])),
        "set ({pkg}_LIBRARIES {libs})".format(pkg = pkg_prefix, libs = " ".join(["${{BAZEL_EXECROOT}}/{}".format(lib.path) for lib in static_libs])),
        "include(FindPackageHandleStandardArgs)",
        "",
        "find_package_handle_standard_args(",
        "    {pkg}".format(pkg = ctx.attr.package_name),
        "    REQUIRED_VARS {pkg}_LIBRARIES {pkg}_INCLUDE_DIRS".format(pkg = pkg_prefix),
        ")",
        "mark_as_advanced(",
        "    {pkg}_FOUND".format(pkg = pkg_prefix),
        "    {pkg}_LIBRARIES {pkg}_INCLUDE_DIRS".format(pkg = pkg_prefix),
        ")",
        "",
    ]
    ctx.actions.write(
        output = cmake_config,
        content = "\n".join(cmake_config_content),
    )

    return [
        ctx.attr.dep[CcInfo],
        CmakeConfigInfo(files = depset([cmake_config])),
    ]

cmake_cc_wrapper = rule(
    implementation = _cmake_cc_wrapper_impl,
    attrs = {
        "dep": attr.label(mandatory = True),
        "package_name": attr.string(mandatory = True),
        #    "_cc_toolchain": attr.label(
        #        default = Label("@bazel_tools//tools/cpp:current_cc_toolchain"),
        #    ),
    },
    #toolchains = [
    #"@bazel_tools//tools/cpp:toolchain_type",
    #"@rules_foreign_cc//toolchains:cmake_toolchain",
    #],
    #incompatible_use_toolchain_transition = True,
)
