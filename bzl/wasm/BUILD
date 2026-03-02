load("@bazel_skylib//rules:common_settings.bzl", "string_flag", "string_list_flag")

# Specify the wasm platforms to use in builds that have both native and wasm compilation.
string_list_flag(
    name = "wasm-platforms",
    build_setting_default = [
        "//bzl:wasm32",
    ],
    visibility = ["//visibility:public"],
)

# These flags are applied to implicit WebAssembly compilation, those created by
# building a cc_binary or cc_test rule with --config=wasm. You can modify them
# for a specific build with repeated use of the flag --//bzl/wasm:flag=-sMY_FLAG.
string_list_flag(
    name = "flag",
    build_setting_default = [
        "-sEXIT_RUNTIME",
        "-sMAXIMUM_MEMORY=1024mb",
        "-sALLOW_MEMORY_GROWTH=1",
        "-sFETCH=1",
        "--bind",
    ],
    visibility = ["//visibility:public"],
)

# This flag is used when building with --features=emrun. It will set the
# --browser flag of the emrun wrapper script to execute within.
string_flag(
    name = "browser",
    build_setting_default = "chrome",
    visibility = ["//visibility:public"],
)
