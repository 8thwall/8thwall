licenses(["permissive"])  # Simplified BSD

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "concurrentqueue",
    hdrs = [
        "blockingconcurrentqueue.h",
        "concurrentqueue.h",
        "lightweightsemaphore.h",
    ],
    copts = [
    ],
    include_prefix = "moodycamel",
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "moody-test",
    srcs = [
        "tests/fuzztests/fuzztests.cpp",
        "tests/corealgos.h",
    ] + glob([
        "tests/common/*.h",
        "tests/common/*.cpp",
    ]),
    deps = [
        ":concurrentqueue",
    ],
)
