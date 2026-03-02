_TEMPLATE = Label("//bzl/crosstool:emscripten.config.tpl")

def _emscripten_config_impl(repository_ctx):
    repository_ctx.file("BUILD", content = "exports_files([\"emscripten.config\"])", executable = False)
    repository_ctx.template(
        "emscripten.config",
        repository_ctx.path(_TEMPLATE),
        substitutions = {
            "{python_toolchain}": str(repository_ctx.path(repository_ctx.attr.python)),
            "{node_toolchain}": str(repository_ctx.path(repository_ctx.attr.node)),
        },
        executable = False,
    )

emscripten_config = repository_rule(
    implementation = _emscripten_config_impl,
    attrs = {
        "node": attr.label(mandatory = True),
        "python": attr.label(mandatory = True),
        "_template": attr.label(
            default = _TEMPLATE,
            allow_single_file = True,
        ),
    },
)

def _get_wasm_binaries_url_and_sha256(repository_ctx):
    os_name = repository_ctx.os.name.lower()
    os_type = "linux"
    suffix = "tbz2"

    if os_name.find("mac os") != -1:
        os_type = "mac"
    elif os_name.find("windows") != -1:
        os_type = "win"
        suffix = "zip"

    qualifier = ""
    if repository_ctx.os.arch == "aarch64" or repository_ctx.os.arch == "arm64":
        qualifier = "-arm64"
        os_arch = os_type + qualifier
    else:
        os_arch = os_type + "-x86_64"

    sha256 = repository_ctx.attr.wasm_release_sha256[os_arch]

    # This needs to mirror the logic in emsdk.py for generating this URL.
    return ("https://storage.googleapis.com/webassembly/emscripten-releases-builds/{os}/{release}/wasm-binaries{qualifier}.{suffix}".format(
        os = os_type,
        qualifier = qualifier,
        release = repository_ctx.attr.wasm_release_commit,
        suffix = suffix,
    ), sha256)

def _emscripten_toolchain_impl(repository_ctx):
    repository_ctx.file("BUILD", content = """filegroup(
    name = "stub",
    srcs = ["{}.sha1"],
    visibility = [ "//visibility:public" ],
)
""".format(repository_ctx.name), executable = False)

    repository_ctx.download_and_extract(
        url = repository_ctx.attr.url,
        sha256 = repository_ctx.attr.sha256,
        stripPrefix = repository_ctx.attr.strip_prefix,
    )

    # The following would be sufficient, but from time-to-time, bazel would
    # re-run this rule and redownload the 1.0GB toolchain. Instead, of the below, we
    # download and extract using bazel methods that cache artifacts
    # indefinitely.
    # for tool in repository_ctx.attr.tools:
    #     result = repository_ctx.execute([
    #         "./emsdk", "install", tool,
    #     ])
    #     if (result.return_code != 0):
    #         fail(result.stderr)

    binaries_url, binaries_sha256 = _get_wasm_binaries_url_and_sha256(repository_ctx)

    repository_ctx.download_and_extract(
        url = binaries_url,
        sha256 = binaries_sha256,
        type = "tar.bz2" if binaries_url.endswith("tbz2") else "",
    )

    repository_ctx.symlink(repository_ctx.attr.config, "emscripten.config")
    repository_ctx.symlink("../../" + repository_ctx.attr.cache_stub.workspace_root + "/cache", "cache")
    repository_ctx.symlink("install", "upstream")

    for patch in repository_ctx.attr.patches:
        repository_ctx.patch(patch)

    # Create a sha1 hash of the files in the toolchain and make that an input
    # to the :stub rule, as we are intentially hiding all of the files in the
    # toolchain from bazel to prevent slow-downs from importing the toolchain
    # into the sandbox for every action. This hash ensures that any files
    # changed in the toolchain will cause a cache-miss and recompilation.
    result = repository_ctx.execute([
        "bash",
        "-c",
        "find -L . -type f -print0 | sort -z | xargs -0 sha1sum | sha1sum | cut -d \" \" -f 1",
    ])

    if result.return_code != 0:
        fail("Failed to compute sha1 hash")

    repository_ctx.file("{}.sha1".format(repository_ctx.name), content = result.stdout, executable = False)

emscripten_toolchain = repository_rule(
    implementation = _emscripten_toolchain_impl,
    attrs = {
        "version": attr.string(mandatory = True),
        "wasm_release_commit": attr.string(mandatory = True),
        "wasm_release_sha256": attr.string_dict(mandatory = True),
        "url": attr.string(mandatory = True),
        "sha256": attr.string(default = ""),
        "strip_prefix": attr.string(default = ""),
        "config": attr.label(mandatory = True),
        "cache_stub": attr.label(mandatory = True),
        "patches": attr.label_list(default = []),
    },
)
