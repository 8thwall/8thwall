"""
Build rules for capnproto
"""

load("//bzl/unity:unity.bzl", "csharp_library")
load("//bzl/js:js.bzl", "js_files_provider")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_java//java:defs.bzl", "java_library")
load("@rules_python//python:defs.bzl", "py_library")

capnp_files_provider = provider(fields = ["includes", "inputs"])

def _get_transitive_srcs(srcs, deps):
    return depset(srcs, transitive = [dep[js_files_provider].transitive_srcs for dep in deps])

# Uses the capnp cross-language source generator (@capnproto//:capnp) to
# generate source files in multiple languages.
def _impl_capnp_src_gen(ctx):
    includes = depset(
        ctx.attr.includes +
        [ctx.attr.capnp_cc_system_include] +
        [ctx.attr.capnp_cs_system_include] +
        [ctx.attr.capnp_java_system_include],
        transitive = [dep[capnp_files_provider].includes for dep in ctx.attr.deps],
    )
    inputs = depset(ctx.files.srcs + ctx.files.data + ctx.files.deps, transitive = [dep[capnp_files_provider].inputs for dep in ctx.attr.deps])
    inputsList = inputs.to_list()
    capnp_srcs = depset([s.path for s in ctx.files.srcs])

    # The capnp-js plugin generates typescript files and then transpiles them back to javascript. In
    # order for transpiler to have all the information it needs, all the capnp files must be
    # supplied as args to the capnp compiler. Other platforms, including TS, don't need this.
    if ("capnpc-js-plugin" in ctx.executable.capnpc_plugin.path):
        capnp_srcs = depset(transitive = [capnp_srcs, depset([x.path for x in inputsList if x.path.endswith(".capnp")])])

    capnp_srcs = capnp_srcs.to_list()

    cc_out = "-o%s:%s" % (ctx.executable.capnpc_plugin.path, ctx.var["GENDIR"])
    args = ["compile", "--verbose", "--no-standard-import", cc_out]
    include_flags = ["-I" + inc for inc in includes.to_list()]

    ctx.actions.run(
        inputs = inputsList + ctx.files.capnpc_schemas + ctx.attr.capnpc_plugin.default_runfiles.files.to_list(),
        tools = ctx.files.capnpc_plugin + ctx.files._node,
        outputs = ctx.outputs.outs,
        executable = ctx.executable.capnpc_exe,
        arguments = args + include_flags + capnp_srcs,
        env = {
            "NODE": ctx.file._node.path,
        },
        mnemonic = "GenCapnp",
    )

    return [
        capnp_files_provider(includes = includes, inputs = inputs),
        js_files_provider(transitive_srcs = _get_transitive_srcs(ctx.outputs.outs, ctx.attr.deps), runfiles = ctx.runfiles(), includes = depset([ctx.bin_dir.path])),
        DefaultInfo(files = depset(ctx.outputs.outs)),
    ]

# Uses the capnp cross-language source generator (@capnproto//:capnp) to
# generate source files in a single language, given the supplied language
# plugin.
_capnp_src_gen = rule(
    attrs = {
        "srcs": attr.label_list(allow_files = True),
        "deps": attr.label_list(providers = [capnp_files_provider]),
        "data": attr.label_list(allow_files = True),
        "includes": attr.string_list(),
        "_node": attr.label(
            executable = True,
            cfg = "host",
            allow_single_file = True,
            default = Label("//bzl/node:node"),
        ),
        "capnpc_exe": attr.label(
            executable = True,
            cfg = "host",
            allow_single_file = True,
            mandatory = True,
            default = Label("@capnproto//:capnp"),
        ),
        "capnpc_plugin": attr.label(
            executable = True,
            cfg = "host",
            allow_single_file = True,
            mandatory = True,
            default = None,
        ),
        "capnpc_schemas": attr.label_list(default = [
            "@capnproto//:capnp-capnp",
            "@capnproto_java//:capnp-java-annotations",
            "@capnproto_python//:capnp-python-annotations",
            "//third_party/capnpcs:capnp-cs-annotations",
        ]),
        "capnp_cc_system_include": attr.string(default = Label("@capnproto//:capnp").workspace_root + "/c++/src"),
        "capnp_cs_system_include": attr.string(default = Label("//third_party/capnpcs:capnpc-cs").workspace_root + "third_party/capnpcs/compiler"),
        "capnp_java_system_include": attr.string(default = Label("@capnproto_java//:capnpc-java").workspace_root + "/compiler/src/main/schema"),
        "outs": attr.output_list(),
    },
    output_to_genfiles = True,
    implementation = _impl_capnp_src_gen,
)

