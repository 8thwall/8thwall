"""
Rules for building JavaScript code using webpack.

js_binary: Builds a JavaScript binary.
js_cli: Builds a JavaScript CLI tool.
js_test: Builds a JavaScript test suite.
js_library: Represents a unit of JavaScript source code.
"""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

NpmPackageInfo = provider(
    "The 'workspace' field contains the name of the external repository rule, such as 'npm-c8'.",
    fields = ["workspace"],
)

def _js_files_provider_init(
        *,
        transitive_srcs = depset(),
        runfiles,
        includes = depset(),
        node_opts = depset()):
    return {
        "transitive_srcs": transitive_srcs,
        "runfiles": runfiles,
        "includes": includes,
        "node_opts": node_opts,
    }

js_files_provider, _new_js_files_provider = provider(
    doc = "Transitive files for js rules",
    fields = {
        "transitive_srcs": "A list of transitive sources to compile",
        "runfiles": "Data files needed at runfile",
        "includes": "Additional directories to use for include searches",
        "node_opts": "Node options to pass to the node binary",
    },
    init = _js_files_provider_init,
)

def _default_target_transition_impl(settings, attr):
    is_exec_configuration = settings["//command_line_option:is exec configuration"]
    target_setting = settings["//bzl/js:target"]

    # We only apply the target attribute from the rule if the rule is top level, or we're using
    # the rule as part of the build process.
    should_apply_target_attr = is_exec_configuration or target_setting == "default"

    new_target = attr.target if should_apply_target_attr else settings["//bzl/js:target"]

    # We remove these features because they are intended to only apply to wasm_binary at the top
    # level, not when depended on by a js_binary/library/test.
    features_to_remove = ["nodejs", "emrun"]

    new_features = [x for x in settings["//command_line_option:features"] if x not in features_to_remove]

    return {
        "//bzl/js:target": new_target,
        "//command_line_option:features": new_features,
    }

_default_target_transition = transition(
    implementation = _default_target_transition_impl,
    inputs = ["//bzl/js:target", "//command_line_option:is exec configuration", "//command_line_option:features"],
    outputs = ["//bzl/js:target", "//command_line_option:features"],
)

