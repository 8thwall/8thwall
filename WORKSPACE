WORKSPACE_NAME = "the8thwall"

workspace(name = WORKSPACE_NAME)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")
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

# Explicitly overriding remote_coverage_tools
# Needed by bazel query because bazel internals do not offer a public alternative like others (https://bit.ly/3FkzxHz)
# IMPORTANT!! Remember to check distdir_defs.bzl on bazel branch
# at tag (https://bit.ly/46AFKv8)everytime we upgrade bazel version
local_repository(
    name = "remote_coverage_tools",
    path = "third_party/remote_coverage_tools",
)

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

pip_parse(
    name = "a4lidartag_deps",
    envsubst = ["PIP_INDEX_URL"],
    extra_pip_args = [
        "--index-url",
        "${PIP_INDEX_URL:-https://pypi.org/simple}",
    ],
    python_interpreter_target = python_interpreter,
    requirements_lock = "//bzl/python:a4lidartag_requirements.txt",
)

# Define repos for python pip dependencies.
load("@a4lidartag_deps//:requirements.bzl", a4lidartag_install_deps = "install_deps")

a4lidartag_install_deps()

pip_parse(
    name = "whismur_deps",
    envsubst = ["PIP_INDEX_URL"],
    extra_pip_args = [
        "--index-url",
        "${PIP_INDEX_URL:-https://pypi.org/simple}",
    ],
    python_interpreter_target = python_interpreter,
    requirements_lock = "//bzl/python:whismur_requirements.txt",
)

# Define repos for python pip dependencies.
load("@whismur_deps//:requirements.bzl", whismur_install_deps = "install_deps")

whismur_install_deps()

pip_parse(
    name = "benchmark_quest_deps",
    envsubst = ["PIP_INDEX_URL"],
    extra_pip_args = [
        "--index-url",
        "${PIP_INDEX_URL:-https://pypi.org/simple}",
    ],
    python_interpreter_target = python_interpreter,
    requirements_lock = "//bzl/python:benchmark_quest_requirements.txt",
)

# Define repos for python pip dependencies.
load("@benchmark_quest_deps//:requirements.bzl", benchmark_quest_install_deps = "install_deps")

benchmark_quest_install_deps()

pip_parse(
    name = "nae_publish_deps",
    envsubst = ["PIP_INDEX_URL"],
    extra_pip_args = [
        "--index-url",
        "${PIP_INDEX_URL:-https://pypi.org/simple}",
    ],
    python_interpreter_target = python_interpreter,
    requirements_lock = "//bzl/python:nae_publish_requirements.txt",
)

# Define repos for python pip dependencies.
load("@nae_publish_deps//:requirements.bzl", nae_publish_install_deps = "install_deps")

nae_publish_install_deps()

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

# Mono toolchains.
http_archive(
    name = "mono-macosx",
    build_file = "//bzl/thirdpartybuild:mono.BUILD",
    sha256 = "8d569e7dc07207a3e008b3756bb1f10a93a720e325dcf559e6e0e5a449b94421",
    strip_prefix = "Mono.framework/Versions/6.12.0",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/mono/Mono.framework-6.12.0-macosx-x86_64.tar",
)

# Register mono toolchains.
register_toolchains(
    "//bzl/mono:mono-macosx",
)

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
    exports_files = [
        "node_modules/eslint/bin/eslint.js",
    ],
    package = "//bzl/npmpackage/eslint:package.json",
    package_lock = "//bzl/npmpackage/eslint:package-lock.json",
    patches = [
        "//bzl/npmpackage/eslint/patches:eslint-plugin-local-rules+0.1.1.patch",
    ],
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

# Node modules for Discord Activity Backend.
npm_package(
    name = "npm-discord-activity",
    package = "//apps/client/exploratory/discord-activity-example:package.json",
    package_lock = "//apps/client/exploratory/discord-activity-example:package-lock.json",
)

# Node modules for 8th Wall tune-parameters.
npm_package(
    name = "npm-tune-parameters",
    package = "//bzl/npmpackage/tune-parameters:package.json",
    package_lock = "//bzl/npmpackage/tune-parameters:package-lock.json",
)

# Node modules for 8th Wall responsive immersive.
npm_package(
    name = "npm-responsive-immersive",
    package = "//apps/client/public/web/responsive-immersive:package.json",
    package_lock = "//apps/client/public/web/responsive-immersive:package-lock.json",
)

# Node modules for 8th Wall's js engine.
npm_package(
    name = "npm-jsxr",
    package = "//reality/app/xr/js:package.json",
    package_lock = "//reality/app/xr/js:package-lock.json",
)