# Capitalize the first char in a string.
def _first_upper(str):
    return str.upper if len(str) < 2 else str[0:1].upper() + str[1:]

# Convert a capnp filename to its expected java outer class by removing the
# capnp suffix, tokenizing on '-' and camel-casing.
#
# e.g. myproto.capnp -> "Myproto"
# e.g. my-proto.capnp-> "MyProto"
def _java_outer_class(capnpfile):
    base = capnpfile.replace(".capnp", "")
    tokens = base.split("-")
    tokensWithFirstCaps = [_first_upper(token) for token in tokens]
    return "".join(tokensWithFirstCaps) + ".java"

# Convert a capnp filename to its expected cs filename.
#
# e.g. myproto.capnp -> "myproto.capnp.cs"
# e.g. my-proto.capnp-> "my-proto.capnp.cs"
def _cs_filename(capnpfile):
    return capnpfile + ".cs"

def cc_capnp_library(
        name,
        srcs = [],
        deps = [],
        data = [],
        include = None,
        **kargs):
    """Bazel rule to create a C++ capnproto library from capnp source files
    """

    includes = []
    if include != None:
        includes = [include]

    outs = ([s + ".h" for s in srcs] +
            [s + ".c++" for s in srcs])

    providers = _capnp_src_gen(
        name = name + "_gencapnp_cc",
        srcs = srcs,
        deps = [s + "_gencapnp_cc" for s in deps],
        data = data,
        includes = includes,
        capnpc_exe = "@capnproto//:capnp",
        capnpc_plugin = "@capnproto//:capnpc-c++",
        outs = outs,
        visibility = ["//visibility:public"],
    )
    cc_libs = ["@capnproto//:capnp-lib"]
    cc_library(
        name = name,
        srcs = outs,
        deps = cc_libs + deps,
        includes = includes,
        **kargs
    )

    return providers

def java_capnp_library(
        name,
        srcs = [],
        deps = [],
        data = [],
        **kargs):
    """Bazel rule to create a Java capnproto library from capnp source files
    """

    includes = []

    # This requires the java outer class name to be equal to the capnp filename
    # with the first character capitalized and without the .capnp suffix.
    #
    # e.g. myproto.capnp -> Myproto.java, where Myproto is the outer class.
    # e.g. my-proto.capnp -> MyProto.java, where MyProto is the outer class.
    outs = ([_java_outer_class(s) for s in srcs])

    providers = _capnp_src_gen(
        name = name + "_gencapnp_java",
        srcs = srcs,
        deps = [s + "_gencapnp_java" for s in deps],
        data = data,
        includes = [],
        capnpc_exe = "@capnproto//:capnp",
        capnpc_plugin = "@capnproto_java//:capnpc-java",
        outs = outs,
        visibility = ["//visibility:public"],
    )

    java_libs = ["@capnproto_java//:capnp-runtime-java"]
    java_library(
        name = name,
        srcs = outs,
        deps = java_libs + deps,
        **kargs
    )

    return providers

def cs_capnp_library(
        name,
        srcs = [],
        deps = [],
        data = [],
        **kargs):
    """Bazel rule to create a C# capnproto library from capnp source files
    """

    includes = []

    outs = ([_cs_filename(s) for s in srcs])

    providers = _capnp_src_gen(
        name = name + "_gencapnp_cs",
        srcs = srcs,
        deps = [s + "_gencapnp_cs" for s in deps],
        data = data,
        includes = [],
        capnpc_exe = "@capnproto//:capnp",
        capnpc_plugin = "//third_party/capnpcs:capnpc-cs",
        outs = outs,
        visibility = ["//visibility:public"],
    )

    cs_libs = ["//third_party/capnpcs:capnp-runtime"]
    csharp_library(
        name = name,
        srcs = outs,
        deps = cs_libs + deps,
        **kargs
    )

    return providers