def _webpack_js(ctx):
    if (ctx.attr._buildonly):
        index_files = [f.path for f in ctx.files.srcs if not f.path.endswith(".d.ts")]
    elif (len(ctx.files.main) == 0):
        index_files = [f.path for f in ctx.files.srcs if f.basename == "index.js"]
    else:
        index_files = [f.path for f in ctx.files.main]

    target_setting = ctx.attr._target_setting[BuildSettingInfo].value

    is_host = ctx.var["BINDIR"].startswith("bazel-out/host/")

    if target_setting == "default" or is_host:
        # The default_target_transition didn't occur, likely because we are
        # building this js_cli as an exec platform tool, not as a target
        # output. In this case, use the value from ctx.attr.target.
        target_setting = ctx.attr.target

    if (len(index_files) == 0):
        fail(msg = "js_binary requires at least one main or index.js source.")

    if ctx.attr.target == "web" and ctx.attr.include_web_types:
        fail("Cannot set include_web_types with target=\"web\", DOM types are already included")

    # Frequently a library will be a single source in a rule with the same name. This would cause
    # the output filename.js to match the input filename.js which bazel disallows. So if we do
    # buildonly, we suffix the output so that the build product is filename-build.js.
    buildsuffix = "-build" if ctx.attr._buildonly else ""

    outs = [
        ctx.actions.declare_file("%s%s.js" % (ctx.label.name, buildsuffix)),
    ]
    outpattern = outs[0].path

    # Execute the wrapper script.
    if (ctx.attr._cli and len(index_files) != 1):
        fail(msg = "js_cli requires exactly one main or index.js source.")

    # --features=sourcemap takes precedence over --features=nosourcemap, but if neither are
    # specified, the default is to output source maps.
    sourcemap = "sourcemap" in ctx.features or (not "nosourcemap" in ctx.features)
    if sourcemap:
        outs.append(ctx.actions.declare_file("%s%s.js.map" % (ctx.label.name, buildsuffix)))

    inspect_setting = ctx.attr._inspect_setting[BuildSettingInfo].value
    inspect_opt_str = ""
    if inspect_setting == "brk":
        inspect_opt_str = "--inspect-brk --preserve-symlinks"
    elif inspect_setting == "yes":
        inspect_opt_str = "--inspect --preserve-symlinks"

    if ctx.attr.webpack_analyze:
        outs.append(ctx.actions.declare_file("%s-webpack-stats.json" % ctx.label.name))
        outs.append(ctx.actions.declare_file("%s-webpack-report.html" % ctx.label.name))

    ts_declaration = ctx.attr.export_library and index_files[0].endswith(".ts")
    if ts_declaration:
        outs.append(ctx.actions.declare_file("%s.d.ts" % ctx.label.name))

    srcs = _get_transitive_srcs(ctx.files.srcs + ctx.files.main, ctx.attr.deps)

    src_includes = []
    for src in ctx.files.srcs + ctx.files.main:
        if src.root.path:
            src_includes.append(src.root.path)

    includes = depset(
        src_includes,
        transitive = [dep[js_files_provider].includes for dep in ctx.attr.deps],
    )

    inputs = srcs.to_list() + ctx.files.npm_rule

    if target_setting == "node":
        inputs.extend(ctx.files._generated_node)
    else:
        inputs.extend(ctx.files._generated_browser)

    if ctx.file.bundle_header:
        inputs.append(ctx.file.bundle_header)
    if ctx.file.bundle_footer:
        inputs.append(ctx.file.bundle_footer)

    if ctx.attr._test:
        tools = depset(ctx.files._webpack_build + ctx.files._npm_mocha_chai)
    else:
        tools = ctx.files._webpack_build

    string_replacement_path = ""
    if ctx.attr.string_replacements:
        replacement_files = ctx.attr.string_replacements[DefaultInfo].files.to_list()
        string_replacement_path = [f.path for f in replacement_files if f.path.endswith("js")][0]

        # make this file available for import during webpack-build.js invoke
        for file in replacement_files:
            inputs.append(file)

    node_path_override = ""
    externals = ctx.attr.externals

    if ctx.attr.externalize_npm:
        if not ctx.attr._test:
            fail("externalize_npm can only be used with js_test rules")
        if externals != "":
            fail("Cannot use externals and externalize_npm on the same rule")
        externals = "*"

        node_path_override = """
export NODE_PATH=$(dirname $(realpath "$RUNFILES_DIR/{workspace}/external/{npm_folder}/fingerprint.sha1"))/node_modules
""".format(
            workspace = ctx.workspace_name,
            npm_folder = ctx.attr.npm_rule.label.workspace_name,
        )

    ctx.actions.run(
        inputs = inputs,
        outputs = outs,
        executable = ctx.executable._webpack_build,
        # so action_env is available in webpack-build.js
        use_default_shell_env = True,
        env = {
            "NODE_OPTIONS": "--max-old-space-size=8096",
        },
        arguments = [
            "--target=%s" % target_setting,
            "--includewebtypes=%s" % ("1" if target_setting == "web" or ctx.attr.include_web_types else "0"),
            "--outpattern=%s" % (outpattern),
            "--entries=%s" % (",".join(index_files)),
            "--mode=%s" % ctx.attr.webpack_mode,
            "--npmrule=%s" % (ctx.attr.npm_rule.label if ctx.attr.npm_rule else ""),
            "--commonjs=%s" % ("1" if ctx.attr.commonjs else "0"),
            "--analyze=%s" % ("1" if ctx.attr.webpack_analyze else "0"),
            "--sourcemap=%s" % ("1" if sourcemap else "0"),
            "--headerpath=%s" % (ctx.file.bundle_header.path if ctx.file.bundle_header else ""),
            "--footerpath=%s" % (ctx.file.bundle_footer.path if ctx.file.bundle_footer else ""),
            "--mangle=%s" % ("1" if ctx.attr.mangle else "0"),
            "--polyfill=%s" % (ctx.attr.polyfill),
            "--exportLibrary=%s" % ("1" if ctx.attr.export_library else "0"),
            "--externals=%s" % externals,
            "--externalsType=%s" % ctx.attr.externalsType,
            "--stringReplacements=%s" % string_replacement_path,
            "--esnext=%s" % ("1" if ctx.attr.esnext else "0"),
            "--tsDeclaration=%s" % ("1" if ts_declaration else "0"),
            "--fullDts=%s" % ("1" if ctx.attr.full_dts else "0"),
        ] + ["-I{}".format(x) for x in includes.to_list()],
        tools = tools,
        mnemonic = "GenJsBinary",
    )

    expanded_node_opts = [ctx.expand_location(opt) for opt in ctx.attr.node_opts]
    node_opts = depset(expanded_node_opts, transitive = [dep[js_files_provider].node_opts for dep in ctx.attr.deps])
    node_opt_wrapped = ["'{}'".format(flag) if " " in flag else flag for flag in node_opts.to_list()]
    node_opt_string = " ".join(node_opt_wrapped)

    if ctx.attr._cli:
        # Wrap flags in single quotes if they contain whitespace.
        out_wrapper = ctx.actions.declare_file(ctx.label.name)
        ctx.actions.write(output = out_wrapper, content = """#!/bin/bash --norc
set -eu
export RUNFILES_DIR=${{RUNFILES_DIR:-$(realpath $0.runfiles)}}
${{RUNFILES_DIR}}/{workspace}/{node} {inspect_opt} {node_opts} ${{RUNFILES_DIR}}/{workspace}/{jsfile} "$@"
""".format(
            workspace = ctx.workspace_name,
            node = ctx.file.target_node.short_path,
            inspect_opt = inspect_opt_str,
            node_opts = node_opt_string,
            jsfile = outs[0].short_path,
        ), is_executable = True)
        return ([out_wrapper], outs)
    elif ctx.attr._test:
        out_wrapper = ctx.actions.declare_file(ctx.label.name)
        ctx.actions.write(
            output = out_wrapper,
            content = """#!/bin/bash --norc
set -eu
export RUNFILES_DIR=${{RUNFILES_DIR:-$(realpath $0.runfiles)}}
export TEST_SRCDIR=${{TEST_SRCDIR:-$RUNFILES_DIR}}

REPORTER_OPTS=()
if [[ -n "${{XML_OUTPUT_FILE:-}}" ]]; then
    # If running within 'bazel test', we also output the test results to an junit XML file.
    REPORTER_OPTS+=("--reporter" "mocha-multi-reporters" "--reporter-options" "configFile={reporters}")
    export MOCHA_FILE=$XML_OUTPUT_FILE
fi
{node_path_override}
${{RUNFILES_DIR}}/_main/{node_workspace}/{node} {inspect_opt} {node_opts} ${{RUNFILES_DIR}}/{workspace}/{mocha} ${{REPORTER_OPTS[0]+"${{REPORTER_OPTS[@]}}"}} ${{RUNFILES_DIR}}/{workspace}/{jsfile} "$@"
""".format(
                node = ctx.file.target_node.short_path,
                inspect_opt = inspect_opt_str,
                node_opts = node_opt_string,
                mocha = ctx.file._mocha_bin.short_path,
                reporters = ctx.file._mocha_reporters.short_path,
                workspace = ctx.workspace_name,
                node_workspace = ctx.file.target_node.owner.workspace_root,
                jsfile = outs[0].short_path,
                node_path_override = node_path_override,
                is_executable = True,
            ),
        )
        return ([out_wrapper], outs)
    else:
        return (outs, [])

