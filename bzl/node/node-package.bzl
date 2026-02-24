"""
This module defines rules for packaging a Node.js application into bundles
"""

load("@rules_pkg//:pkg.bzl", "pkg_tar")
load("//bzl/runfiles:materialize-node-runfiles.bzl", "materialize_node_runfiles")

def node_package(name, main = [], data = []):
    """
    Packages a Node.js application into an tar archive using the provided main entrypoint (js target) and any other data dependencies.

    Args:
      name: The name of the resulting tar archive.
      main: A list containing exactly one main entrypoint file (e.g., index.js).
      data: A list of additional files or dependencies to include in the package.
    """

    if (len(main) != 1):
        fail(msg = "node_package requires exactly one main or index.js source.")

    node_assets_name = name + "-node-assets"
    materialize_node_runfiles(
        name = node_assets_name,
        deps = data,
        wasm_deps = [],
    )

    pkg_tar(
        name = name,
        srcs = main + data + [node_assets_name],
        strip_prefix = native.package_name(),
        extension = "tgz",
        package_dir = ".",
    )
