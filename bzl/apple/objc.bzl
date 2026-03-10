"""
This module defines Bazel rules for objective-c libraries.
"""

# buildifier: disable=list-append
load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

# The native objc_library rule in bazel has a deficiency in which it fails to propagate compilation
# contexts, linking contexts, and runfiles when a dependency chain looks like: cc_library -> objc_library -> cc_library.
# This custom macro works around this issue and can be used in place of objc_library to fix this
# issue.

def _nia_objc_library_impl(ctx):
    objc_library = ctx.attr.objc_library

    toolchain = find_cpp_toolchain(ctx)

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    # Bazel is not properly setting alwayslink in objc_libraries, so we
    # workaround the issue here by setting alwayslink=1 on the top-level
    # libraries. See core issue: https://github.com/bazelbuild/bazel/issues/13510.
    objc_cc_info = objc_library[CcInfo]
    if ctx.attr.alwayslink:
        objc_linker_inputs = []
        for linker_input in objc_cc_info.linking_context.linker_inputs.to_list():
            if (len(linker_input.libraries) > 0):
                top_library = linker_input.libraries[0]  # <-- Bazel docs say this will be a depset in the future.
                new_library_to_link = cc_common.create_library_to_link(
                    cc_toolchain = toolchain,
                    feature_configuration = feature_configuration,
                    actions = ctx.actions,
                    static_library = top_library.static_library,
                    pic_static_library = top_library.pic_static_library,
                    # If this is passed, it causes bazel to crash. See crash at:
                    # https://docs.google.com/document/d/<REMOVED_BEFORE_OPEN_SOURCING>
                    # dynamic_library = top_library.dynamic_library,
                    interface_library = top_library.interface_library,
                    alwayslink = ctx.attr.alwayslink,
                )
                new_linker_input = cc_common.create_linker_input(
                    owner = linker_input.owner,
                    libraries = depset([new_library_to_link] + linker_input.libraries[1:]),
                    user_link_flags = linker_input.user_link_flags,
                    additional_inputs = depset(linker_input.additional_inputs),
                )
                objc_linker_inputs.append(new_linker_input)

        # Create a new CcInfo with this patched compilation_context.
        objc_cc_info = CcInfo(
            compilation_context = objc_cc_info.compilation_context,
            linking_context = cc_common.create_linking_context(linker_inputs = depset(objc_linker_inputs)),
        )

    cc_info = cc_common.merge_cc_infos(
        direct_cc_infos = [objc_cc_info],
        # Merge previous CcInfos.
        cc_infos = [dep[CcInfo] for dep in ctx.attr.deps],
    )

    runfiles = ctx.runfiles(files = ctx.files.data)

    # Merge runfiles with default_runfiles of deps
    runfiles = runfiles.merge_all([dep[DefaultInfo].default_runfiles for dep in ctx.attr.deps])

    return [
        DefaultInfo(files = depset(ctx.files.objc_library), runfiles = runfiles),
        cc_info,
        objc_library[apple_common.Objc],
    ]

_nia_objc_library = rule(
    attrs = {
        "deps": attr.label_list(
            default = [],
        ),
        "data": attr.label_list(default = []),
        "alwayslink": attr.bool(default = False),
        "_cc_toolchain": attr.label(default = Label("@bazel_tools//tools/cpp:current_cc_toolchain")),
        "objc_library": attr.label(
            mandatory = True,
            providers = [apple_common.Objc],
        ),
    },
    fragments = ["cpp"],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    implementation = _nia_objc_library_impl,
)

def nia_objc_library(name, alwayslink = False, deps = [], data = [], visibility = ["//visibility:private"], tags = [], **kwargs):
    objc_lib = name + "-nia_objc"

    native.objc_library(
        name = objc_lib,
        deps = deps,
        data = data,
        alwayslink = alwayslink,
        visibility = ["//visibility:private"],
        tags = tags,
        **kwargs
    )

    _nia_objc_library(name = name, deps = deps, alwayslink = alwayslink, data = data, objc_library = objc_lib, visibility = visibility, tags = tags)

# Convienence function for being nia_objc_library when the sources have objc and cc_library when they don't.
def cc_or_objc_library(name, **kwargs):
    """Convenience function for being nia_objc_library when the sources have objc and cc_library when they don't.

    Args:
      name: The name of the target.
      **kwargs: Additional keyword arguments passed to the underlying rules.

    Returns:
      The target that was created.
    """
    if "strip_include_prefix" in kwargs:
        fail("strip_include_prefix not supported in cc_or_objc_library, use \"includes\" for similar functionality")

    all_srcs = kwargs.get("srcs", []) + kwargs.get("non_arc_srcs", [])
    has_objc = False
    for src in all_srcs:
        if src.endswith(".mm") or src.endswith(".m"):
            has_objc = True
            break

    if has_objc:
        return nia_objc_library(name = name, **kwargs)
    else:
        kwargs.pop("srcs", None)
        kwargs.pop("non_arc_srcs", None)
        copts = kwargs.pop("copts", [])
        linkopts = kwargs.pop("linkopts", [])
        if "enable_modules" in kwargs:
            kwargs.pop("enable_modules", None)
            copts.append("-fmodules")
        linkopts += [y for x in kwargs.pop("sdk_frameworks", []) for y in ("-framework", x)]
        linkopts += [y for x in kwargs.pop("weak_sdk_frameworks", []) for y in ("-framework", x)]
        return native.cc_library(name = name, copts = copts, linkopts = linkopts, srcs = all_srcs, **kwargs)
