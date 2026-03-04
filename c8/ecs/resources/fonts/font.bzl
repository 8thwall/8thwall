"""Define a font packaging rule."""

load("@rules_pkg//pkg:mappings.bzl", "pkg_files")

def font(name):
    pkg_files(
        name = name,
        srcs = [
            name + "/" + name + ".png",
            name + "/" + name + ".ttf",
            name + "/" + name + ".json",
            name + "/" + "LICENSE",
        ],
        prefix = name,
    )
