"""
This module defines Bazel rules for generating export options plists for Xcode projects.
"""

ExportOptionsPlistInfo = provider(
    "Xcode + iOS info needed to archive an xcode project + export an xcode archive.",
    fields = {
        "plist": "generated plist file",
        "profilename": "provisioning profile name mentioned in generated plist",
        "distributionmethod": "distribution method mentioned in generated plist",
    },
)

def get_bundle_id(app_name):
    return "com.nianticlabs.%s" % (app_name)

def _export_options_plist_impl(ctx):
    export_options_plist_out = ctx.actions.declare_file("%s/export_options.plist" % (ctx.label.name))
    ctx.actions.expand_template(
        template = ctx.file._exportOptionsTemplatePList,
        output = export_options_plist_out,
        substitutions = {
            "%TEAM_ID%": "<REMOVED_BEFORE_OPEN_SOURCING>",
            "%METHOD%": ctx.attr.distribution_method,
            "%SIGNING_STYLE%": "manual",
            "%PROVISIONING_PROFILE_KEY%": get_bundle_id(ctx.attr.app_name),
            "%PROVISIONING_PROFILE_STRING%": ctx.attr.provisioning_profile_name,
        },
    )

    return [
        DefaultInfo(files = depset([export_options_plist_out])),
        ExportOptionsPlistInfo(
            plist = export_options_plist_out,
            profilename = ctx.attr.provisioning_profile_name,
            distributionmethod = ctx.attr.distribution_method,
        ),
    ]

export_options_plist = rule(
    implementation = _export_options_plist_impl,
    attrs = {
        "_exportOptionsTemplatePList": attr.label(
            default = Label("//bzl/xcode:exportOptionsTemplate.plist"),
            allow_single_file = True,
        ),
        "distribution_method": attr.string(
            mandatory = True,
        ),
        "app_name": attr.string(
            mandatory = True,
        ),
        "provisioning_profile_name": attr.string(
            mandatory = True,
        ),
    },
)
