def _http_file_server_impl(ctx):
    outs = [ctx.actions.declare_file(ctx.label.name)]

    serve_path = "{}/{}_webserver_files".format(ctx.label.package, ctx.label.name)

    # Create a runfiles directory and keep the content to serve via the web server in a separate
    # root directory.
    runfiles = ctx.runfiles(
        files = [ctx.file._http_file_server_attr],
        symlinks = {"{}/{}".format(serve_path, src.short_path): src for src in ctx.files.data},
    )
    runfiles = runfiles.merge(ctx.attr._http_file_server_attr[DefaultInfo].default_runfiles)

    ctx.actions.write(
        output = outs[-1],
        content = """#!/bin/bash --norc
set -eu
DIR=`dirname ${BASH_SOURCE[0]}`
export RUNFILES_DIR=${RUNFILES_DIR:-$0.runfiles}
${DIR}/%s.runfiles/_main/bzl/httpfileserver/impl/http-file-server \
  --https=%s \
  --port=%s \
  --root_redirect="%s" \
  ${DIR}/%s.runfiles/%s/%s
""" % (ctx.label.name, "true" if ctx.attr.https else "false", ctx.attr.port, ctx.attr.root_redirect, ctx.label.name, ctx.workspace_name, serve_path),
    )
    return [DefaultInfo(
        files = depset(outs),
        executable = outs[0],
        runfiles = runfiles,
    )]

_http_file_server = rule(
    attrs = {
        "data": attr.label_list(
            allow_files = True,
            default = [],
        ),
        "_http_file_server_attr": attr.label(
            default = Label("//bzl/httpfileserver/impl:http-file-server"),
            allow_single_file = True,
            executable = True,
            cfg = "target",
        ),
        "https": attr.bool(default = True, mandatory = False),
        "port": attr.int(default = 8888, mandatory = False),
        "root_redirect": attr.string(default = "", mandatory = False),
    },
    executable = True,
    implementation = _http_file_server_impl,
)

# Build rule for serving bazel-built targets by a local server. This starts a server on the local
# machine and serves all of the files generated in data.
def http_file_server(
        name,
        data = [],
        https = True,
        port = 8888,
        root_redirect = "",
        visibility = None):
    _http_file_server(
        name = name,
        data = data,
        https = https,
        port = port,
        root_redirect = root_redirect,
        visibility = visibility,
    )