def js_capnp_library(
        name,
        srcs = [],
        deps = [],
        data = [],
        **kargs):
    """Bazel rule to create a TS and JS library from capnp source file
    """
    return _capnp_src_gen(
        name = name,
        srcs = srcs,
        deps = deps,
        data = data,
        includes = [],
        capnpc_exe = "@capnproto//:capnp",
        capnpc_plugin = "//bzl/capnproto:capnpc-js-plugin",
        outs = [s + ".js" for s in srcs],
        visibility = ["//visibility:public"],
    )

def ts_capnp_library(
        name,
        srcs = [],
        deps = [],
        data = [],
        **kargs):
    """Bazel rule to create a TS and JS library from capnp source file
    """
    return _capnp_src_gen(
        name = name,
        srcs = srcs,
        deps = deps,
        data = data,
        includes = [],
        capnpc_exe = "@capnproto//:capnp",
        capnpc_plugin = "//bzl/capnproto:capnpc-ts-plugin",
        outs = [s + ".ts" for s in srcs],
        visibility = ["//visibility:public"],
    )

def python_capnp_library(
        name,
        srcs = [],
        deps = [],
        data = [],
        **kargs):
    """Bazel rule to create a Python capnproto library from capnp source files

    Since pycapnp doesn't compile the .capnp files into a library but load them on the fly,
    we simply offer up the capnp files as data files for the python code to load using capnp.load()

    Currently, pycapnp is provided in tools8/pip3.sh. This allows a pycapnp load() to use the files
    under thirdparty/capnp easily when you are in jupyter. If you are writing a python executatble, see chesspose.py
    and its corresponding BUILD file for a usage within our codebase that references the capnp annotations files put
    in your runfiles.
    """

    # TODO(dat): Do we not have to depend on the c++ capnp? Installing pycapnp from pip does include
    #            its own lib/capnp
    # These schema is required otherwise pycapnp won't be able to read our .capnp files that use these annotations
    pycapnp_req = [
        "@capnproto_python//:capnp-python-annotations",
        "@capnproto//:capnp-capnp",
        "@capnproto_java//:capnp-java-annotations",
        "//third_party/capnpcs:capnp-cs-annotations",
    ]
    py_library(
        name = name,
        srcs = [],
        data = pycapnp_req + srcs,
        deps = deps,
        **kargs
    )

def capnp_library(
        name,
        srcs = [],
        deps = [],
        **kargs):
    cc_deps = [d + ".capnp-cc" for d in deps]
    cs_deps = [d + ".capnp-cs" for d in deps]
    java_deps = [d + ".capnp-java" for d in deps]
    js_deps = [d + ".capnp-js" for d in deps]
    ts_deps = [d + ".capnp-ts" for d in deps]
    python_deps = [d + ".capnp-python" for d in deps]

    cc_capnp_library(
        name = name + ".capnp-cc",
        srcs = srcs,
        deps = cc_deps,
        **kargs
    )

    cs_capnp_library(
        name = name + ".capnp-cs",
        srcs = srcs,
        deps = cs_deps,
        **kargs
    )

    java_capnp_library(
        name = name + ".capnp-java",
        srcs = srcs,
        deps = java_deps,
        **kargs
    )

    js_capnp_library(
        name = name + ".capnp-js",
        srcs = srcs,
        deps = js_deps,
        **kargs
    )

    ts_capnp_library(
        name = name + ".capnp-ts",
        srcs = srcs,
        deps = ts_deps,
        **kargs
    )

    python_capnp_library(
        name = name + ".capnp-python",
        srcs = srcs,
        deps = python_deps,
        **kargs
    )