def _resolve_npm_provider(ctx):
    npm_workspace = ctx.attr.npm_rule.label.workspace_name if ctx.attr.npm_rule else None

    for dep in ctx.attr.deps:
        if NpmPackageInfo in dep:
            dep_npm_provider = dep[NpmPackageInfo]
            if not npm_workspace:
                npm_workspace = dep_npm_provider.workspace
            elif dep_npm_provider.workspace and npm_workspace != dep_npm_provider.workspace:
                fail(msg = "npm_rule conflicts detected: '%s' vs '%s' (%s)" % (npm_workspace, dep_npm_provider.workspace, dep.label))

    return NpmPackageInfo(workspace = npm_workspace)

def _js_binary_impl(ctx):
    outs, runfiles = _webpack_js(ctx)

    runfiles = ctx.runfiles(files = outs + ctx.files.data + runfiles)
    runfiles = runfiles.merge_all([dep[js_files_provider].runfiles for dep in ctx.attr.deps])

    out_structs = []

    should_forward_npm = True

    expanded_node_opts = [ctx.expand_location(opt) for opt in ctx.attr.node_opts]
    node_opts = depset(expanded_node_opts, transitive = [dep[js_files_provider].node_opts for dep in ctx.attr.deps])

    if (ctx.attr._cli):
        runfiles = runfiles.merge(ctx.runfiles(files = ctx.files.target_node))
        runfiles = runfiles.merge(ctx.attr.target_node[DefaultInfo].default_runfiles)
        out_structs.append(DefaultInfo(
            files = depset(outs),
            executable = outs[0],
            runfiles = runfiles,
        ))
    else:
        out_structs.append(DefaultInfo(files = depset(outs), runfiles = runfiles))

        if ctx.attr.export_library:
            src_includes = []
            for src in ctx.files.srcs + ctx.files.main:
                if src.root.path:
                    src_includes.append(src.root.path)
            out_structs.append(
                js_files_provider(
                    transitive_srcs = depset(outs),
                    runfiles = runfiles,
                    includes = depset([ctx.bin_dir.path]),
                    node_opts = node_opts,
                ),
            )
            should_forward_npm = False

    if should_forward_npm:
        out_structs.append(_resolve_npm_provider(ctx))

    return out_structs

