"""Provides the unity_filegroup() Bazel macro."""

load("//bzl/unity/impl:transitions.bzl", "unity_platform_transition")

# The "unity_contents_path" field contains the path to Unity installation being built.
_UnityContentsPathInfo = provider("Provides information about Unity", fields = ["unity_contents_path"])

def _unity_filegroup_resolve_unity_location_impl(ctx):
    unity = ctx.toolchains["@unity-version//:toolchain_type"]
    if unity.unityinfo.unity_contents_path == "None":
        fail("Unity is not installed.")
    return [
        DefaultInfo(),
        _UnityContentsPathInfo(unity_contents_path = unity.unityinfo.unity_contents_path),
    ]

_unity_filegroup_resolve_unity_location = rule(
    implementation = _unity_filegroup_resolve_unity_location_impl,
    cfg = unity_platform_transition,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "_unity_platform": attr.label(
            default = "@unity-version//:local",
        ),
    },
    toolchains = [
        "@unity-version//:toolchain_type",
    ],
)

def _unity_filegroup_copy_files_impl(ctx):
    # First create output files using the provided srcs.
    outs = []
    for src in ctx.attr.srcs:
        outs.append(ctx.actions.declare_file(src))

    # Next copy files from Unity directory provided by resolve_unity_location to the each output file path.
    unity_contents_path = ctx.attr.stage1[_UnityContentsPathInfo].unity_contents_path
    for idx, out in enumerate(outs):
        unity_file_src = unity_contents_path + "/" + ctx.attr.srcs[idx]
        ctx.actions.run_shell(
            inputs = [],
            outputs = [out],
            command = "cp \"%s\" \"%s\"" % (
                unity_file_src,
                out.path,
            ),
        )

    return [DefaultInfo(files = depset(outs), runfiles = ctx.runfiles(outs))]

_unity_filegroup_copy_files = rule(
    implementation = _unity_filegroup_copy_files_impl,
    attrs = {
        # This is a string list b/c it's not a real path in Bazel (so you can't use glob()).
        "srcs": attr.string_list(
            default = [],
        ),
        # Run before _unity_filegroup_copy_files_impl and provides _UnityContentsPathInfo.
        "stage1": attr.label(
            mandatory = True,
        ),
    },
)

def unity_filegroup(name, srcs):
    """Bazel macro to prepare a list of Unity files which are intended to be included in another target.

    Args:
      name (string): Target name.
      srcs (List[string]): List of Unity files. These should be relative to the Contents/ directory
                           in the appropriate Unity install. For example if
                           "PluginAPI/IUnityEventQueue.h" is in srcs and we are building for
                           2021.3.12f1 on a machine with Unity Hub, we will find it in:
                           /Applications/Unity/Hub/Editor/2021.3.12f1/Unity.app/Contents/PluginAPI/IUnityEventQueue.h.
                           srcs will be copied into the directory in which this target is defined.
                           For example if a unity_filegroup macro is defined in /bzl/foo/BUILD, a
                           source file which uses "PluginAPI/bar.h" should use
                           #include "bzl/foo/PluginAPI/bar.h".
    """

    # The first stage is used to get the Unity path to use (which includes the version). We do this
    # by including unity_platform_transition which will make "@unity-version//:toolchain_type"
    # available which then provides unity_contents_path.
    resolve_unity_location = name + "-resolve-unity-location"
    _unity_filegroup_resolve_unity_location(name = resolve_unity_location)

    # We then use the second stage to copy the Unity files from the unity_contents_path directory
    # to the sandbox. We cannot do this in resolve_unity_location because it will be executed from
    # thein the wrong directory to copy files to, and doesn't know the directory that copy_files
    # will be executed from.
    # For example in one run:
    # - resolve_unity_location was executed from bazel-out/darwin_arm64-fastbuild-ST-55f69b09042a
    #  - copy_files was executed from bazel-out/darwin_arm64-fastbuild (this is where we want these
    #    files to end up).
    _unity_filegroup_copy_files(name = name, srcs = srcs, stage1 = resolve_unity_location)
