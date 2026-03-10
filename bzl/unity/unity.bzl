load("//bzl/unity/impl:unity-app.bzl", _unity_app = "unity_app")
load("//bzl/unity/impl:unity-csharp-dll.bzl", _csharp_library = "csharp_library", _unity_csharp_dll = "unity_csharp_dll")
load("//bzl/unity/impl:unity-package.bzl", _unity_package = "unity_package")
load("//bzl/unity/impl:unity-plugin.bzl", _unity_plugin = "unity_plugin")
load("//bzl/unity/impl:unity-project.bzl", _unity_project = "unity_project")
load("//bzl/unity/impl:unity-upm-package.bzl", _unity_upm_package = "unity_upm_package")
load("//bzl/unity/impl:unity-integration-test.bzl", _unity_integration_test = "unity_integration_test")
load("//bzl/unity/impl:export-manifest.bzl", _export_manifest = "export_manifest")

csharp_library = _csharp_library
unity_app = _unity_app
unity_csharp_dll = _unity_csharp_dll
unity_package = _unity_package
unity_project = _unity_project
unity_plugin = _unity_plugin
unity_upm_package = _unity_upm_package
unity_integration_test = _unity_integration_test
export_manifest = _export_manifest

PLATFORM_ADDON_STRING = select({
    "@platforms//os:android": ".android",
    "@platforms//os:ios": ".ios",
    "@platforms//os:linux": ".linux",
    "@platforms//os:osx": ".osx",
    "@platforms//os:windows": ".windows",
    "//conditions:default": "",
})

ARCH_ADDON_STRING = select({
    "@platforms//cpu:arm": ".armv7a",
    "@platforms//cpu:arm64": ".arm64",
    "@platforms//cpu:wasm32": ".wasm32",
    "@platforms//cpu:x86_32": ".x86_32",
    "@platforms//cpu:x86_64": ".x86_64",
    "//conditions:default": "",
})