def _js_test_impl(ctx):
    outs, runfiles = _webpack_js(ctx)

    runfiles = ctx.runfiles(files = ctx.files.data + runfiles + (ctx.files.npm_rule if ctx.attr.externalize_npm else []))
    runfiles = runfiles.merge_all([ctx.runfiles(files = ctx.files._mocha_bin), ctx.attr._target_mocha[DefaultInfo].default_runfiles])
    runfiles = runfiles.merge_all([dep[js_files_provider].runfiles for dep in ctx.attr.deps])
    runfiles = runfiles.merge(ctx.runfiles(files = ctx.files.target_node))
    runfiles = runfiles.merge(ctx.attr.target_node[DefaultInfo].default_runfiles)

    out_structs = [_resolve_npm_provider(ctx)]

    out_structs.append(DefaultInfo(
        files = depset(outs),
        executable = outs[-1],
        runfiles = runfiles,
    ))

    return out_structs

def _get_transitive_srcs(srcs, deps):
    return depset(srcs, transitive = [dep[js_files_provider].transitive_srcs for dep in deps])

def _js_library_impl(ctx):
    srcs = _get_transitive_srcs(ctx.files.srcs, ctx.attr.deps)

    runfiles = ctx.runfiles(files = ctx.files.data)
    runfiles = runfiles.merge_all([dep[js_files_provider].runfiles for dep in ctx.attr.deps])

    src_includes = []
    for src in ctx.files.srcs:
        if src.root.path:
            src_includes.append(src.root.path)

    includes = depset(src_includes, transitive = [dep[js_files_provider].includes for dep in ctx.attr.deps])
    expanded_node_opts = [ctx.expand_location(opt, ctx.attr.data) for opt in ctx.attr.node_opts]
    node_opts = depset(expanded_node_opts, transitive = [dep[js_files_provider].node_opts for dep in ctx.attr.deps])

    return [
        js_files_provider(transitive_srcs = srcs, runfiles = runfiles, includes = includes, node_opts = node_opts),
        _resolve_npm_provider(ctx),
    ]

