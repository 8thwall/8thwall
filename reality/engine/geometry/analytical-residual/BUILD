load("@pip-deps//:requirements.bzl", "requirement")

cc_library(
    name = "pose-estimation",
    srcs = [
        ":pose-estimation-analytical-cc",
    ],
    hdrs = [
        "pose-estimation.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//reality/engine/geometry:observed-point",
        "@ceres",
    ],
)

py_binary(
    name = "generate-pose-estimation",
    srcs = [
        "generate-pose-estimation.py",
        "memoizer.py",
        "rotation.py",
    ],
    imports = ["."],
    main = "generate-pose-estimation.py",
    python_version = "PY3",
    deps = [
        # Available pip deps are listed in //bzl/python/requirements.txt.
        requirement("sympy"),
    ],
)

genrule(
    name = "pose-estimation-analytical-cc",
    outs = [
        "pose-estimation.cc",
    ],
    cmd = "python3 $(location :generate-pose-estimation) $(@D)/pose-estimation.cc",
    tools = [":generate-pose-estimation"],
)
