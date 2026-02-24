WORKSPACE_NAME = "the8thwall"

workspace(name = WORKSPACE_NAME)

load("//bzl/node:npm.bzl", "npm_package")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

# Load rules_nodejs to provide nodejs toolchains.
http_archive(
    name = "build_bazel_rules_nodejs",
    sha256 = "709cc0dcb51cf9028dd57c268066e5bc8f03a119ded410a13b5c3925d6e43c48",
    urls = [
        "https://github.com/bazelbuild/rules_nodejs/releases/download/5.8.4/rules_nodejs-5.8.4.tar.gz",
    ],
)

# Rules for downloading Node.js toolchains.
load(
    "@build_bazel_rules_nodejs//:repositories.bzl",
    "build_bazel_rules_nodejs_dependencies",
)

build_bazel_rules_nodejs_dependencies()

load("@build_bazel_rules_nodejs//:index.bzl", "node_repositories")

# Node toolchain to install.
node_repositories(
    # https://github.com/bazel-contrib/rules_nodejs/blob/5.8.4/nodejs/private/node_versions.bzl
    node_version = "18.17.0",
    yarn_version = "1.22.18",
)

npm_package(
    name = "npm-eslint",
    exports_files = [
        "node_modules/eslint/bin/eslint.js",
    ],
    package = "//bzl/npmpackage/eslint:package.json",
    package_lock = "//bzl/npmpackage/eslint:package-lock.json",
    patches = [
        "//bzl/npmpackage/eslint/patches:eslint-plugin-local-rules+0.1.1.patch",
    ],
)
