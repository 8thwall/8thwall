load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

ShaderFiles = provider("transitive_srcs")

SHADER_DEFAULT_COPTS = [
    # Enable full performance optimizations.
    "-O",
    # Newer GLSL uses layout specifiers for mapping uniforms and in/out
    # variables. These take the form layout(location=0) and
    # layout(binding=0) before variable declarations. The following flags
    # automatically generate them if you don't have them specified in your
    # code, but it would be better to modify your shader.
    "--auto-map-locations",
    "--auto-map-bindings",
] + select({
    "@the8thwall//bzl/conditions:android": ["-DANDROID"],
    "@the8thwall//bzl/conditions:osx": ["-DOSX"],
    "//conditions:default": [],
})

SHADER_DEFAULT_XOPTS = [
    "--no-420pack-extension",
    "--unmap-bindings",
    "--unmap-locations",
]

GLSL_WEBGL1 = ["--version", "100", "--es"]

GLSL_WEBGL2 = ["--version", "300", "--es"]

GLSL_150 = ["--version", "150", "--no-es"]

GLSL_300ES = ["--version", "300", "--es"]

SHADER_DEFAULT_TARGETS = select({
    "//c8/pixels/opengl:with-angle": ["glsl300es"],
    "//bzl/conditions:wasm": ["webgl1", "webgl2"],
    "//bzl/shader:osx-without-angle": ["glsl150"],
    "//conditions:default": ["glsl300es"],
})

SHADER_TARGETS = struct(
    webgl1 = GLSL_WEBGL1,
    webgl2 = GLSL_WEBGL2,
    glsl150 = GLSL_150,
    glsl300es = GLSL_300ES,
)

def _get_transitive_srcs(srcs, deps):
    return depset(srcs, transitive = [dep[ShaderFiles].transitive_srcs for dep in deps])

def _shader_include_impl(ctx):
    srcs = _get_transitive_srcs(ctx.files.srcs, ctx.attr.deps)

    return [
        ShaderFiles(transitive_srcs = srcs),
    ]

def _shader_binary_impl(ctx):
    binaryName = "_libs/%s.spv" % ctx.label.name
    binary = ctx.actions.declare_file(binaryName)

    srcs = _get_transitive_srcs(ctx.files.srcs, ctx.attr.deps)

    ctx.actions.run(
        outputs = [binary],
        inputs = srcs.to_list(),
        executable = ctx.file._shadercc,
        tools = [
            ctx.file._validator,
            ctx.file._optimizer,
        ],
        env = {
            "VALIDATOR": ctx.file._validator.path,
            "OPTIMIZER": ctx.file._optimizer.path,
        },
        arguments = [
            "-G",
            "--quiet",
            "-o",
            binary.path,
            "-I.",
        ] + [x.path for x in ctx.files.srcs] + ctx.attr.copts,
        progress_message = "Compiling and optimizing shader",
        mnemonic = "ShaderCompile",
    )

    targetBasename = ctx.label.name
    targetExtension = ctx.files.srcs[0].extension

    if targetBasename.endswith("-%s" % targetExtension):
        targetName = targetBasename[:-(len(targetExtension) + 1)]
    else:
        targetName = targetBasename

    targets = []
    for (index, target) in enumerate(ctx.attr.targets):
        if not hasattr(SHADER_TARGETS, target):
            fail("Invalid shader target '%s'" % target)
        if index == 0:
            targetFilename = "%s.%s" % (targetName, targetExtension)
        else:
            targetFilename = "%s.%s.%s" % (targetName, target, targetExtension)
        targetFile = ctx.actions.declare_file(targetFilename)
        targets.append(targetFile)
        ctx.actions.run(
            outputs = [targetFile],
            inputs = [binary],
            executable = ctx.file._crosstool,
            arguments = [
                "--output",
                targetFile.path,
                binary.path,
            ] + getattr(SHADER_TARGETS, target) + ctx.attr.xopts,
            progress_message = "Cross-compiling shader target",
            mnemonic = "ShaderCrossCompile",
        )

    return [
        DefaultInfo(files = depset(targets)),
        OutputGroupInfo(
            binary_file = depset([binary]),
            all_files = depset([binary] + targets),
        ),
    ]

_shader_include = rule(
    implementation = _shader_include_impl,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "deps": attr.label_list(
            default = [],
        ),
    },
)

_shader_binary = rule(
    implementation = _shader_binary_impl,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "deps": attr.label_list(
            default = [],
        ),
        "copts": attr.string_list(
            default = [],
        ),
        "xopts": attr.string_list(
            default = [],
        ),
        "targets": attr.string_list(
            default = [],
        ),
        "_validator": attr.label(
            default = Label("@glslang//:glslangValidator"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_optimizer": attr.label(
            default = Label("@spirv_tools//:spirv-opt"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_shadercc": attr.label(
            default = Label("//bzl/shader:shadercc"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_crosstool": attr.label(
            default = Label("//third_party/spirvcross:spirv-cross"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_angle": attr.label(
            default = Label("//c8/pixels/opengl:angle"),
            providers = [BuildSettingInfo],
        ),
    },
)

# Rule for grouping fragments of shader code that are imported into shader_binary rules.
def shader_include(name, srcs, deps = [], visibility = []):
    _shader_include(
        name = name,
        srcs = srcs,
        deps = deps,
        visibility = visibility,
    )

# Builds a shader binary and cross-compiles the output to the language of choice.
def shader_binary(name, srcs = [], deps = [], targets = SHADER_DEFAULT_TARGETS, copts = SHADER_DEFAULT_COPTS, xopts = SHADER_DEFAULT_XOPTS):
    _shader_binary(
        name = name,
        srcs = srcs,
        deps = deps,
        targets = targets,
        copts = copts,
        xopts = xopts,
    )
