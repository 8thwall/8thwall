load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//bzl/js:js.bzl", "js_files_provider")

_WasmCompileInfo = provider(fields = ["kind", "is_dynamic_library", "deps"])

def _wasm_impl(target, ctx, is_dynamic_library, target_deps, options, html, embind, source_map_base, single_file, prejs_files, postjs_files):
    opts = list(options)
    cc_info = cc_common.merge_cc_infos(
        direct_cc_infos = [CcInfo(compilation_context = target[CcInfo].compilation_context)],
        cc_infos = [dep[CcInfo] for dep in target_deps],
    )

    toolchain = find_cpp_toolchain(ctx)

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    if toolchain.cpu not in ["wasm", "asmjs", "js"]:
        return struct(files = depset())

    if "nodejs" in ctx.features or "emrun" in ctx.features:
        # This is a request to wrap the output for nodejs or emrun.
        single_file = 1
        jsOutputName = ctx.label.name
    else:
        jsOutputName = ctx.label.name + ".js"

    inputLibs = []
    binary_rule = target

    outputs = []
    jsHtmlOutputName = ctx.label.name + ".html"

    jsWasmOutputName = ctx.label.name + ".wasm"

    sourceMapOutputName = jsWasmOutputName + ".map"

    compiler_options = []

    if is_dynamic_library:
        jsWasmOutputName = "lib{}.so".format(jsWasmOutputName)

    if not is_dynamic_library and (not html or not single_file):
        jsOutput = ctx.actions.declare_file(jsOutputName)
        outputs.append(jsOutput)

    symbolLevel = 0
    mapBase = False
    compiler_options = [opt for opt in compiler_options]
    for index, opt in enumerate(compiler_options):
        if opt.startswith("--source-map-base"):
            mapBase = opt.split(" ", 2)[1]

    if single_file:
        opts.extend(["--memory-init-file", "0", "-sSINGLE_FILE=1"])
    else:
        opts.extend(["--memory-init-file", "1"])

    # Add the inputs.
    for inputLib in inputLibs:
        opts.extend([
            inputLib.path,
        ])

    if not single_file:
        jsWasmOutput = ctx.actions.declare_file(jsWasmOutputName)
        outputs.append(jsWasmOutput)

    if embind:
        opts.extend(["--bind"])

    if html:
        jsHtmlOutput = ctx.actions.declare_file(jsHtmlOutputName)
        outputs.append(jsHtmlOutput)

    extra_files = []
    for prejs in prejs_files:
        opts.extend(["--pre-js", prejs.path])
        extra_files.append(prejs)

    for postjs in postjs_files:
        opts.extend(["--post-js", postjs.path])
        extra_files.append(postjs)

    if ctx.var["COMPILATION_MODE"] == "dbg" and not single_file:
        # Emit source maps in --copt=dbg mode if single_file=0 (supported configuration)
        opts.extend(["--source-map-base", source_map_base])
        sourceMap = ctx.actions.declare_file(sourceMapOutputName)
        outputs.append(sourceMap)

    # When building for pthread, we need to produce the worker files as well
    if "pthread" in ctx.features:
        outputs.append(ctx.actions.declare_file(ctx.label.name + ".worker.js"))

    objects = []
    pic_objects = []
    for cc_out in binary_rule[OutputGroupInfo].compilation_outputs.to_list():
        # Separate objects and pic objects. Some objects will be in both.
        path = cc_out.path
        if path.endswith(".pic.o") or path.endswith(".o") and not path.endswith(".nopic.o"):
            pic_objects.append(cc_out)
        if path.endswith(".o") and not path.endswith(".pic.o"):
            objects.append(cc_out)
    compilation_outputs = cc_common.create_compilation_outputs(objects = depset(objects), pic_objects = depset(pic_objects))

    if is_dynamic_library:
        # If the source is a dynamic library and -sSIDE_MODULE or (MAIN) are not specified, add it automatically.
        for opt in opts:
            if opt.startswith("-sSIDE_MODULE") or opt.startswith("-sMAIN_MODULE"):
                break
        opts.append("-sSIDE_MODULE")

    if html:
        primaryOutputName = jsHtmlOutputName
    elif is_dynamic_library:
        primaryOutputName = ctx.label.name + ".wasm"
    else:
        primaryOutputName = jsOutputName

    cc_common.link(
        name = primaryOutputName,
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = toolchain,
        compilation_outputs = compilation_outputs,
        linking_contexts = [cc_info.linking_context],
        output_type = "dynamic_library" if is_dynamic_library else "executable",
        additional_outputs = outputs,
        additional_inputs = extra_files,
        user_link_flags = opts,
    )

    return outputs

