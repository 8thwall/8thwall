load("//bzl/unity/impl:transitions.bzl", "unity_platform_transition")

_CsharpDepsInfo = provider(
    "Pass information about .net library dependencies.",
    fields = {
        "srcs": "csharp sources",
        "dlls": "dependent .dll files",
    },
)

# Generates a transitive dependency list of csharp .cs that can be compiled into a .dll assembly.
def _csharp_library_impl(ctx):
    return _CsharpDepsInfo(
        srcs = depset(ctx.files.srcs, transitive = [dep[_CsharpDepsInfo].srcs for dep in ctx.attr.deps]),
        dlls = depset([]),
    )

_csharp_library = rule(
    implementation = _csharp_library_impl,
    attrs = {
        "outs": attr.output_list(),
        "srcs": attr.label_list(
            allow_files = True,
        ),
        "deps": attr.label_list(providers = [_CsharpDepsInfo]),
    },
)

def _unity_csharp_dll_impl(ctx):
    inputs = depset(ctx.files.srcs, transitive = [dep[_CsharpDepsInfo].srcs for dep in ctx.attr.deps])

    # Tell mono where to find the dependent csharp dlls.
    dlls = depset(transitive = [dep[_CsharpDepsInfo].dlls for dep in ctx.attr.deps])

    includes = []
    for dll in dlls.to_list():
        includes.append("-r:$EXEC_ROOT_PATH/" + dll.path)

    # Construct the output path for the compiler.
    dll_out = ctx.outputs.outs[0].path
    out_arg = ["-out:" + dll_out]

    unity = ctx.toolchains["@unity-version//:toolchain_type"]
    mono = ctx.toolchains["//bzl/mono:toolchain_type"]

    # Execute the wrapper script.
    ctx.actions.run(
        inputs = inputs.to_list() + ctx.files.deps,
        outputs = ctx.outputs.outs,
        executable = ctx.executable._unity_csharp_dll,
        env = {
            "UNITY": unity.unityinfo.unity_path,
            "MONO": mono.monoinfo.mono_path,
            "WORKSPACE_ENV": ctx.file._workspace_env.path,
        },
        tools = [ctx.file._workspace_env],
        arguments = includes + out_arg + [s.path for s in inputs.to_list()],
        mnemonic = "GenUnityCSharpDll",
    )

    return [_CsharpDepsInfo(
        srcs = depset([]),
        dlls = depset(ctx.outputs.outs, transitive = [dlls]),
    )]

_unity_csharp_dll = rule(
    implementation = _unity_csharp_dll_impl,
    cfg = unity_platform_transition,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "_unity_platform": attr.label(
            default = "@unity-version//:local",
        ),
        "outs": attr.output_list(),
        "srcs": attr.label_list(
            allow_files = True,
        ),
        "deps": attr.label_list(providers = [_CsharpDepsInfo]),
        "_unity_csharp_dll": attr.label(
            default = Label("//bzl/unity/impl:unity-csharp-dll"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
            cfg = "host",
        ),
    },
    toolchains = [
        "//bzl/mono:toolchain_type",
        "@unity-version//:toolchain_type",
    ],
)

# These rules are under development and are not suitable for production use yet.
def csharp_library(name, srcs, deps = [], visibility = None):
    """Bazel rule to encapsulate a set of .cs files and track dependencies.

    Args:
      name (string): Target name.
      srcs (Label): csharp files to compile.
      deps (List[Label]): csharp_library dependencies.
      visibility (Label): Bazel package visibility.
    """

    _csharp_library(
        name = name,
        srcs = srcs,
        deps = deps,
        visibility = visibility,
    )

def unity_csharp_dll(name, srcs, dll_name = None, outs = [], deps = [], **kwargs):
    """Bazel rule to generate a csharp .dll file from a set of C# sources.

    Args:
      name (string): Target name.
      dll_name (string): Dll basename.
      src (Label): Unity package to unpack.
      csopts (List[string]): C# compile options passed to Monodevelop.
    """

    # If target_compatible_with is not specified, require unity installations.
    if "target_compatible_with" not in kwargs:
        kwargs["target_compatible_with"] = [
            "@unity-version//:unity-installed-constraint",
        ]

    # Prefix output directory with target name.
    fullouts = ["%s.dll" % (dll_name if dll_name else name)]

    _unity_csharp_dll(
        name = name,
        srcs = srcs,
        deps = deps,
        outs = fullouts,
        **kwargs
    )
