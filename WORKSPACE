WORKSPACE_NAME = "the8thwall"

workspace(name = WORKSPACE_NAME)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:jvm.bzl", "jvm_maven_import_external")
load("//bzl/android:android-sdk.bzl", "android_sdk")
load("//bzl/crosstool:emscripten.bzl", "emscripten_config", "emscripten_toolchain")
load("//bzl/crosstool:local-tool.bzl", "local_tool")
load("//bzl/crosstool:toolchains.bzl", "http_toolchain")
load("//bzl/gpu:cuda-triplet.bzl", "cuda_triplet")
load("//bzl/node:npm.bzl", "npm_package")
load("//bzl/utils:maybe.bzl", "niantic_maybe")

http_archive(
    name = "rules_license",
    sha256 = "4531deccb913639c30e5c7512a054d5d875698daeb75d8cf90f284375fe7c360",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_license/releases/download/0.0.7/rules_license-0.0.7.tar.gz",
        "https://github.com/bazelbuild/rules_license/releases/download/0.0.7/rules_license-0.0.7.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# Explicitly register go toolchain to the version used in tensorflow
http_archive(
    name = "io_bazel_rules_go",
    sha256 = "bfc5ce70b9d1634ae54f4e7b495657a18a04e0d596785f672d35d5f505ab491a",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_go/releases/download/v0.40.0/rules_go-v0.40.0.zip",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.40.0/rules_go-v0.40.0.zip",
    ],
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains")

go_register_toolchains(version = "1.23.7")

http_archive(
    name = "rules_foreign_cc",
    patch_args = ["-p1"],
    patches = [
        "//third_party/rulesforeign:rules_foreign_cc-0.9.0.patch",
    ],
    sha256 = "2a4d07cd64b0719b39a7c12218a3e507672b82a97b98c6a89d38565894cf7c51",
    strip_prefix = "rules_foreign_cc-0.9.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/0.9.0.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.9.0/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

http_archive(
    name = "rules_python",
    sha256 = "c68bdc4fbec25de5b5493b8819cfc877c4ea299c0dcb15c244c5a00208cde311",
    strip_prefix = "rules_python-0.31.0",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.31.0/rules_python-0.31.0.tar.gz",
)

load("@rules_python//python:repositories.bzl", "py_repositories", "python_register_toolchains")

py_repositories()

# Register a Python3 toolchain.
python_register_toolchains(
    name = "python-3.9",
    # Available versions are listed in @rules_python//python:versions.bzl.
    python_version = "3.9",
)

load(
    "@python-3.9//:defs.bzl",
    EMCC_PYTHON = "interpreter",
    python_interpreter = "interpreter",
)
load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
    name = "pip-deps",
    envsubst = ["PIP_INDEX_URL"],
    extra_pip_args = [
        "--index-url",
        "${PIP_INDEX_URL:-https://pypi.org/simple}",
    ],
    python_interpreter_target = python_interpreter,
    requirements_lock = "//bzl/python:requirements.txt",
)

# Define repos for python pip dependencies.
load("@pip-deps//:requirements.bzl", python_install_deps = "install_deps")

python_install_deps()

pip_parse(
    name = "v8_python_deps",
    envsubst = ["PIP_INDEX_URL"],
    extra_pip_args = [
        "--require-hashes",
        "--index-url",
        "${PIP_INDEX_URL:-https://pypi.org/simple}",
    ],
    requirements_lock = "//bzl/node:node_v8_deps.txt",
)

# Define repos for python pip dependencies.
load("@v8_python_deps//:requirements.bzl", v8_python_deps_install_deps = "install_deps")

v8_python_deps_install_deps()

################################################# END 1

# Load and cache select environment variables at bazel startup for tool locations.
load("//bzl/crosstool:env-vars.bzl", "env_vars")

env_vars(
    name = "local-env",
    env = [
        "PKG_CONFIG",
        "BAZEL_SH",
    ],
)

# These are LLVM toolchain built from source for macosx, capable of targeting
# ARM, WebAssembly and x86.
# Instructions on building an LLVM toolchain can be found here:
# https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/wiki/spaces/AR/pages/1945436891
http_toolchain(
    name = "llvm-macosx-arm64",
    build_file = "//third_party/llvm:llvm.BUILD",
    sha256 = "b4a76987199c768c62d007f6a24b2a5ec7c9a454fc9a583ec0b102aa16d18e5a",
    strip_prefix = "llvm-16.0.6-7cbf1a2-macosx-arm64",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/llvm/llvm-16.0.6-7cbf1a2-macosx-arm64.tar.gz",
)

http_toolchain(
    name = "llvm-macosx-x86_64",
    build_file = "//third_party/llvm:llvm.BUILD",
    sha256 = "e11e55fa28810da05d43368b060dd991863e735de9198ea11de37ce9ba11c2fd",
    strip_prefix = "llvm-16.0.6-7cbf1a2-macosx-x86_64",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/llvm/llvm-16.0.6-7cbf1a2-macosx-x86_64.tar.gz",
)

http_toolchain(
    name = "llvm-linux",
    build_file = "//third_party/llvm:llvm.BUILD",
    sha256 = "3b8ad3832ad992e104b18901730a93ef8a5e7a2b680c0322e4cae829a7613a67",
    strip_prefix = "llvm-16.0.6-7cbf1a2-Linux-x86_64",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/llvm/llvm-16.0.6-7cbf1a2-Linux-x86_64.tar",
)

http_toolchain(
    name = "llvm-linux-arm64",
    build_file = "//third_party/llvm:llvm.BUILD",
    sha256 = "591ff74b1e27cf7ef2fe4891b8be6831d4a51b5cfefa9054d1e6887ad31d44db",
    strip_prefix = "llvm-16.0.6-7cbf1a2-Linux-arm64",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/llvm/llvm-16.0.6-7cbf1a2-Linux-arm64.tar",
)

http_toolchain(
    name = "llvm-windows",
    build_file = "//third_party/llvm:llvm.BUILD",
    sha256 = "9c7a6b87e284d678f64e2343d31ca20439c4eef27141c41f19b6a8c7a1c58744",
    strip_prefix = "llvm-7cbf1a2-windows-x86_64",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/llvm/llvm-16.0.6-7cbf1a2-windows-x86_64.tar",
)

# Register LLVM toolchains (used as host)
register_toolchains(
    "//bzl/llvm:llvm-macosx-arm64",
    "//bzl/llvm:llvm-macosx-x86_64",
    "//bzl/llvm:llvm-linux",
    "//bzl/llvm:llvm-linux-arm64",
    "//bzl/llvm:llvm-windows",
)

http_toolchain(
    name = "gradle",
    build_file = "//third_party/gradle:gradle.BUILD",
    sha256 = "f6b8596b10cce501591e92f229816aa4046424f3b24d771751b06779d58c8ec4",
    strip_prefix = "gradle-7.5.1",
    url = "https://services.gradle.org/distributions/gradle-7.5.1-bin.zip",
)

http_toolchain(
    name = "gradle-8",
    build_file = "//third_party/gradle:gradle.BUILD",
    sha256 = "f2b9ed0faf8472cbe469255ae6c86eddb77076c75191741b4a462f33128dd419",
    strip_prefix = "gradle-8.4",
    url = "https://services.gradle.org/distributions/gradle-8.4-all.zip",
)

# This is an XCode toolchain, pared down to retain the platform SDKs and
# frameworks needed for cross compilation.
# Instructions on building an XCode toolchain can be found here:
# <REMOVED_BEFORE_OPEN_SOURCING>

# Xcode 13.0.0 toolchain w/o llvm
http_toolchain(
    name = "xcode13",
    build_file = "//third_party/xcode:xcode.BUILD",
    sha256 = "4519a08380c7863a537f40d30e1212f3fbbcb01c2a0e907e05a44b625f9bfc05",
    strip_prefix = "Xcode_13.0.0",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/xcode/Xcode_13.0.0-no_GateKeeper.tar",
)

# Xcode 14.2 toolchain with llvm
http_toolchain(
    name = "xcode14",
    build_file = "//third_party/xcode:xcode.BUILD",
    sha256 = "92c4da6f3062b88124468e6f6a575008fccee469ec92c30a09630d34645812c6",
    strip_prefix = "Xcode_14.2_Ventura",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/xcode/Xcode_14.2_Ventura-no_GateKeeper.tar",
)

# Xcode 15.4 toolchain
http_toolchain(
    name = "xcode15",
    build_file = "//third_party/xcode:xcode.BUILD",
    sha256 = "2c0970f41ede0e54adcff06ae6d344ce250b3daf4da9bd0c4d53c77427970f05",
    strip_prefix = "Xcode_15.4",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/xcode/Xcode_15.4_2_stripped_libs.tar.gz",
)

# Xcode 16.4 toolchain
http_toolchain(
    name = "xcode16",
    build_file = "//third_party/xcode:xcode.BUILD",
    sha256 = "5ab03f37e0135db1c83d49a19369ca9ea2ea785e0d33ab35d7454525e1569199",
    strip_prefix = "Xcode_16.4",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/xcode/Xcode_16.4_2_stripped_libs.tar.gz",
)

# This is an MSVC toolchain with Windows platform SDKs and headers needed for
# cross-compilation.
# Instructions on building an XCode toolchain can be found here:
# https://stackoverflow.com/questions/23248989/clang-c-cross-compiler-generating-windows-executable-from-mac-os-x
http_toolchain(
    name = "msvc",
    build_file = "//third_party/msvc:msvc.BUILD",
    sha256 = "a5ef33433ee181d0375f701863114511fad550fbd1daf0c51b23e538a5cf1342",
    strip_prefix = "msvc-2021-platform-sdk",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/msvc/msvc-2021-platform-sdk.tar",
)

## Rules for compling Kotlin code.
http_archive(
    name = "rules_kotlin",
    sha256 = "3b772976fec7bdcda1d84b9d39b176589424c047eb2175bed09aac630e50af43",
    url = "https://github.com/bazelbuild/rules_kotlin/releases/download/v1.9.6/rules_kotlin-v1.9.6.tar.gz",
)

load("@rules_kotlin//kotlin:repositories.bzl", "kotlin_repositories", "kotlinc_version")

kotlin_repositories(
    compiler_release = kotlinc_version(
        release = "1.9.25",
        sha256 = "6ab72d6144e71cbbc380b770c2ad380972548c63ab6ed4c79f11c88f2967332e",
    ),
)

load("@rules_kotlin//kotlin:core.bzl", "kt_register_toolchains")

kt_register_toolchains()

load("//bzl/android:android-sdk-packages.bzl", "ANDROID_SDK_PACKAGES")

# Repository rule which will download and install SDK packages locally using
# the Android sdkmanager, necessary for Android development.
android_sdk(
    name = "android-sdk",
    packages = ANDROID_SDK_PACKAGES,
)

# Some optional local tools to be used as runtime runfiles.
local_tool(
    name = "local-wine64",
    tool = "wine64",
)

load("//bzl/crosstool:workspace-env.bzl", "workspace_env")

workspace_env(
    name = "workspace-env",
    workspace_name = WORKSPACE_NAME,
)

load(
    "//bzl/android:android-version.bzl",
    "ANDROID_BUILD_TOOLS_VERSION",
    "DEFAULT_ANDROID_API_LEVEL",
)

android_sdk_repository(
    name = "androidsdk",
    api_level = DEFAULT_ANDROID_API_LEVEL,
    build_tools_version = ANDROID_BUILD_TOOLS_VERSION,
)

bind(
    name = "android-platform-jar",
    actual = "@androidsdk//:platforms/android-%d/android.jar" % 30,
)

bind(
    name = "android/crosstool",
    actual = "//bzl/crosstool:android-cc-toolchain",
)

jvm_maven_import_external(
    name = "junit4",
    artifact = "junit:junit:4.12",
    artifact_sha256 = "59721f0805e223d84b90677887d9ff567dc534d7c502ca903c0c2b17f05c116a",
    server_urls = ["https://repo1.maven.org/maven2/"],
)

jvm_maven_import_external(
    name = "mockito",
    artifact = "org.mockito:mockito-all:1.10.19",
    artifact_sha256 = "d1a7a7ef14b3db5c0fc3e0a63a81b374b510afe85add9f7984b97911f4c70605",
    server_urls = ["https://repo1.maven.org/maven2/"],
)

# Register mono toolchains.
register_toolchains(
    "//bzl/mono:mono-macosx",
)

# Load rules_nodejs to provide nodejs toolchains.
http_archive(
    name = "rules_nodejs",
    sha256 = "b6016a89a12a3d339ece93f2b3988f5e812f452ad497bc963634646ff4aa100b",
    strip_prefix = "rules_nodejs-6.1.2",
    urls = [
        "https://github.com/bazelbuild/rules_nodejs/releases/download/v6.1.2/rules_nodejs-v6.1.2.tar.gz",
    ],
)

# Rules for downloading Node.js toolchains.
load("@rules_nodejs//nodejs:repositories.bzl", "nodejs_register_toolchains", "rules_nodejs_dependencies")

rules_nodejs_dependencies()

nodejs_register_toolchains(
    # https://github.com/bazel-contrib/rules_nodejs/blob/v6.1.2/nodejs/private/node_versions.bzl
    name = "nodejs",
    node_version = "20.14.0",
)

load("//bzl/crosstool:node-toolchain.bzl", "node_toolchain")

node_toolchain(name = "node-toolchain")

EMCC_NODE = "@nodejs_host//:bin/node"

emscripten_config(
    name = "emscripten-config",
    node = EMCC_NODE,
    python = EMCC_PYTHON,
)

# Emscripten pre-compiled libraries and platform sdks.
http_toolchain(
    name = "emscripten-cache",
    build_file = "//third_party/emscripten:emscripten-cache.BUILD",
    sha256 = "ff3017b81b3c32c175e3ad1697ba9650b5085fd49f7d621a4b5b7381152687d7",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/emscripten/emscripten-3.1.24-cache.tar",
)

# Emscripten toolchain.
emscripten_toolchain(
    name = "emscripten",
    cache_stub = "@emscripten-cache//:stub",
    config = "@emscripten-config//:emscripten.config",
    patches = [
        "//third_party/emscripten:emrun.py.patch",
    ],
    sha256 = "1aa5365ccb2147701cc9d1e59a5a49577c1d6aea55da7c450df2d5ffa48b8a58",
    strip_prefix = "emsdk-3.1.24",
    url = "https://github.com/emscripten-core/emsdk/archive/refs/tags/3.1.24.tar.gz",
    version = "3.1.24",
    wasm_release_commit = "54217a0950bb1dafe8808cc6207d378e323f9d74",
    wasm_release_sha256 = {
        "mac-arm64": "e87b0727343051312f82a6653cad4682a518dd9cb6575844c0cd6505d520fab6",
        "mac-x86_64": "cfb897a980dd51fceb02ff143ad0fd8e5d299db640c5646d1547d522194545f2",
        "linux-x86_64": "20e8e5bd745e3ad69c03bb877091d2fbb0c7db1eab309de8f185e9821aea40f4",
        "win-x86_64": "a0ea07f9014a912f13176fdbbc1ee7ab08104d45e7ca7e1c237505579b63d530",
    },
)

# Node modules for Webpack.
npm_package(
    name = "npm-webpack-build",
    package = "//bzl/npmpackage/webpack-build:package.json",
    package_lock = "//bzl/npmpackage/webpack-build:package-lock.json",
)

# Node modules for Mocha, JavaScript test framework.
npm_package(
    name = "npm-mocha",
    exports_files = [
        "node_modules/mocha/bin/mocha",
    ],
    package = "//bzl/npmpackage/mocha:package.json",
    package_lock = "//bzl/npmpackage/mocha:package-lock.json",
)

# Node modules for eslint.
npm_package(
    name = "npm-eslint",
    env = {
        "NPM_CONFIG_LEGACY_PEER_DEPS": "1",
    },
    exports_files = [
        "node_modules/eslint/bin/eslint.js",
    ],
    package = "//bzl/npmpackage/eslint:package.json",
    package_lock = "//bzl/npmpackage/eslint:package-lock.json",
)

# Node modules for capnp-ts.
npm_package(
    name = "npm-capnp-ts",
    package = "//bzl/npmpackage/capnp-ts:package.json",
    package_lock = "//bzl/npmpackage/capnp-ts:package-lock.json",
)

# Node modules for capnpc-ts.
npm_package(
    name = "npm-capnpc-ts",
    package = "//bzl/npmpackage/capnpc-ts:package.json",
    package_lock = "//bzl/npmpackage/capnpc-ts:package-lock.json",
)

# Node modules for capnpc-js.
npm_package(
    name = "npm-capnpc-js",
    package = "//bzl/npmpackage/capnpc-js:package.json",
    package_lock = "//bzl/npmpackage/capnpc-js:package-lock.json",
)

# Node modules for 8th Wall tune-parameters.
npm_package(
    name = "npm-tune-parameters",
    package = "//bzl/npmpackage/tune-parameters:package.json",
    package_lock = "//bzl/npmpackage/tune-parameters:package-lock.json",
)

# Node modules for 8th Wall's js engine.
npm_package(
    name = "npm-jsxr",
    package = "//reality/app/xr/js:package.json",
    package_lock = "//reality/app/xr/js:package-lock.json",
)

# Node modules for rendering.
npm_package(
    name = "npm-rendering",
    package = "//bzl/npmpackage/rendering:package.json",
    package_lock = "//bzl/npmpackage/rendering:package-lock.json",
    patches = [
        "//bzl/npmpackage/rendering/patches:html-element+2.3.1.patch",
        "//bzl/npmpackage/rendering/patches:image-js+0.35.4.patch",
    ],
)

# Node modules for lambdas in reality/cloud/aws/lambda and reality/cloud/aws/edge-lambda
npm_package(
    name = "npm-lambda",
    package = "//bzl/npmpackage/lambda:package.json",
    package_lock = "//bzl/npmpackage/lambda:package-lock.json",
)

# Node modules for c8/ecs
npm_package(
    name = "npm-ecs",
    package = "//c8/ecs:package.json",
    package_lock = "//c8/ecs:package-lock.json",
    patches = [
        "//c8/ecs/patches:@types+css-font-loading-module+0.0.14.patch",
        "//c8/ecs/patches:@types+node+16.18.36.patch",
    ],
)

# Node modules for packaging HTML apps into native apps.
npm_package(
    name = "npm-html-app-packager",
    package = "//bzl/npmpackage/html-app-packager:package.json",
    package_lock = "//bzl/npmpackage/html-app-packager:package-lock.json",
)

# Node modules building the Tauri shell.
npm_package(
    name = "npm-tauri-shell",
    exports_files = [
        "node_modules/.bin/tauri",
    ],
    package = "//bzl/npmpackage/tauri-shell:package.json",
    package_lock = "//bzl/npmpackage/tauri-shell:package-lock.json",
)

load("//bzl/crosstool:execution-platform-configure.bzl", "execution_platform_configure")

# Override the built-in 'local_config_platform' rule to ensure a
# Niantic-specific default execution platform.
execution_platform_configure(name = "local_config_platform_custom")

register_execution_platforms(
    "@local_config_platform_custom//:host",
)

# Register x86_64 as a secondary execution platform for Apple silicon Macs with rosetta.
load("//bzl/apple:rosetta-platform-configure.bzl", "configure_rosetta")

configure_rosetta(name = "rosetta")

load("@rosetta//:repo.bzl", "register_rosetta_execution_platform")

register_rosetta_execution_platform()

# These are toolchains for compiling code. They are defined in
# //bzl/crosstool/BUILD, where they describe the supported execution and target
# platforms.
register_toolchains(
    # Toolchains for compiling for OSX from OSX.
    "//bzl/crosstool:cc-toolchain-darwin_x86_64",
    "//bzl/crosstool:cc-toolchain-darwin_arm64",
    "//bzl/crosstool:cc-toolchain-darwin_universal",
    # Toolchain for compiling for iOS from OSX.
    "//bzl/crosstool:cc-toolchain-ios_arm64",
    # Toolchain for compiling for iOS simulator from OSX
    "//bzl/crosstool:cc-toolchain-iossimulator_arm64",
    # Toolchain for compiling for Windows from OSX.
    "//bzl/crosstool:cc-toolchain-x64_windows-exec-osx",
    # Toolchain for compiling for Windows from Linux.
    "//bzl/crosstool:cc-toolchain-x64_windows-exec-linux",
    # Toolchain for compiling for Windows from Windows.
    "//bzl/crosstool:cc-toolchain-x64_windows-exec-windows",
    # Toolchain for compiling for Android from OSX.
    "//bzl/crosstool:cc-toolchain-android_armv7a-exec-osx",
    "//bzl/crosstool:cc-toolchain-android_arm64-exec-osx",
    "//bzl/crosstool:cc-toolchain-android_x86_32-exec-osx",
    "//bzl/crosstool:cc-toolchain-android_x86_64-exec-osx",
    # Toolchain for compiling for Android from Linux (x86_64 host supported only).
    "//bzl/crosstool:cc-toolchain-android_armv7a-exec-linux",
    "//bzl/crosstool:cc-toolchain-android_arm64-exec-linux",
    "//bzl/crosstool:cc-toolchain-android_x86_32-exec-linux",
    "//bzl/crosstool:cc-toolchain-android_x86_64-exec-linux",
    # Toolchain for compiling for WebAssembly.
    "//bzl/crosstool:cc-toolchain-wasm32",
    # Toolchain for compiling for Amazon Linux from OSX, Linux x86 and Linux arm64.
    "//bzl/crosstool:cc-toolchain-amazonlinux",
    # Toolchain for compiling for Niantic Linuxes from OSX, Linux x86 and Linux arm64.
    "//bzl/crosstool:cc-toolchain-v1-linux",  # https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/l/cp/co6yA1pW
    "//bzl/crosstool:cc-toolchain-v2-linux",  # https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/l/cp/X25vkYhE
    # Required by Bazel 7 to allow transitive external dependencies (from org_tensorflow as usual)
    # to use local python version. See https://bit.ly/49ySxPJ for more information.
    "@bazel_tools//tools/python:autodetecting_toolchain",
)

load("//bzl/xcode:apple-developer-team.bzl", "apple_developer_team")

# Define Bazel config_settings for installed Apple developer team profiles.
# If a new Apple developer provisioning file is installed that matches any
# of the below teams, run 'bazel clean' to update future executions.
#
# The outputs of this rule will be written to bazel-code8/external/apple-developer-team/BUILD
#
# To configure this for the first time, do the following:
#   1. Download XCode.
#      - Install the iOS SDK as you do so.
#   2. Login to your Apple Developer Account.
#      - Open XCode and go to Settings -> Accounts -> + -> Apple Account, then login with your
#        @8thwall.com account.
#   3. Force Apple to create a certificate for you.
#      - First create the app: In Xcode, File -> New -> Project... -> App -> Select
#        Team: "8th Wall, Inc." -> write any Organization Identifier -> finish creating the app.
#        - You may have to click Team -> Add Account -> sign in with your @8thwall.com Apple ID.
#        - Example app name: "ParisTest", example Organization Identifier "com.the8thwall".
#      - Next, set up signing: In the app, go to `Signing & Capabilities` -> `Signing` -> select
#        `Automatically Manage Signing`, and choose "8th Wall, Inc." as `Team`.
#      - Doing this should force XCode to create a personal certificate for you. Check for it under
#        Keychain Access - it will be named something like "Apple Development: Paris Morgan (<id>)".
#   4. Find the iOS device you want to test with and get its UDID.
#      - Plug it into your mac, open the Window -> Devices and Simulators window in XCode, right
#        click on the device in the left sidebar, and select "Copy Identifier". This is the UDID.
#   5. Reach out to Paris or Tony and share both your UDID and a device name (i.e. "Paris 16 Pro" or
#      "8w 13 Mini") with them. They will need to:
#        a. On https://developer.apple.com/account/resources/devices/list, add your device.
#        b. On https://developer.apple.com/account/resources/profiles/edit/K3MFQ5N2MY, check both
#           your newly added device and certificate, then click `Save`.
#   6. Open XCode and go to Settings -> Accounts -> Select "8th Wall, Inc." -> click "Download
#      Manual Profiles".
#   7. Try building, e.g. "bazel clean && ./apps/client/nae/catch-the-stack/ios/build-install.sh".
#   8. If it doesn't work, you can reach out to Paris to debug together.
#
# Troubleshooting:
# - Any time you change your provisioning profile or certificate, you will need to bazel clean
#   before your next build. This is b/c the outputs of `apple_developer_team()` are cached.
# - If you do not see "8th Wall, Inc.", then you may need to be added to the Apple developer
#   team. Contact Tony or Paris to add you (they should add you as a "Developer" with
#   "Access to Certificates, Identifiers & Profiles." and the items under it checked).
# - If you get "This provisioning profile cannot be installed on this device.", then the Wildcard
#   Development profile may not have been updated with your device. Contact Tony or Paris to make
#   sure your device is added to the Wildcard Development profile. If it is, then you should delete
#   the downloaded provisioning profiles and download it again (step #6). To delete the provisioning
#   profiles, delete the files under:
#     - XCode <=15: ~/Library/MobileDevice/Provisioning\ Profiles
#     - XCode >=16: ~/Library/Developer/Xcode/UserData/Provisioning\ Profiles
#   Then try a bazel clean and build again.
#
# If you do not run these steps, you will get errors about "@@platforms//:incompatible".
apple_developer_team(
    name = "apple-developer-team",
    provisioning_profile_names = {
        "Wildcard Development": "wildcard-development",
    },
    team_identifiers = {
        "<REMOVED_BEFORE_OPEN_SOURCING>": "niantic",
    },
)

# Install a newer version of googleapis than is provided in org_tensorflow.
niantic_maybe(
    git_repository,
    name = "com_google_googleapis",
    commit = "eabec5a21219401bad79e1cc7d900c1658aee5fd",
    patch_args = ["-p1"],
    patches = [
        "//third_party/googleapis:angle_bracket_includes.patch",  # Required for @com_github_googleapis_google_cloud_cpp
    ],
    remote = "https://github.com/googleapis/googleapis.git",
    shallow_since = "1614118133 -0800",
)

load("@com_google_googleapis//:repository_rules.bzl", "switched_rules_by_language")

switched_rules_by_language(
    name = "com_google_googleapis_imports",
    cc = True,  # C++ support is only "Partially implemented", roll our own.
    grpc = True,
)

# This is duplicated here only for org_tensorflow->com_github_grpc_grp internal dependency that is still using
# native.bind which are deprecated and not working for MODULE.bazel deps (See https://bazel.build/external/migration#bind-targets)
new_git_repository(
    name = "zlib",
    build_file = "//bzl/thirdpartybuild:zlib.BUILD",
    # This commit fixes the ZLIB_VERNUM != PNG_ZLIB_VERNUM error
    commit = "04f42ceca40f73e2978b50e93806c2a18c1281fc",
    remote = "https://github.com/madler/zlib.git",
    shallow_since = "1665637615 -0700",
)

# The following rules allow exporting java libraries into maven/artifactory
# For documentation, see https://github.com/bazelbuild/rules_jvm_external/blob/master/docs/api.md
RULES_JVM_EXTERNAL_SHA = (
    "b17d7388feb9bfa7f2fa09031b32707df529f26c91ab9e5d909eb1676badd9a6"
)

# This version is newer than the one provided by org_tensorflow
http_archive(
    name = "rules_jvm_external",
    sha256 = RULES_JVM_EXTERNAL_SHA,
    strip_prefix = "rules_jvm_external-4.5",
    url = "https://github.com/bazelbuild/rules_jvm_external/archive/4.5.zip",
)

load("@rules_jvm_external//:repositories.bzl", "rules_jvm_external_deps")

rules_jvm_external_deps()

load("@rules_jvm_external//:setup.bzl", "rules_jvm_external_setup")

rules_jvm_external_setup()

# If --//bzl/gpu:cuda-support=hermetic the v1-cuda-triplet contains
# the following tools and libraries :
#
# v1-cuda-triplet
# ├── cuda -> cuda_11.2.2
# ├── cuda_11.2.2
# ├── cudnn -> cudnn-linux-x86_64-8.7.0.84_cuda11-archive
# ├── cudnn-linux-x86_64-8.7.0.84_cuda11-archive
# ├── nccl -> nccl_2.16.5-1+cuda11.0_x86_64
# └── nccl_2.16.5-1+cuda11.0_x86_64
#
# If --//bzl/gpu:cuda-support=system the version of the host system will be used if present
#
# Contact https://go/slack-build-infra for a new versions of the hermetic cuda-triplet
#
cuda_triplet(
    name = "v1-cuda-triplet",
    build_file = "//bzl/thirdpartybuild/v1-cuda-triplet:v1-cuda-triplet.BUILD",
    sha256 = "2ccbf0e20fee0dc3cfbb963dcf045819c1c1c9ca7eec62fe963a53ef0c03c191",
    strip_prefix = "cuda-triplet",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/cuda/v1-cuda-triplet.tar",
)

git_repository(
    name = "build_bazel_rules_cuda",
    commit = "29f3ced1b7541ae629bbfabe0c07dbfe76f29f4d",
    remote = "https://github.com/liuliu/rules_cuda.git",
    shallow_since = "1599856094 -0400",
)

load("@build_bazel_rules_cuda//gpus:cuda_configure.bzl", "cuda_configure")

cuda_configure(name = "local_config_cuda")

### Protobuf 3.0.0 from github with the windows protoc.exe patched for supporting bazel 7.0.0
#
http_archive(
    name = "com_google_protobuf_3.0.0",
    sha256 = "50edd0f0a8645cf3bc6dfea6a8ef733dc1b0d17a17609a6f7cd2f3fbd1bf7f42",
    strip_prefix = "3.0.x-niantic-bazel_7.0.0_with_protoc_exe",
    urls = [
        "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/protobuf/protobuf-3.0.0-niantic-bazel_7.0.0_with_protoc_exe.tar",
    ],
)

http_archive(
    name = "openssl",
    build_file = "//third_party/openssl:openssl.BUILD",
    patches = [
        "//third_party/openssl:Configure.patch",
    ],
    sha256 = "bf61b62aaa66c7c7639942a94de4c9ae8280c08f17d4eac2e44644d9fc8ace6f",
    strip_prefix = "openssl-1.1.1p",
    url = "https://www.openssl.org/source/openssl-1.1.1p.tar.gz",
)

# Taking last stable version of https://github.com/google/boringssl/tree/chromium-stable-with-bazel
# to date 230301 that has official support with bazel 6.0
http_archive(
    name = "boringssl",
    patch_args = ["-p1"],
    patches = ["//third_party/boringssl:boringssl-51bd4554d-230301_132340-windows_fix.patch"],
    sha256 = "5b7065b94542db8909f7988a8a4388f075cc55517cfd8424bd345eeace9e9c4e",
    strip_prefix = "boringssl-51bd4554d-230301_132341",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/boringssl/boringssl-51bd4554d-230301_132341.tar",
)

### Rules proto grpc project configured for nia protobuf - BEGIN

# General Settings - https://rules-proto-grpc.com/en/latest/index.html#installation
http_archive(
    name = "rules_proto_grpc",
    sha256 = "c0d718f4d892c524025504e67a5bfe83360b3a982e654bc71fed7514eb8ac8ad",
    strip_prefix = "rules_proto_grpc-4.6.0",
    urls = ["https://github.com/rules-proto-grpc/rules_proto_grpc/archive/refs/tags/4.6.0.tar.gz"],
)

load(
    "@rules_proto_grpc//:repositories.bzl",
    "rules_proto_grpc_repos",
    "rules_proto_grpc_toolchains",
)

rules_proto_grpc_toolchains()

rules_proto_grpc_repos()

load(
    "@rules_proto//proto:repositories.bzl",
    "rules_proto_dependencies",
    "rules_proto_toolchains",
)

rules_proto_dependencies()

rules_proto_toolchains()

# CSharp support - https://rules-proto-grpc.com/en/latest/lang/csharp.html#workspace
load(
    "@rules_proto_grpc//csharp:repositories.bzl",
    rules_proto_grpc_csharp_repos = "csharp_repos",
)

rules_proto_grpc_csharp_repos()

load("@io_bazel_rules_dotnet//dotnet:deps.bzl", "dotnet_repositories")

dotnet_repositories()

load(
    "@io_bazel_rules_dotnet//dotnet:defs.bzl",
    "dotnet_register_toolchains",
    "dotnet_repositories_nugets",
)

dotnet_register_toolchains()

dotnet_repositories_nugets()

load("@rules_proto_grpc//csharp/nuget:nuget.bzl", "nuget_rules_proto_grpc_packages")

nuget_rules_proto_grpc_packages()

# gRPC support - https://rules-proto-grpc.com/en/latest/lang/cpp.html#id4

load("@rules_proto_grpc//cpp:repositories.bzl", rules_proto_grpc_cpp_repos = "cpp_repos")

rules_proto_grpc_cpp_repos()

### Rules proto grpc project configured for default protobuf - END

npm_package(
    name = "npm-c8-model-web",  # For targets in //c8/model/web
    package = "//c8/model/web/npm:package.json",
    package_lock = "//c8/model/web/npm:package-lock.json",
)

npm_package(
    name = "npm-bzl-examples",  # For targets in //bzl/examples
    package = "//bzl/examples/npm:package.json",
    package_lock = "//bzl/examples/npm:package-lock.json",
)

npm_package(
    name = "npm-bzl-httpfileserver",  # For targets in //bzl/httpfileserver
    package = "//bzl/httpfileserver/npm:package.json",
    package_lock = "//bzl/httpfileserver/npm:package-lock.json",
)

npm_package(
    name = "npm-examples-js-resolve",
    package = "//bzl/examples/js/resolve:package.json",
    package_lock = "//bzl/examples/js/resolve:package-lock.json",
)

npm_package(
    name = "npm-desktop",
    export_zip = True,
    package = "//apps/desktop:package.json",
    package_lock = "//apps/desktop:package-lock.json",
)

npm_package(
    name = "npm-protoc-gen-ts",
    exports_files = [
        "node_modules/.bin/protoc-gen-ts",
    ],
    package = "//bzl/js/proto:package.json",
    package_lock = "//bzl/js/proto:package-lock.json",
)
# End npm_package rules.

git_repository(
    name = "glslang",
    commit = "adf7bf0113ba99fb8e49b23ba7f30c6ee277d14b",
    patches = [
        "//third_party/glslang:BUILD.bazel.patch",
    ],
    remote = "https://github.com/KhronosGroup/glslang.git",
    shallow_since = "1659554541 -0600",
)

git_repository(
    name = "spirv_headers",
    commit = "8b246ff75c6615ba4532fe4fde20f1be090c3764",  # vulkan-sdk-1.3.280
    remote = "https://github.com/KhronosGroup/SPIRV-Headers.git",
    shallow_since = "1709319984 -0800",
)

git_repository(
    name = "spirv_tools",
    commit = "04896c462d9f3f504c99a4698605b6524af813c1",  # vulkan-sdk-1.3.280
    remote = "https://github.com/KhronosGroup/spirv-tools.git",
    shallow_since = "1709825474 -0500",
)

# Libwebsockets - Websocket C-API library
# Licence: MIT (https://github.com/warmcat/libwebsockets/blob/main/LICENSE)
new_git_repository(
    name = "libwebsockets",
    build_file = "//third_party/libwebsockets:libwebsockets.BUILD",
    commit = "b0a749c8e7a8294b68581ce4feac0e55045eb00b",  # v4.3.2
    patches = [
        "//third_party/libwebsockets:sha-1.c.patch",
        "//third_party/libwebsockets:private-lib-plat-windows.h.patch",
        "//third_party/libwebsockets:ops-ws.c.patch",
    ],
    remote = "https://github.com/warmcat/libwebsockets.git",
    shallow_since = "1652806407 +0100",
)

new_git_repository(
    name = "imgui",
    build_file = "//bzl/thirdpartybuild:imgui.BUILD",
    commit = "1ad1429c6df657f9694b619d53fa0e65e482f32b",
    patches = [
        "@the8thwall//third_party/imgui:imconfig.h.patch",
        "@the8thwall//third_party/imgui:examples/example_apple_opengl2/main.mm.patch",
    ],
    remote = "https://github.com/ocornut/imgui",
    shallow_since = "1621961154 +0200",
)

new_git_repository(
    name = "python-gitlab",
    build_file = "//third_party/python-gitlab:python-gitlab.BUILD",
    commit = "dde3642bcd41ea17c4f301188cb571db31fe4da8",
    patch_args = [
        "-p1",
    ],
    patches = [
        "//third_party/python-gitlab:history-of-scheduled-pipelines.patch",
    ],
    remote = "https://github.com/python-gitlab/python-gitlab.git",
    shallow_since = "1666584850 -0700",
)

niantic_maybe(
    http_archive,
    name = "com_github_google_glog",
    sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
    strip_prefix = "glog-0.6.0",
    urls = ["https://github.com/google/glog/archive/v0.6.0.zip"],
)

niantic_maybe(
    http_archive,
    name = "tbb",
    patch_args = ["-p1"],
    patches = [
        "//third_party/tbb:BUILD.bazel.patch",
    ],
    sha256 = "782ce0cab62df9ea125cdea253a50534862b563f1d85d4cda7ad4e77550ac363",
    strip_prefix = "oneTBB-2021.11.0",
    urls = ["https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2021.11.0.tar.gz"],
)

niantic_maybe(
    git_repository,
    name = "com_github_googleapis_google_cloud_cpp",
    commit = "fcbbc055d3100b67096857f4fae9ee09aa700c79",
    patch_args = ["-p1"],
    patches = [
        "//bzl/thirdpartybuild/googlecloud:json_external_repo_name.patch",
        "//bzl/thirdpartybuild/googlecloud:repo_mapping.patch",
        "//bzl/thirdpartybuild/googlecloud:macos_m1.patch",
        "//bzl/thirdpartybuild/googlecloud:remove_ambiguous_macos_x86_64_config_setting.patch",
        "//bzl/thirdpartybuild/googlecloud:support_protobuf_nia.patch",
    ],
    remote = "https://github.com/googleapis/google-cloud-cpp.git",
    repo_mapping = {
        "@com_github_curl_curl": "@curl",
    },
    shallow_since = "1614618757 -0500",
)

load(
    "@com_github_googleapis_google_cloud_cpp//bazel:google_cloud_cpp_deps.bzl",
    "google_cloud_cpp_deps",
)

google_cloud_cpp_deps()

http_toolchain(
    name = "amazonlinux",
    build_file = "//bzl/thirdpartybuild:linux.BUILD",
    sha256 = "3642cba09a62aa6ad47226a8999fd2980b092e1b79490d9f223a73470ae91e78",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/amazonlinux8/amazonlinux8-crosstool-2021-06-09.tar.xz",
)

http_toolchain(
    name = "v1-linux",
    build_file = "//bzl/thirdpartybuild:linux.BUILD",
    sha256 = "524d11ed157cea2a840048f302ce2ca719f7c5caa654a8af00cdd472c6339c3b",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/linux/x86_64-niantic_v1.0-linux-gnu--231206_114936.tar",
)

http_toolchain(
    name = "v2-linux",
    build_file = "//bzl/thirdpartybuild:linux.BUILD",
    sha256 = "585ebdbf2e95d4973cf0cb49083707f2c84fbad48c89674f2cfb998461f94455",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/linux/x86_64-niantic_v2.0-linux-gnu--230202_120122.tar",
)

http_archive(
    name = "bazel-contrib-rules_cuda",
    patch_args = ["-p1"],
    patches = [
        "//bzl/thirdpartybuild/bazel-contrib-rules_cuda:change_generated_external_repo_name.patch",
        "//bzl/thirdpartybuild/bazel-contrib-rules_cuda:hermetic_host_compiler.patch",
        "//bzl/thirdpartybuild/bazel-contrib-rules_cuda:dummy_local_cuda_BUILD.patch",
    ],
    sha256 = "dc1f4f704ca56e3d5edd973f98a45f0487d0f28c689d0a57ba236112148b1833",
    strip_prefix = "rules_cuda-v0.1.2",
    urls = [
        "https://github.com/bazel-contrib/rules_cuda/releases/download/v0.1.2/rules_cuda-v0.1.2.tar.gz",
    ],
)

load(
    "@bazel-contrib-rules_cuda//cuda:repositories.bzl",
    "register_detected_cuda_toolchains",
    "rules_cuda_dependencies",
)

rules_cuda_dependencies()

register_detected_cuda_toolchains()

# The following two lines are present to avoid build failures during analysis of
# targets that require a CUDA toolchain for unsupported platforms.
# See 'register_dummy_toolchains.bzl' for more details/explanation.
load(
    "//bzl/thirdpartybuild/bazel-contrib-rules_cuda:register_dummy_toolchains.bzl",
    "register_dummy_cuda_toolchain",
)

register_dummy_cuda_toolchain()

load("@com_google_protobuf//:protobuf_deps.bzl", nia_protobuf_deps = "protobuf_deps")

nia_protobuf_deps()

# Downloaded from https://gitlab.com/<REMOVED_BEFORE_OPEN_SOURCING>/repos/legacy/niantic-ar/3rd-party/angle/-/tags
http_archive(
    name = "angle",
    sha256 = "42c4d922e2b78732c2145445907d72f6cf551a9a3ea378528ad3a09704e3b3a4",
    strip_prefix = "angle-nia-5943-p3",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/angle/angle-nia-5943-p3.tar.gz",
)

http_archive(
    name = "ring",
    build_file = "//bzl/thirdpartybuild:ring.BUILD",
    sha256 = "a4689e6c2294d81e88dc6261c768b63bc4fcdb852be6d1352498b114f61383b7",
    strip_prefix = "ring-0.17.14",
    urls = ["https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/ring/ring-0.17.14.tar.gz"],
)

new_git_repository(
    name = "chromium-icu",
    build_file = "@node//deps/v8:bazel/BUILD.icu",
    commit = "985b9a6f70e13f3db741fed121e4dcc3046ad494",
    patch_args = ["-p1"],
    patches = [
        "//third_party/v8:chromium-icu.patch",
    ],
    remote = "https://chromium.googlesource.com/chromium/deps/icu.git",
    shallow_since = "1693432754 +0000",
)

http_archive(
    name = "node",
    patch_args = ["-p1"],
    patches = [
        "//third_party/node-nia:node-nia.bazel7.patch",
    ],
    sha256 = "f0d6ac768686664973353743b78334a25ca5e3dd0444b53de14b2eda98b542f1",
    strip_prefix = "node-nia-v20.6.1-p11",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/node/node-nia-v20.6.1-p11.tar",
)