def _wasm_binary_impl(ctx):
    opts = list(ctx.attr.opts)
    opts.extend(["-s{}".format(opt) for opt in ctx.attr.emopts])

    kind = "cc_test" if ctx.attr._is_test else "cc_binary"
    target = ctx.attr.cc_test if kind == "cc_test" else ctx.attr.cc_binary

    wasm_files = _wasm_impl(
        target = target,
        ctx = ctx,
        is_dynamic_library = target[_WasmCompileInfo].is_dynamic_library,
        target_deps = target[_WasmCompileInfo].deps,
        options = opts,
        html = ctx.attr.html,
        embind = ctx.attr.embind,
        source_map_base = ctx.attr.source_map_base,
        single_file = ctx.attr.single_file,
        prejs_files = ctx.files.prejs,
        postjs_files = ctx.files.postjs,
    )
    return [
        js_files_provider(transitive_srcs = depset(wasm_files[:1]), runfiles = ctx.runfiles(), includes = depset([ctx.bin_dir.path])),
        DefaultInfo(files = depset(wasm_files), executable = wasm_files[0]),
    ]

def _wasm_cc_aspect_impl(target, ctx):
    """Aspect that does the wasm linking step."""
    if ctx.rule.kind != "cc_binary" and ctx.rule.kind != "cc_test":
        # It is then a wasm_binary or wasm_test rule. If so, pass through the output.
        return [OutputGroupInfo(wasm_files = target[DefaultInfo].files)]

    wasm_files = _wasm_impl(
        target = target,
        ctx = ctx,
        is_dynamic_library = (ctx.rule.kind != "cc_test" and ctx.rule.attr.linkshared),
        target_deps = ctx.rule.attr.deps,
        options = ctx.attr._flags[BuildSettingInfo].value,
        html = False,
        embind = False,
        source_map_base = "/",
        single_file = False,
        prejs_files = [],
        postjs_files = [],
    )
    return [OutputGroupInfo(wasm_files = depset(wasm_files))]

wasm_cc_aspect = aspect(
    implementation = _wasm_cc_aspect_impl,
    attr_aspects = ["cc_binary", "cc_test", "wasm_binary", "wasm_test"],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    attrs = {
        "_flags": attr.label(default = "//bzl/wasm:flag", providers = [BuildSettingInfo]),
    },
    fragments = ["cpp"],
)

def _wasm_compile_aspect_impl(target, ctx):
    """Aspect that provides info from a cc_binary or cc_test rule."""
    if ctx.rule.kind != "cc_binary" and ctx.rule.kind != "cc_test":
        # Nothing to do.
        return []

    return [
        _WasmCompileInfo(
            kind = ctx.rule.kind,
            is_dynamic_library = (ctx.rule.kind != "cc_test" and ctx.rule.attr.linkshared),
            deps = ctx.rule.attr.deps,
        ),
    ]

_wasm_compile_aspect = aspect(
    implementation = _wasm_compile_aspect_impl,
    attr_aspects = ["cc_binary", "cc_test"],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    fragments = ["cpp"],
)

def _get_attrs(test):
    return {
        "cc_test" if test else "cc_binary": attr.label(
            mandatory = True,
            providers = [CcInfo, DebugPackageInfo, _WasmCompileInfo],
            aspects = [_wasm_compile_aspect],
        ),
        "opts": attr.string_list(
            default = [],
        ),
        "emopts": attr.string_list(
            default = [],
        ),
        "html": attr.bool(
            default = False,
        ),
        "embind": attr.bool(
            default = False,
        ),
        "debug_symbol": attr.int(
            default = -1,
        ),
        "source_map_base": attr.string(
            default = "/",
        ),
        "single_file": attr.bool(
            default = False,
        ),
        "prejs": attr.label(
            allow_single_file = True,
        ),
        "postjs": attr.label_list(
            allow_files = True,
        ),
        "_is_test": attr.bool(
            default = test,
        ),
        "_cc_toolchain": attr.label(default = Label("@bazel_tools//tools/cpp:current_cc_toolchain")),
    }

_wasm_binary = rule(
    attrs = _get_attrs(test = False),
    fragments = ["cpp"],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    implementation = _wasm_binary_impl,
)

_wasm_test = rule(
    attrs = _get_attrs(test = True),
    fragments = ["cpp"],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    implementation = _wasm_binary_impl,
    test = True,
)

def wasm_binary(**kwargs):
    # If target_compatible_with is not specified, require wasm as a sane default.
    if "target_compatible_with" not in kwargs:
        kwargs["target_compatible_with"] = ["//bzl/crosstool:wasm"]

    _wasm_binary(**kwargs)

def wasm_test(**kwargs):
    # If target_compatible_with is not specified, require wasm as a sane default.
    if "target_compatible_with" not in kwargs:
        kwargs["target_compatible_with"] = ["//bzl/crosstool:wasm"]

    _wasm_test(**kwargs)
