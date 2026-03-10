licenses(["permissive"])  # 3-clause BSD

cc_library(
    name = "arc4random",
    hdrs = [
        "arc4random.c",  # Despite the name, this is treated as a header.
    ],
    visibility = [
        "@libevent//:__pkg__",
    ],
)

cc_library(
    name = "platform-specific-headers",
    hdrs = select({
        "//bzl/conditions:android32": [
            "android/include32/event2/event-config.h",
            "android/include32/evconfig-private.h",
        ],
        "//bzl/conditions:android64": [
            "android/include64/event2/event-config.h",
            "android/include64/evconfig-private.h",
        ],
        "//bzl/conditions:apple32": [
            "apple/include32/event2/event-config.h",
            "apple/include32/evconfig-private.h",
        ],
        "//bzl/conditions:apple64": [
            "apple/include64/event2/event-config.h",
            "apple/include64/evconfig-private.h",
        ],
        "//bzl/conditions:windows64": [
            "windows/include64/event2/event-config.h",
            "windows/include64/evconfig-private.h",
        ],
        "//conditions:default": [],
    }),
    includes = ["."] + select({
        "//bzl/conditions:android32": ["android/include32"],
        "//bzl/conditions:android64": ["android/include64"],
        "//bzl/conditions:apple32": ["apple/include32"],
        "//bzl/conditions:apple64": ["apple/include64"],
        "//bzl/conditions:windows64": ["windows/include64"],
        "//conditions:default": [],
    }),
    visibility = [
        "@libevent//:__pkg__",
    ],
)
