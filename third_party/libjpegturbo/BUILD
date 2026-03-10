licenses(["permissive"])

exports_files(["turbojpeg.c.patch"])

filegroup(
    name = "jconfig",
    srcs = [
        "jconfig.h",
        "jconfigint.h",
        "jversion.h",
    ],
    visibility = [
        "@libjpegturbo//:__pkg__",
    ],
)
