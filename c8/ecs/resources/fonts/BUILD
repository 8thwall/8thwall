load("@rules_pkg//pkg:mappings.bzl", "pkg_filegroup")
load(":font.bzl", "font")

package(default_visibility = ["//c8/ecs:__subpackages__"])

pkg_filegroup(
    name = "fonts",
    srcs = [
        ":baskerville",
        ":futura",
        ":inconsolata",
        ":nanum_pen_script",
        ":nunito",
        ":press_start_2p",
        ":roboto",
    ],
    prefix = "fonts",
)

font(
    name = "baskerville",
)

font(
    name = "futura",
)

font(
    name = "inconsolata",
)

font(
    name = "nanum_pen_script",
)

font(
    name = "nunito",
)

font(
    name = "press_start_2p",
)

font(
    name = "roboto",
)
