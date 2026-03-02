licenses(["permissive"])  # BSD 3-clause

cc_library(
    name = "cli11",
    srcs = glob(
        [
            "include/CLI/*.hpp",
            "include/CLI/impl/*.hpp",
        ],
        exclude = [
            "include/CLI/CLI.hpp",
        ],
    ),
    hdrs = [
        "include/CLI/CLI.hpp",
    ],
    copts = [
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
)
