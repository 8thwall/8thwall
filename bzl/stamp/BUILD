genrule(
    name = "release-build-id-unstamped",
    outs = ["release-build-id-unstamped.version"],
    cmd = "echo 0.0.0.0 >> $@",
    visibility = ["//visibility:private"],
)

genrule(
    name = "release-build-id-stamped",
    srcs = [],
    outs = ["release-build-id-stamped.version"],
    cmd = "cat bazel-out/stable-status.txt | grep STABLE_C8_RELEASE_BUILD_ID | awk '{print $$2}' >> $@",
    stamp = True,
    visibility = ["//visibility:private"],
)

config_setting(
    name = "release",
    values = {"action_env": "C8VERSION=true"},
    visibility = ["//visibility:private"],
)

filegroup(
    name = "release-build-id-dynamic",
    srcs = select({
        ":release": [":release-build-id-stamped"],
        "//conditions:default": [":release-build-id-unstamped"],
    }),
    visibility = ["//visibility:public"],
)

genrule(
    name = "gen-release-build-id",
    srcs = [":release-build-id-dynamic"],
    outs = ["release-build-id.cc"],
    cmd = "cat $< | sed 's/\\(.*\\)/\\/\\/ C8VERSION \\1\\\n#include \"bzl\\/stamp\\/release-build-id.h\"\\\n\\\nconst char* c8::ReleaseBuildId() { return \"\\1\"; }/' >> $@",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "release-build-id",
    srcs = [
        ":gen-release-build-id",
    ],
    hdrs = [
        "release-build-id.h",
    ],
    visibility = ["//c8:__pkg__"],
)
