load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("@rules_pkg//pkg:mappings.bzl", "pkg_files")

package(default_visibility = ["//apps/desktop:__subpackages__"])

pkg_files(
    name = "config-types",
    srcs = [
        "config/types/assets.d.ts",
        "config/types/index.d.ts",
    ],
    prefix = "config/types",
)

pkg_files(
    name = "config",
    srcs = [
        "config/asset-loader.js",
        "config/entry-plugin.js",
        "config/webpack.config.js",
    ],
    prefix = "config",
)

pkg_files(
    name = "src",
    srcs = [
        "src/.expanse.json",
        "src/example-component.ts",
        "src/index.html",
    ],
    prefix = "src",
)

pkg_zip(
    name = "new-project",
    srcs = [
        "package.json",
        "tsconfig.json",
        ":config",
        ":config-types",
        ":src",
    ],
)
