# Embed data in your object files. Try to avoid doing this in most situations,
# as it increases binary size, and instead use it when it really makes sense.

load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

def _embedded_data_impl(ctx):
    hFilename = ctx.label.name + ".h"
    hFile = ctx.actions.declare_file(hFilename)

    hLines = ["#pragma once"]
    hLines.append("#ifdef __cplusplus")
    hLines.append("#include <string_view>")
    hLines.append("#endif")

    ccFilename = ctx.label.name + ".cc"
    ccFile = ctx.actions.declare_file(ccFilename)

    toolchain = find_cpp_toolchain(ctx)

    symbolPrefix = ""
    useAsm = ("1" if ctx.attr.use_asm else "0")
    rosection = ".rodata"
    os_osx = ctx.attr._os_osx[platform_common.ConstraintValueInfo]
    os_ios = ctx.attr._os_ios[platform_common.ConstraintValueInfo]
    os_wasm = ctx.attr._os_wasm[platform_common.ConstraintValueInfo]
    is_apple = ctx.target_platform_has_constraint(os_osx) or ctx.target_platform_has_constraint(os_ios)
    if is_apple:
        rosection = "__DATA,__const"
        symbolPrefix = "_"
    elif ctx.target_platform_has_constraint(os_wasm):
        rosection = '.rodata,"",@'
        useAsm = "0"

    ctx.actions.run_shell(
        inputs = ctx.files.srcs,
        outputs = [ccFile],
        tools = [ctx.file._gen_embed_cc],
        command = "%s %s > %s" % (ctx.file._gen_embed_cc.path, " ".join([src.path for src in ctx.files.srcs]), ccFile.path),
        mnemonic = "GenerateEmbedCC",
        progress_message = "Generating " + ccFilename,
        env = {
            "ROSECTION": rosection,
            "SYMBOL_PREFIX": symbolPrefix,
            "USE_ASSEMBLY": useAsm,
        },
    )

    for src in ctx.files.srcs:
        canonicalName = "".join([x.lower() if x.isalnum() else "_" for x in src.basename.elems()])
        words = canonicalName.split("_")
        camelName = "".join([w.capitalize() for w in words])
        symbolName = "embedded%s" % camelName
        hLines.append("#ifdef __cplusplus")
        hLines.append("extern const std::string_view %sView;" % (symbolName))
        hLines.append('extern "C" {')
        hLines.append("#endif")
        hLines.append("extern const uint8_t %sData[];" % symbolName)
        hLines.append("extern const char *const %sCStr;" % symbolName)
        hLines.append("extern const size_t %sSize;" % symbolName)
        hLines.append("#ifdef __cplusplus")
        hLines.append('}  // extern "C"')
        hLines.append("#endif")

    ctx.actions.run_shell(
        inputs = ctx.files.srcs,
        outputs = [hFile],
        command = "cat <<EOF > %s\n%s\nEOF\n" % (hFile.path, "\n".join(hLines)),
        mnemonic = "GenerateEmbeddedHeader",
        progress_message = "Generating " + hFilename,
    )

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    compileContext, compileOutputs = (
        cc_common.compile(
            name = ctx.label.name + "-cc",
            actions = ctx.actions,
            feature_configuration = feature_configuration,
            cc_toolchain = toolchain,
            additional_inputs = ctx.files.srcs,
            public_hdrs = [hFile],
            srcs = [ccFile],
        )
    )

    linkContext, linkOutputs = (
        cc_common.create_linking_context_from_compilation_outputs(
            name = ctx.label.name + "-link",
            actions = ctx.actions,
            feature_configuration = feature_configuration,
            cc_toolchain = toolchain,
            additional_inputs = ctx.files.srcs,
            compilation_outputs = compileOutputs,
        )
    )
    allLinkOutputs = [
        linkOutputs.library_to_link.static_library,
        linkOutputs.library_to_link.pic_static_library,
    ]
    if (toolchain.cpu != "js" and toolchain.cpu != "wasm"):
        allLinkOutputs.append(
            linkOutputs.library_to_link.resolved_symlink_dynamic_library,
        )

    return [
        DefaultInfo(
            files = depset([out for out in allLinkOutputs if out != None]),
        ),
        apple_common.new_objc_provider(),
        CcInfo(
            compilation_context = compileContext,
            linking_context = linkContext,
        ),
    ]

_embedded_data = rule(
    implementation = _embedded_data_impl,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "use_asm": attr.bool(mandatory = True, default = True),
        "_cc_toolchain": attr.label(
            default = Label("@bazel_tools//tools/cpp:current_cc_toolchain"),
        ),
        "_gen_embed_cc": attr.label(
            default = Label("//bzl/utils:gen-embed-cc"),
            allow_single_file = True,
        ),
        "_os_osx": attr.label(
            default = "@platforms//os:osx",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_ios": attr.label(
            default = "@platforms//os:ios",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_wasm": attr.label(
            default = "//bzl/crosstool:wasm",
            providers = [platform_common.ConstraintValueInfo],
        ),
    },
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
    fragments = ["cpp"],
)

# Embed binary files into compiled code. This rule will generate 3 readonly
# symbols in the library and a header file.
#
# #include "package/path/name.h"
#
# # Pointer to the data.
# extern const uint8_t *const embeddedCamelCaseNameData;
#
# # Size of the embedded data in bytes. Does not include null-terminating character.
# extern const size_t embeddedCamelCaseNameSize;
#
# # Pointer to the data, null-terminated for convienent use in c-string functions.
# extern const char *const embeddedCamelCaseNameCStr;
#
# # A c++ string_view that is initialized to the data and size.
# extern const std::string_view embeddedCamelCaseNameView;
def embedded_data(name, srcs = [], use_asm = True):
    _embedded_data(
        name = name,
        srcs = srcs,
        use_asm = use_asm,
        visibility = ["//visibility:public"],
    )
