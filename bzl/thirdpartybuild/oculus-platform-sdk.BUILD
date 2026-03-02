# ovr_platform_sdk_x.y
# ├── Windows/
# ├── Samples/
# ├── Include/
# └── Android/

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ovr_platform_sdk_android",
    srcs = ["Android/libs/arm64-v8a/libovrplatformloader.so"],
    hdrs = glob([
        "Include/**/*.h",
    ]),
    includes = ["Include"],
)