# Node modules for 8th Wall's Omniscope js app.
npm_package(
    name = "npm-omni-js",
    package = "//apps/client/internalqa/omniscope/js:package.json",
    package_lock = "//apps/client/internalqa/omniscope/js:package-lock.json",
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

# Node modules for ecr in reality/cloud/aws/ecr
npm_package(
    name = "npm-ecr",
    package = "//reality/app/nae/npm:package.json",
    package_lock = "//reality/app/nae/npm:package-lock.json",
)

# Node modules for nae-assets-car-api in reality/cloud/aws/cdk/nae-assets-car-api
npm_package(
    name = "npm-nae-assets-car-api",
    package = "//reality/cloud/aws/cdk/nae-assets-car-api/src/api:package.json",
    package_lock = "//reality/cloud/aws/cdk/nae-assets-car-api/src/api:package-lock.json",
)

# Node modules for scanmap.
npm_package(
    name = "npm-scanmap",
    package = "//apps/client/exploratory/scanmap/npm:package.json",
    package_lock = "//apps/client/exploratory/scanmap/npm:package-lock.json",
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

# These are execution platforms that are capable of compiling code.
register_execution_platforms(
    "@local_execution_config_platform//:platform",
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

load("//bzl/unity:unity-version.bzl", "unity_version")

# Creates a 'platform' and 'toolchain' for each version of Unity installed
# locally (e.g., 2021.3.37f1), and creates a 'constraint_value' for any strings
# added to the versions attribute for any strings added to the versions
# attribute.
#
# To install new Unity versions on your mac, use the following example command:
#   brew install unity-hub
#   /Applications/Unity\ Hub.app/Contents/MacOS/Unity\ Hub -- --headless install -v "2021.3.37f1" -m android ios mac-il2cpp windows-mono
#
# If a new Unity version is installed that matches any of the below versions,
# run 'bazel clean' to update future executions.
unity_version(
    name = "unity-version",
    unity_search_paths = [
        "/Applications/Unity/Hub/Editor",  # Default for macOS
        "/opt/niantic/public/unity",  # Default for Linux
        "/c/Program Files/Unity/Hub/Editor",  # Default for Windows (WSL or msys2 format)
    ],
    # Each string below creates a version constraint_value that can be used in
    # unity rules. Valid strings are major versions, e.g "2022", minor version
    # "2021.3" or patch versions "2021.3.37f1". These are then accessed as
    # targets such as "@unity-version//:2021.3".
    versions = [
        "6000.0",
        "2022.3",
        "2021.3",
        "2020.3",
        "2019.4",
        "2018.4",
    ],
)

load("@unity-version//:repo.bzl", "register_unity_toolchains")

register_unity_toolchains()

# Install a newer version of googletest than is provided in org_tensorflow.
http_archive(
    name = "com_google_googletest",
    patch_args = ["-p1"],
    patches = [
        "//third_party/googletest:googletest-1.14.0-wasm_support.patch",
    ],
    sha256 = "8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7",
    strip_prefix = "googletest-1.14.0",
    urls = [
        "https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz",
    ],
)

# Install a newer version of vulkan_headers than is provided in org_tensorflow.
http_archive(
    name = "vulkan_headers",
    build_file = "//bzl/thirdpartybuild:vulkan-headers.BUILD",
    sha256 = "45a8c99f867a686b85bbb6fa460dad41840ecfa1ca111a29f044387f47308dce",
    strip_prefix = "Vulkan-Headers-1.3.296",
    url = "https://github.com/KhronosGroup/Vulkan-Headers/archive/refs/tags/v1.3.296.zip",
)

new_git_repository(
    name = "vulkan-memory-allocator",
    build_file = "//bzl/thirdpartybuild:vulkan-memory-allocator.BUILD",
    commit = "e87036508bb156f9986ea959323de1869e328f58",
    remote = "https://chromium.googlesource.com/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git",
    shallow_since = "1689220916 -0400",
)

http_archive(
    name = "vulkan-utility-libraries",
    build_file = "//bzl/thirdpartybuild:vulkan-utility-libraries.BUILD",
    sha256 = "9dc5247bfb1585ecab48fdd4708b52ba1839cebf0347077bbc897580401b15ca",
    strip_prefix = "Vulkan-Utility-Libraries-1.3.295",
    url = "https://github.com/KhronosGroup/Vulkan-Utility-Libraries/archive/v1.3.295.tar.gz",
)

http_archive(
    name = "com_google_benchmark",
    build_file = "//bzl/thirdpartybuild:benchmark.BUILD",
    sha256 = "3aff99169fa8bdee356eaa1f691e835a6e57b1efeadb8a0f9f228531158246ac",
    strip_prefix = "benchmark-1.7.0",
    url = "https://github.com/google/benchmark/archive/refs/tags/v1.7.0.tar.gz",
)

git_repository(
    name = "cpuinfo",
    commit = "8ec7bd91ad0470e61cf38f618cc1f270dede599c",
    patch_args = ["-p1"],
    patches = [
        "//third_party/cpuinfo:cpuinfo.fix_support_for_wasm.patch",
        "//third_party/cpuinfo:cpuinfo.support_cpu_darwin_transition.patch",
        "//third_party/cpuinfo:cpuinfo.bazel7_detected_correctly_k8.patch",
    ],
    remote = "https://github.com/pytorch/cpuinfo.git",
    repo_mapping = {"@org_pytorch_cpuinfo": "@cpuinfo"},
    shallow_since = "1660926227 -0700",
)

git_repository(
    name = "miniaudio",
    build_file = "//bzl/thirdpartybuild:miniaudio.BUILD",
    commit = "4a5b74bef029b3592c54b6048650ee5f972c1a48",  # v0.11.21
    remote = "https://github.com/mackron/miniaudio.git",
    shallow_since = "1700011380 +1000",
)

# Source repo: https://github.com/8thwall/miniaudio-addon
http_archive(
    name = "miniaudio-addon",
    sha256 = "b70cfaaae5cfe2d0974a4ae3e95b85117d5a145c797a2b900758e60a0aac05a3",
    strip_prefix = "miniaudio-addon-0.11.21-p17",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/miniaudio-addon/miniaudio-addon-0.11.21-p17.tar.gz",
)

# Install a newer version of gflags than is provided in org_tensorflow.
git_repository(
    name = "com_github_gflags_gflags",
    commit = "986e8eed00ded8168ef4eaa6f925dc6be50b40fa",
    remote = "https://github.com/gflags/gflags",
    shallow_since = "1641684284 +0000",
)

# Mirror XNNPACK that is provided in org_tensorflow to add patch.
http_archive(
    name = "XNNPACK",
    build_file = "//third_party/xnnpack:xnnpack.BUILD",
    patch_args = ["-p1"],
    patches = [
        "//third_party/xnnpack:xnnpack_add_android_x86_64_linkopt.patch",  # Fix Bazel ambiguous match error
        "//third_party/xnnpack:xnnpack.llvm_windows_support.patch",
        "//third_party/xnnpack:xnnpack.remove_linux_k8_linkopts.patch",  # Resolves ambiguous match on linkopts for :linux_k8 and :android
    ],
    sha256 = "7a16ab0d767d9f8819973dbea1dc45e4e08236f89ab702d96f389fdc78c5855c",
    strip_prefix = "XNNPACK-e8f74a9763aa36559980a0c2f37f587794995622",
    url = "https://github.com/google/XNNPACK/archive/e8f74a9763aa36559980a0c2f37f587794995622.zip",
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

# Zlib with chromium optimizations and extra contributed utility APIs.
new_git_repository(
    name = "zlib-chromium",
    build_file = "//bzl/thirdpartybuild:zlib-chromium.BUILD",
    commit = "f5fd0ad2663e239a31184ad4c9919991dda16f46",
    remote = "https://chromium.googlesource.com/chromium/src/third_party/zlib",
    shallow_since = "1691776355 -0700",
)

new_git_repository(
    name = "cnpy",
    build_file = "//bzl/thirdpartybuild:cnpy.BUILD",
    commit = "4e8810b1a8637695171ed346ce68f6984e585ef4",
    remote = "https://github.com/rogersce/cnpy.git",
    shallow_since = "1527823740 +0000",
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

# Nia Protobuf 25.3.0 based section - it has to be defined before org_tensorflow section - BEGIN

http_archive(
    name = "com_github_grpc_grpc",
    patch_args = ["-p1"],
    patches = [
        "//third_party/grpc:grpc-v1.27.3-support_nia_protobuf_25.3.patch",
    ],
    sha256 = "c2ab8a42a0d673c1acb596d276055adcc074c1116e427f118415da3e79e52969",
    strip_prefix = "grpc-1.27.3",
    urls = ["https://github.com/grpc/grpc/archive/refs/tags/v1.27.3.tar.gz"],
)

# The public protobuf repo for the 25.3 release (v3.25.3).  This archive is patched using patch
# files from nia-protobuf-internal, which includes Niantic-specific changes to protoc and the
# C# runtime assembly, as described in this document: https://go/niantic-internal-protobuf-doc
http_archive(
    name = "com_google_protobuf",
    sha256 = "674afb2d0daaf266859b4fa8f9f1c051dbac56800a621dfb48d2302842960bfe",
    strip_prefix = "protobuf-25.3-nia-v0.3",
    urls = ["https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/protobuf/protobuf-25.3-nia-v0.3.tar"],
)

# Nia Protobuf 25.3.0 based section - it has to be defined before org_tensorflow section - END

http_archive(
    name = "org_tensorflow",
    patches = [
        "//third_party/tensorflow:tensorflow-v2.11.0-p8-bazel_6.1.2.patch",
        "//third_party/tensorflow:tensorflow-inject_flatc_redist.patch",
        "//third_party/tensorflow:tf_protobuf_nia_v0.1.patch",
        "//third_party/tensorflow:tensorflow_minosversion.patch",
        "//third_party/tensorflow:tensorflow-v2.11.0-p8-disable_llvm-raw.patch",
        "//third_party/tensorflow:tensorflow-v2.11.0-p8-fix_ubuntu_local_python_configuration.patch",
    ],
    sha256 = "dcf38db689396ac3ae846b060639a0b0f88589acd61fb9100ef30b101d529172",
    strip_prefix = "tensorflow-v2.11.0-p8",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/tensorflow/tensorflow-v2.11.0-p8.tar",
)

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

# More modern protobuf version for use of later protobuf v3 features
# Assumption is made that the presence of this http_archive does not disrupt
# the functionality of the protobuf v3.9.2 embedded in the 8w fork of TF, and
# the protobuf v3.0.0 included above.
http_archive(
    name = "com_google_protobuf_3.17.3",
    sha256 = "528927e398f4e290001886894dac17c5c6a2e5548f3fb68004cfb01af901b53a",
    strip_prefix = "protobuf-3.17.3",
    urls = [
        "https://github.com/protocolbuffers/protobuf/archive/v3.17.3.zip",
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

new_git_repository(
    name = "libevent",
    build_file = "//third_party/libevent:libevent.BUILD",
    commit = "e7ff4ef2b4fc950a765008c18e74281cdb5e7668",
    remote = "https://github.com/libevent/libevent",
    shallow_since = "1485387435 +0300",
)

# Initialize the TensorFlow repository and all dependencies.
#
# The cascade of load() statements and tf_workspace?() calls works around the
# restriction that load() statements need to be at the top of .bzl files.
# E.g. we can not retrieve a new repository with http_archive and then load()
# a macro from that repository in the same file.
load("@org_tensorflow//tensorflow:workspace3.bzl", "tf_workspace3")

tf_workspace3()

load("@org_tensorflow//tensorflow:workspace2.bzl", "tf_workspace2")

tf_workspace2()

load("@org_tensorflow//tensorflow:workspace1.bzl", "tf_workspace1")

tf_workspace1()

load("@org_tensorflow//tensorflow:workspace0.bzl", "tf_workspace0")

tf_workspace0()

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

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

### Rules proto grpc project configured for default protobuf - END

http_archive(
    name = "eigen3",
    build_file = "//bzl/thirdpartybuild:eigen3.BUILD",
    patches = [
        "//third_party/eigen3:TriangularSolver.h.patch",
    ],
    sha256 = "8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72",
    strip_prefix = "eigen-3.4.0",
    url = "https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz",
)

http_archive(
    name = "giflib",
    build_file = "//bzl/thirdpartybuild:giflib.BUILD",
    sha256 = "be7ffbd057cadebe2aa144542fd90c6838c6a083b5e8a9048b8ee3b66b29d5fb",
    strip_prefix = "giflib-5.2.2",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/giflib/giflib-5.2.2.tar.gz",
)

new_git_repository(
    name = "libjpegturbo",
    build_file = "//bzl/thirdpartybuild:libjpegturbo.BUILD",
    commit = "9171fd4bdef0f3aecba61413b858b1766bbad8d5",
    patch_args = ["-p1"],
    patches = [
        "//third_party/libjpegturbo:turbojpeg.c.patch",
    ],
    remote = "https://github.com/libjpeg-turbo/libjpeg-turbo.git",
    shallow_since = "1651080868 -0500",
)

new_git_repository(
    name = "libyuv",
    build_file = "//bzl/thirdpartybuild:libyuv.BUILD",
    commit = "de71c67e53d79190b5b7cc760ade9027855dd945",
    remote = "https://chromium.googlesource.com/libyuv/libyuv",
    shallow_since = "1651045085 +0000",
)

new_git_repository(
    name = "json",
    build_file = "//bzl/thirdpartybuild:json.BUILD",
    commit = "4f8fba14066156b73f1189a2b8bd568bde5284c5",
    remote = "https://github.com/nlohmann/json.git",
    shallow_since = "1641188409 +0100",
)

new_git_repository(
    name = "yaml-cpp",
    build_file = "//bzl/thirdpartybuild:yaml-cpp.BUILD",
    commit = "9a3624205e8774953ef18f57067b3426c1c5ada6",
    remote = "https://github.com/jbeder/yaml-cpp",
    shallow_since = "1569430560 -0700",
)

http_archive(
    name = "cli11",
    build_file = "//bzl/thirdpartybuild:cli11.BUILD",
    sha256 = "562c4be7507dc6fb4997ecd648bf935d84efe17b54715fa5cfbddac05279f668",
    strip_prefix = "CLI11-2.3.2",
    url = "https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.3.2.zip",
)

# Log formatter for XCode build output.
new_git_repository(
    name = "xcpretty",
    build_file_content = """
filegroup(
    name = 'xcpretty',
    srcs = glob(
        include=['**'],
        exclude=['features/assets/**'],
    ),
    visibility = ["//visibility:public"],
)""",
    commit = "fb3afc6f4495fa89c2dcb351b274eacb74595285",
    remote = "https://github.com/supermarin/xcpretty.git",
    shallow_since = "1476195465 -0700",
)

# Node modules for trees in the workspace. These should cover all of the js_binary, js_cli, and
# js_test rules under a given tree in the workspace.
npm_package(
    name = "npm-apps-client-gitlab",  # For targets in //apps/client/gitlab
    package = "//apps/client/gitlab/npm:package.json",
    package_lock = "//apps/client/gitlab/npm:package-lock.json",
)

npm_package(
    name = "npm-c8-model-web",  # For targets in //c8/model/web
    package = "//c8/model/web/npm:package.json",
    package_lock = "//c8/model/web/npm:package-lock.json",
)

npm_package(
    name = "npm-apps-client-studio2d-web3js",  # For targets in //apps/client/studio2d/web3js
    package = "//apps/client/studio2d/web3js/npm:package.json",
    package_lock = "//apps/client/studio2d/web3js/npm:package-lock.json",
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
    name = "npm-ci-support",  # For targets in //ci-support
    package = "//ci-support/npm:package.json",
    package_lock = "//ci-support/npm:package-lock.json",
)

npm_package(
    name = "npm-platform-client-geo-game-board",  # For targets in //platform/client/geo-game-board
    package = "//platform/client/geo-game-board/npm:package.json",
    package_lock = "//platform/client/geo-game-board/npm:package-lock.json",
)

npm_package(
    name = "npm-examples-js-resolve",
    package = "//bzl/examples/js/resolve:package.json",
    package_lock = "//bzl/examples/js/resolve:package-lock.json",
)

npm_package(
    name = "npm-lambda-edge-log",
    package = "//reality/cloud/aws/edge-lambda/lambda-edge-log:package.json",
    package_lock = "//reality/cloud/aws/edge-lambda/lambda-edge-log:package-lock.json",
)

npm_package(
    name = "npm-studio-deploy",
    export_zip = True,
    package = "//reality/cloud/aws/lambda/studio-deploy:package.json",
    package_lock = "//reality/cloud/aws/lambda/studio-deploy:package-lock.json",
    patches = [
        "//reality/cloud/aws/lambda/studio-deploy/patches:ts-loader+6.2.2.patch",
    ],
)

npm_package(
    name = "npm-ecs-build",
    export_zip = True,
    package = "//reality/cloud/studio-local/ecs-build:package.json",
    package_lock = "//reality/cloud/studio-local/ecs-build:package-lock.json",
)

npm_package(
    name = "npm-studiohub",
    export_zip = True,
    package = "//reality/cloud/studiohub:package.json",
    package_lock = "//reality/cloud/studiohub:package-lock.json",
)

npm_package(
    name = "npm-public-api",
    export_zip = True,
    package = "//reality/cloud/aws/lambda/public-api:package.json",
    package_lock = "//reality/cloud/aws/lambda/public-api:package-lock.json",
)

npm_package(
    name = "npm-nae-lambda-builder",
    package = "//reality/cloud/aws/cdk/nae-lambda-builder:package.json",
    package_lock = "//reality/cloud/aws/cdk/nae-lambda-builder:package-lock.json",
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

# Created by //reality/cloud/aws/edge-lambda/serve-images/zip-sharp-linux.sh
http_file(
    name = "sharp-linux-install",
    sha256 = "ca33ba4092e1c9ea4d9f68069d06c71563aa90c0ede7b3f14b6ebfe7dfb0e00b",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/sharp/sharp-linux-install-lynjnd4y.zip",
)

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

new_git_repository(
    name = "spirv_cross",
    build_file = "//bzl/thirdpartybuild:spirv-cross.BUILD",
    commit = "61c603f3baa5270e04bcfb6acf83c654e3c57679",
    remote = "https://github.com/KhronosGroup/SPIRV-Cross.git",
    shallow_since = "1660048855 +0200",
)

new_git_repository(
    name = "opencv",
    build_file = "//bzl/thirdpartybuild:opencv.BUILD",
    commit = "70bbf17b133496bd7d54d034b0f94bd869e0e810",
    remote = "https://github.com/opencv/opencv",
    shallow_since = "1482497684 +0300",
)

new_git_repository(
    name = "opencv_contrib",
    build_file = "//bzl/thirdpartybuild:opencv_contrib.BUILD",
    commit = "86342522b0eb2b16fa851c020cc4e0fef4e010b7",
    remote = "https://github.com/opencv/opencv_contrib",
    shallow_since = "1482491383 +0200",
)

new_git_repository(
    name = "vectorclass",
    build_file = "//bzl/thirdpartybuild:vectorclass.BUILD",
    commit = "08959ebe6ea5d8317330b242e28ba0d2938ac52f",
    remote = "https://github.com/vectorclass/version2",
    shallow_since = "1659863902 +0200",
)

new_git_repository(
    name = "s2geometry",
    build_file = "//bzl/thirdpartybuild:s2geometry.BUILD",
    commit = "c5055c076bd22281c67445d1df4f3225bfbf9925",
    remote = "https://github.com/google/s2geometry.git",
    shallow_since = "1648814009 +0200",
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
    name = "imnodes",
    build_file = "//bzl/thirdpartybuild:imnodes.BUILD",
    commit = "0fbc7f1a2aab73b3ad4be86241bf3f4802bdb800",
    remote = "https://github.com/Nelarius/imnodes",
    shallow_since = "1626262086 +0300",
)

new_git_repository(
    name = "implot",
    build_file = "//bzl/thirdpartybuild:implot.BUILD",
    commit = "6ee1559715fae9480fcaeb81f24d80a4d1e8c407",
    remote = "https://github.com/epezent/implot",
    shallow_since = "1634698866 -0700",
)

new_git_repository(
    name = "capnproto",
    build_file = "//bzl/thirdpartybuild:capnproto.BUILD",
    commit = "c3c2655f6d6efeb93ae6b3ea718eecd00b72f5d0",
    remote = "https://github.com/8thwall/capnproto.git",
    shallow_since = "1699486035 -0800",
)

new_git_repository(
    name = "capnproto_java",
    build_file = "//bzl/thirdpartybuild:capnproto_java.BUILD",
    commit = "be364f41c66b8c41484e0cb5bc25f726613cc04a",
    remote = "https://github.com/dwrensha/capnproto-java",
    shallow_since = "1488309561 -0500",
)

new_git_repository(
    name = "capnproto_python",
    build_file = "//bzl/thirdpartybuild:capnproto_python.BUILD",
    commit = "b3ab9ab2a6eeb493ee2fe1d61a09a933828145c7",
    remote = "https://github.com/jparyani/pycapnp.git",
    shallow_since = "1501210794 -0700",
)

new_git_repository(
    name = "libgit2",
    build_file = "//bzl/thirdpartybuild:libgit2.BUILD",
    commit = "78fde8cde56492d464575e3bf289056c8289cec8",
    remote = "https://github.com/8thwall/libgit2.git",
    shallow_since = "1731372343 -0800",
)

# Argument parsing library
new_git_repository(
    name = "cxxopts",
    build_file = "//bzl/thirdpartybuild:cxxopts.BUILD",
    commit = "302302b30839505703d37fb82f536c53cf9172fa",
    remote = "https://github.com/jarro2783/cxxopts.git",
    shallow_since = "1596495594 +1000",
)

new_git_repository(
    name = "kissfftlib",
    build_file = "//bzl/thirdpartybuild:kissfft.BUILD",
    commit = "8f47a67f595a6641c566087bf5277034be64f24d",
    remote = "https://github.com/mborgerding/kissfft.git",
    shallow_since = "1612920557 -0500",
)

new_git_repository(
    name = "ceres",
    build_file = "//bzl/thirdpartybuild:ceres.BUILD",
    commit = "2b32b321242b32f6364108694e0069f87e56326c",
    remote = "https://github.com/ceres-solver/ceres-solver",
    shallow_since = "1613587123 +0000",
)

new_git_repository(
    name = "ceres2",
    build_file = "//bzl/thirdpartybuild:ceres2.BUILD",
    commit = "fd6197ce0ef5794bba455fe6f907dcdabcf624eb",
    remote = "https://github.com/ceres-solver/ceres-solver",
    shallow_since = "1686411471 +0200",
)

new_git_repository(
    name = "png",
    build_file = "//bzl/thirdpartybuild:png.BUILD",
    commit = "eddf9023206dc40974c26f589ee2ad63a4227a1e",
    remote = "https://github.com/glennrp/libpng.git",
    shallow_since = "1543674960 -0500",
)

new_git_repository(
    name = "draco",
    build_file = "//bzl/thirdpartybuild:draco.BUILD",
    commit = "bd1e8de7dd0596c2cbe5929cbe1f5d2257cd33db",
    remote = "https://github.com/google/draco.git",
    shallow_since = "1645135565 -0800",
)

http_archive(
    name = "org_freetype_freetype2",
    build_file = "//bzl/thirdpartybuild:freetype2.BUILD",
    sha256 = "e09aa914e4f7a5d723ac381420949c55c0b90b15744adce5d1406046022186ab",
    strip_prefix = "freetype-2.10.2",
    urls = [
        "http://download.savannah.gnu.org/releases/freetype/freetype-2.10.2.tar.gz",
        "https://sourceforge.net/projects/freetype/files/freetype2/2.10.2/freetype-2.10.2.tar.gz",
    ],
)

http_archive(
    name = "robolectric",
    build_file = "//bzl/thirdpartybuild:robolectric.BUILD",
    sha256 = "61802f241e410daf53994a8ea9c14777f7f6350b8a92b1461489fe0618ff41a1",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/roboelectric/robolectric-3.3.zip",
)

new_git_repository(
    name = "tinygltf",
    build_file = "//bzl/thirdpartybuild:tinygltf.BUILD",
    commit = "e0b393c6958c0a7cbe134a240fad7915aae53db3",
    remote = "https://github.com/syoyo/tinygltf.git",
    shallow_since = "1694518115 +0900",
)

new_git_repository(
    name = "androidmdnsresponder",
    build_file = "//bzl/thirdpartybuild:androidmdnsresponder.BUILD",
    commit = "85d33e8312069bc5b3d0e307589e0919922e687c",
    remote = "https://android.googlesource.com/platform/external/mdnsresponder",
    shallow_since = "1507706791 +0000",
)

http_archive(
    name = "mdnsresponder",
    build_file = "//bzl/thirdpartybuild:mdnsresponder.BUILD",
    patches = [
        "@the8thwall//third_party/mdnsresponder:dllmain.c.patch",
    ],
    sha256 = "75d66beadae8a64a5d30986afe45259b73b03bc6f4cb1b64acca7079cf11e056",
    strip_prefix = "mDNSResponder-mDNSResponder-765.50.9",
    url = "https://opensource.apple.com/tarballs/mDNSResponder/mDNSResponder-765.50.9.tar.gz",
)

new_git_repository(
    name = "opus",
    build_file = "//bzl/thirdpartybuild:opus.BUILD",
    commit = "d633f523e36e3b6d01cc6d57386458d770d618be",
    remote = "https://github.com/xiph/opus.git",
    shallow_since = "1611443214 -0800",
)

new_git_repository(
    name = "libwebm",
    build_file = "//bzl/thirdpartybuild:libwebm.BUILD",
    commit = "485fb67b324aec5298765e899dc054459d3946e5",
    remote = "https://chromium.googlesource.com/webm/libwebm",
    shallow_since = "1606261220 +0000",
)

new_git_repository(
    name = "libvpx",
    build_file = "//bzl/thirdpartybuild:libvpx.BUILD",
    commit = "0d8354669ac525a27c78bc8c761e98e0f8c3905c",
    patches = [
        "@the8thwall//third_party/libvpx:config.patch",
    ],
    remote = "https://chromium.googlesource.com/webm/libvpx",
    shallow_since = "1612419145 -0800",
)

new_git_repository(
    name = "openh264",
    build_file = "//bzl/thirdpartybuild:openh264.BUILD",
    commit = "22a46624f3b5c88e017ae5fb78f7a66e06bfccb9",
    remote = "https://github.com/8thwall/openh264",
    shallow_since = "1612314198 -0800",
)

new_git_repository(
    name = "libmp4v2",
    build_file = "//bzl/thirdpartybuild:libmp4v2.BUILD",
    commit = "b0e3019fb301362dedd376ba3f4fb7263b277c11",
    remote = "https://github.com/8thwall/libmp4v2",
    shallow_since = "1589395781 -0700",
)

new_git_repository(
    name = "fdkaac",
    build_file = "//bzl/thirdpartybuild:fdkaac.BUILD",
    commit = "573e93e4d0d08127dd3b2297a0ce52221527d90a",
    patches = [
        "@the8thwall//third_party/fdk-aac:FDK_archdef.h.patch",
        "@the8thwall//third_party/fdk-aac:FDK_lpp_tran.cpp.patch",
    ],
    remote = "https://github.com/mstorsjo/fdk-aac",
    shallow_since = "1629788404 +0300",
)

new_git_repository(
    name = "vlfeat",
    build_file = "//bzl/thirdpartybuild:vlfeat.BUILD",
    commit = "1b9075fc42fe54b42f0e937f8b9a230d8e2c7701",
    remote = "https://github.com/vlfeat/vlfeat",
    shallow_since = "1515711330 +0000",
)

git_repository(
    name = "fmt",
    commit = "a33701196adfad74917046096bf5a2aa0ab0bb50",
    patch_cmds = [
        "mv support/bazel/.bazelrc .bazelrc",
        "mv support/bazel/.bazelversion .bazelversion",
        "mv support/bazel/BUILD.bazel BUILD.bazel",
        "mv support/bazel/WORKSPACE.bazel WORKSPACE.bazel",
    ],
    # Windows-related patch commands are only needed in the case MSYS2 is not installed.
    # More details about the installation process of MSYS2 on Windows systems can be found here:
    # https://docs.bazel.build/versions/main/install-windows.html#installing-compilers-and-language-runtimes
    # Even if MSYS2 is installed the Windows related patch commands can still be used.
    patch_cmds_win = [
        "Move-Item -Path support/bazel/.bazelrc -Destination .bazelrc",
        "Move-Item -Path support/bazel/.bazelversion -Destination .bazelversion",
        "Move-Item -Path support/bazel/BUILD.bazel -Destination BUILD.bazel",
        "Move-Item -Path support/bazel/WORKSPACE.bazel -Destination WORKSPACE.bazel",
    ],
    remote = "https://github.com/fmtlib/fmt",
)

new_git_repository(
    name = "moodycamel",
    build_file = "//bzl/thirdpartybuild:concurrentqueue.BUILD",
    commit = "79cec4c3bf1ca23ea4a03adfcd3c2c3659684dd2",
    remote = "https://github.com/cameron314/concurrentqueue",
    shallow_since = "1580387311 -0500",
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

niantic_maybe(
    http_archive,
    name = "torch",
    build_file = "//bzl/thirdpartybuild:torch.BUILD",
    sha256 = "6b64703b360f06f85d36e2fffb1dd3659a14a3a15d78f43acfd6c9eea206bf46",
    strip_prefix = "libtorch",
    urls = [
        "https://download.pytorch.org/libtorch/cu118/libtorch-cxx11-abi-shared-with-deps-2.1.1%2Bcu118.zip",
    ],
)

# Downloaded from https://gitlab.com/<REMOVED_BEFORE_OPEN_SOURCING>/repos/legacy/niantic-ar/3rd-party/angle/-/tags
http_archive(
    name = "angle",
    sha256 = "42c4d922e2b78732c2145445907d72f6cf551a9a3ea378528ad3a09704e3b3a4",
    strip_prefix = "angle-nia-5943-p3",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/angle/angle-nia-5943-p3.tar.gz",
)

# Downloaded from https://developers.meta.com/horizon/downloads/package/oculus-platform-sdk
http_archive(
    name = "oculus-platform-sdk",
    build_file = "//bzl/thirdpartybuild:oculus-platform-sdk.BUILD",
    sha256 = "dee1b5b1e33fc4ac52358d2a83175f51d7f569212fd26d016ae851cbdc9a3468",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/ovr_platform_sdk/ovr_platform_sdk_69.0.zip",
)

# C++ wrapper classes for Node-API.
http_archive(
    name = "node-addon-api",
    build_file = "//bzl/thirdpartybuild:node-addon-api.BUILD",
    sha256 = "10223967fb13567b271639b530c6b13276bced48b57eea9d7c3e172f72cfee92",
    strip_prefix = "node-addon-api-7.0.0",
    url = "https://github.com/nodejs/node-addon-api/archive/refs/tags/v7.0.0.zip",
)

http_archive(
    name = "addon-tools-raub",
    build_file = "//bzl/thirdpartybuild:addon-tools-raub.BUILD",
    sha256 = "961159c82a24afc3347e2f593143394aa6d2b4be30f6f85c9bb742da03f4ce4c",
    strip_prefix = "addon-tools-raub-7.2.0",
    url = "https://github.com/node-3d/addon-tools-raub/archive/refs/tags/7.2.0.zip",
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
    name = "org_brotli",
    sha256 = "84a9a68ada813a59db94d83ea10c54155f1d34399baf377842ff3ab9b3b3256e",
    strip_prefix = "brotli-3914999fcc1fda92e750ef9190aa6db9bf7bdb07",
    url = "https://github.com/google/brotli/archive/3914999fcc1fda92e750ef9190aa6db9bf7bdb07.zip",
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

new_git_repository(
    name = "flecs",
    build_file = "//bzl/thirdpartybuild:flecs.BUILD",
    commit = "bf9ec9f8aecd987bee63e6c0af4164c7be14b42f",  # https://github.com/SanderMertens/flecs/releases/tag/v4.0.2
    remote = "https://github.com/SanderMertens/flecs.git",
    shallow_since = "1727187792 -0700",
)

http_archive(
    name = "spdlog",
    build_file = "//bzl/thirdpartybuild:spdlog.BUILD",
    patch_args = ["-p1"],
    patches = [
        "@the8thwall//:third_party/spdlog/public_logger.patch",
    ],
    sha256 = "4dccf2d10f410c1e2feaff89966bfc49a1abb29ef6f08246335b110e001e09a9",
    strip_prefix = "spdlog-1.12.0",
    url = "https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.tar.gz",
)

new_git_repository(
    name = "nanoflann",
    build_file = "//bzl/thirdpartybuild:nanoflann.BUILD",
    commit = "37b31cb554688a51a1f773420aa1b2c94c99237b",  # v1.5.3
    remote = "https://github.com/jlblancoc/nanoflann.git",
)

new_git_repository(
    name = "bullet3",
    build_file = "//bzl/thirdpartybuild:bullet3.BUILD",
    commit = "6bb8d1123d8a55d407b19fd3357c724d0f5c9d3c",
    remote = "https://github.com/bulletphysics/bullet3.git",
    shallow_since = "1701210212 -0800",
)

new_git_repository(
    name = "joltphysics",
    build_file = "//bzl/thirdpartybuild:joltphysics.BUILD",
    commit = "0373ec0dd762e4bc2f6acdb08371ee84fa23c6db",  #  v5.3.0
    remote = "https://github.com/jrouwe/JoltPhysics.git",
    shallow_since = "1742071981 +0100",
)

new_git_repository(
    name = "sophus",
    build_file = "//bzl/thirdpartybuild:sophus.BUILD",
    commit = "de0f8d3d92bf776271e16de56d1803940ebccab9",  # 1.22.10
    remote = "https://github.com/strasdat/Sophus.git",
    shallow_since = "1675598033 -0800",
)

new_git_repository(
    name = "webgl-conformance",
    build_file = "//bzl/thirdpartybuild:webgl-conformance.BUILD",
    commit = "8a1bf5671d342458bc258ad8a575ad269292c361",  # 7/10/2024
    remote = "https://github.com/KhronosGroup/WebGL.git",
)

new_git_repository(
    name = "openxr",
    build_file = "//bzl/thirdpartybuild:openxr.BUILD",
    commit = "f90488c4fb1537f4256d09d4a4d3ad5543ebaf24",
    remote = "https://github.com/KhronosGroup/OpenXR-SDK.git",
    shallow_since = "1718057169 -0500",
)

http_file(
    name = "android-manifest-merger",
    sha256 = "4e06d2f8b545741847495f4cfdd14eb8ccf42eb21241e8be0387759ce074c415",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/android-manifest-merger/android-manifest-merger-31.5.2.jar",
)

AAPT2_VERSION = "8.10.0-rc02-12782657"

http_file(
    name = "android-aapt2-linux-jar",
    sha256 = "8a6e4d59a2e51c117b4a54ac2853f6f62c7effa4c54bbbc5cdf531ba215e909e",
    url = "https://dl.google.com/android/maven2/com/android/tools/build/aapt2/{}/aapt2-{}-linux.jar".format(
        AAPT2_VERSION,
        AAPT2_VERSION,
    ),
)

# TODO(lreyna): Bazelify the deps from https://github.com/indygreg/apple-platform-rs, so we can
# build the `rcodesign` binary ourselves.
http_archive(
    name = "apple-codesign-linux-x86_64",
    build_file_content = """
filegroup(
    name = "rcodesign_bin",
    srcs = ["rcodesign"],
    visibility = ["//visibility:public"],
)
""",
    sha256 = "dbe85cedd8ee4217b64e9a0e4c2aef92ab8bcaaa41f20bde99781ff02e600002",
    strip_prefix = "apple-codesign-0.29.0-x86_64-unknown-linux-musl",
    url = "https://github.com/indygreg/apple-platform-rs/releases/download/apple-codesign%2F0.29.0/apple-codesign-0.29.0-x86_64-unknown-linux-musl.tar.gz",
)

http_archive(
    name = "apple-codesign-osx-arm64",
    build_file_content = """
filegroup(
    name = "rcodesign_bin",
    srcs = ["rcodesign"],
    visibility = ["//visibility:public"],
)
""",
    sha256 = "d1a532150adaf90048260d76359261aa716abafc45c53c5dc18845029184334a",
    strip_prefix = "apple-codesign-0.29.0-aarch64-apple-darwin",
    url = "https://github.com/indygreg/apple-platform-rs/releases/download/apple-codesign%2F0.29.0/apple-codesign-0.29.0-aarch64-apple-darwin.tar.gz",
)

http_archive(
    name = "apple-codesign-osx-x86_64",
    build_file_content = """
filegroup(
    name = "rcodesign_bin",
    srcs = ["rcodesign"],
    visibility = ["//visibility:public"],
)
""",
    sha256 = "14ef11bedd51a8d95eafd767939ae96d5900e5a61511bef75bb21db6e7c74140",
    strip_prefix = "apple-codesign-0.29.0-x86_64-apple-darwin",
    url = "https://github.com/indygreg/apple-platform-rs/releases/download/apple-codesign%2F0.29.0/apple-codesign-0.29.0-x86_64-apple-darwin.tar.gz",
)

npm_package(
    name = "npm-tauri-app",
    package = "//apps/client/exploratory/tauri-app:package.json",
    package_lock = "//apps/client/exploratory/tauri-app:package-lock.json",
)

http_archive(
    name = "itms-transporter-macosx-universal",
    build_file = "//bzl/thirdpartybuild:itms-transporter.BUILD",
    sha256 = "719b9152088f56d5bcb7b1bb61238a19e20823968e12ce33b084e9d25be43b7c",
    strip_prefix = "itms-transporter-4.1.0-macosx-universal",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/itms-transporter/itms-transporter-4.1.0-macosx-universal.tgz",
)

http_archive(
    name = "itms-transporter-linux-x86_64",
    build_file = "//bzl/thirdpartybuild:itms-transporter.BUILD",
    sha256 = "559edac54cc020a7ef13d49f7034b942a4d4c7da93582c81eb0d7b8aec5de52a",
    strip_prefix = "itms-transporter-4.1.0-linux-x86_64",
    url = "https://huggingface.co/datasets/8thWall/bazel-dependencies/resolve/main/itms-transporter/itms-transporter-4.1.0-linux-x86_64.tgz",
)
