licenses(["permissive"])  # Apache 2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "common",
    srcs = [
        "addrcache.cc",
        "addrcache.h",
        "checksum.h",
        "codetable.cc",
        "codetable.h",
        "compile_assert.h",
        "google/format_extension_flags.h",
        "google/output_string.h",
        "headerparser.h",
        "instruction_map.h",
        "logging.cc",
        "logging.h",
        "output_string_crope.h",
        "rolling_hash.h",
        "unique_ptr.h",
        "varint_bigendian.cc",
        "varint_bigendian.h",
        "vcdiff_defs.h",
        "vcdiffengine.h",
    ],
    hdrs = [
        "google/codetablewriter_interface.h",
        "google/jsonwriter.h",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "//external:gflags",
        "@the8thwall//third_party/open-vcdiff:config",
        "@zlib",
    ],
)

cc_library(
    name = "encoder",
    srcs = [
        "blockhash.cc",
        "blockhash.h",
        "encodetable.cc",
        "instruction_map.cc",
        "jsonwriter.cc",
        "vcdiffengine.cc",
        "vcencoder.cc",
    ],
    hdrs = [
        "google/encodetable.h",
        "google/vcencoder.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":common",
    ],
)

cc_library(
    name = "decoder",
    srcs = [
        "decodetable.cc",
        "decodetable.h",
        "headerparser.cc",
        "vcdecoder.cc",
    ],
    hdrs = [
        "google/vcdecoder.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":common",
    ],
)
