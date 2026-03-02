"""
Rust helpers.
"""

rust_supported_platforms = select({
    "//bzl/conditions:local_platform": [],
    "//bzl/conditions:android-arm64": [],
    "//bzl/conditions:ios-arm64": [],
    "//bzl/conditions:linux": [],
    "//conditions:default": ["@platforms//:incompatible"],
})