def _js_build_attrs(cli, buildonly, test):
    return {
        "main": attr.label_list(
            allow_files = True,
            default = [],
        ),
        "srcs": attr.label_list(
            allow_files = True,
            default = [],
        ),
        "deps": attr.label_list(
            default = [],
            providers = [js_files_provider],
        ),
        "data": attr.label_list(
            allow_files = True,
            default = [],
        ),
        "node_bin": attr.label(default = "@nodejs_host//:node_bin", allow_single_file = True),
        "node_opts": attr.string_list(default = []),
        "_buildonly": attr.bool(default = buildonly),
        "_cli": attr.bool(default = cli),
        "_test": attr.bool(default = test),
        "webpack_mode": attr.string(default = "production"),
        "commonjs": attr.bool(default = False),
        "_generated_browser": attr.label(
            default = Label("@the8thwall//bzl/js/generated:generated-browser"),
        ),
        "_generated_node": attr.label(
            default = Label("@the8thwall//bzl/js/generated:generated-node"),
        ),
        "_npm_mocha_chai": attr.label(
            default = Label("@npm-mocha//:npm-mocha") if test else None,
        ),
        "_mocha_bin": attr.label(
            default = Label("@npm-mocha//:node_modules/mocha/bin/mocha") if test else None,
            allow_single_file = True,
        ),
        "_mocha_reporters": attr.label(
            default = Label("@the8thwall//bzl/js:mocha-reporters") if test else None,
            allow_single_file = True,
        ),
        "_target_mocha": attr.label(
            default = Label("@the8thwall//bzl/js:mocha") if test else None,
            allow_single_file = True,
            executable = True,
            cfg = "target",
        ),
        "target_node": attr.label(
            default = Label("@the8thwall//bzl/node:node"),
            allow_single_file = True,
            executable = True,
            cfg = "target",
        ),
        "_webpack_build": attr.label(
            default = Label("@the8thwall//bzl/js:webpack-build"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "npm_rule": attr.label(default = None),
        "webpack_analyze": attr.bool(default = False),
        "export_library": attr.bool(default = False),
        "full_dts": attr.bool(default = False),
        "bundle_header": attr.label(default = None, allow_single_file = True),
        "bundle_footer": attr.label(default = None, allow_single_file = True),
        "mangle": attr.bool(default = True),
        "polyfill": attr.string(default = "<default>"),
        "externals": attr.string(default = ""),
        "externalsType": attr.string(default = "commonjs"),
        "esnext": attr.bool(default = False),
        # The target is specified by the --//bzl/js:target=value flag, where
        # value can be one of 'web', 'node', or 'default'. The first two
        # parameters will choose the target build, whereas 'default' will
        # transition to either 'web' or 'node' based on the value of the
        # 'target' attribute.
        "_target_setting": attr.label(
            default = "//bzl/js:target",
            providers = [BuildSettingInfo],
        ),
        "_inspect_setting": attr.label(
            default = "//bzl/js:inspect",
            providers = [BuildSettingInfo],
        ),
        # The target to use when --//bzl/js:target=default.
        "target": attr.string(default = "node" if (cli or test) else "web"),
        # Use include_web_types if you are building for target = "node", but
        # are knowingly building code that won't run in node. Good examples are
        # for testing certain node-compatible functionality in a file that also
        # has browser-only methods.
        "include_web_types": attr.bool(default = False),
        "string_replacements": attr.label(default = None),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "externalize_npm": attr.bool(default = False),
    }

_js_binary = rule(
    attrs = _js_build_attrs(False, False, False),
    cfg = _default_target_transition,
    implementation = _js_binary_impl,
)

_js_cli = rule(
    attrs = _js_build_attrs(True, False, False),
    cfg = _default_target_transition,
    executable = True,
    implementation = _js_binary_impl,
)

_js_library = rule(
    attrs = _js_build_attrs(False, True, False),
    cfg = _default_target_transition,
    implementation = _js_library_impl,
)

_js_node_library = rule(
    attrs = _js_build_attrs(True, True, False),
    cfg = _default_target_transition,
    implementation = _js_library_impl,
)

_js_test = rule(
    attrs = _js_build_attrs(False, False, True),
    cfg = _default_target_transition,
    test = True,
    implementation = _js_test_impl,
)

def js_library(name, srcs, deps = [], npm_rule = None, **kwargs):
    _js_library(
        name = name,
        srcs = srcs,
        deps = deps,
        npm_rule = npm_rule,
        **kwargs
    )

def js_node_library(name, srcs, deps = [], **kwargs):
    _js_node_library(
        name = name,
        srcs = srcs,
        deps = deps,
        **kwargs
    )

def js_binary(
        name,
        main = [],
        srcs = [],
        deps = [],
        data = [],
        commonjs = False,
        npm_rule = None,
        webpack_analyze = False,
        bundle_header = None,
        bundle_footer = None,
        polyfill = None,
        target = "web",  # Can be "web" or "node"
        externals = "",
        export_library = False,
        string_replacements = None,
        esnext = False,
        **kwargs):
    _js_binary(
        name = name,
        main = main,
        srcs = srcs,
        deps = deps,
        data = data,
        commonjs = commonjs,
        export_library = export_library,
        webpack_mode = select({
            "@the8thwall//bzl/js:release": "production",
            "//conditions:default": "development",
        }),
        npm_rule = npm_rule,
        webpack_analyze = webpack_analyze,
        bundle_header = bundle_header,
        bundle_footer = bundle_footer,
        polyfill = polyfill,
        target = target,
        externals = externals,
        string_replacements = string_replacements,
        esnext = esnext,
        **kwargs
    )

def js_cli(
        name,
        main = [],
        srcs = [],
        deps = [],
        data = [],
        node = "@nodejs_host//:node_bin",
        commonjs = False,
        npm_rule = None,
        webpack_analyze = False,
        bundle_header = None,
        bundle_footer = None,
        externals = "",
        esnext = False,
        **kwargs):
    _js_cli(
        name = name,
        main = main,
        srcs = srcs,
        deps = deps,
        data = data,
        commonjs = commonjs,
        node_bin = node,
        webpack_mode = select({
            "@the8thwall//bzl/js:release": "production",
            "//conditions:default": "development",
        }),
        npm_rule = npm_rule,
        target_node = select({
            "@the8thwall//bzl/node:source-built-true": Label("@node//:node-bin"),
            "@the8thwall//bzl/node:source-built-false": Label("@the8thwall//bzl/node:node"),
        }),
        webpack_analyze = webpack_analyze,
        bundle_header = bundle_header,
        bundle_footer = bundle_footer,
        externals = externals,
        esnext = esnext,
        **kwargs
    )

def js_test(name, main = [], srcs = [], deps = [], data = [], npm_rule = None, **kwargs):
    _js_test(
        name = name,
        main = main,
        srcs = srcs,
        deps = deps,
        target_node = select({
            "@the8thwall//bzl/node:source-built-true": Label("@node//:node-bin"),
            "@the8thwall//bzl/node:source-built-false": Label("@the8thwall//bzl/node:node"),
        }),
        data = data,
        npm_rule = npm_rule,
        **kwargs
    )
