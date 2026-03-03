licenses(["permissive"])  # Apache 2.0

cc_library(
    name = "spirv-cross-lib",
    srcs = glob([
        "spirv_*.cpp",
    ]),
    hdrs = [
        "GLSL.std.450.h",
        "spirv.h",
        "spirv.hpp",
        "spirv_cfg.hpp",
        "spirv_common.hpp",
        "spirv_cpp.hpp",
        "spirv_cross.hpp",
        "spirv_cross_c.h",
        "spirv_cross_containers.hpp",
        "spirv_cross_error_handling.hpp",
        "spirv_cross_parsed_ir.hpp",
        "spirv_cross_util.hpp",
        "spirv_glsl.hpp",
        "spirv_hlsl.hpp",
        "spirv_msl.hpp",
        "spirv_parser.hpp",
        "spirv_reflect.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "spirv-cross",
    srcs = [
        "main.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":spirv-cross-lib",
    ],
)
