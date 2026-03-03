licenses(["permissive"])  # BSD 3-clause

cc_library(
    name = "libwebm",
    srcs = [
        "common/file_util.cc",
        "common/hdr_util.cc",
        "common/indent.cc",
        "common/libwebm_util.cc",
        "common/video_frame.cc",
        "common/vp9_header_parser.cc",
        "common/vp9_level_stats.cc",
        "common/webm_endian.cc",
        "mkvmuxer/mkvmuxer.cc",
        "mkvmuxer/mkvmuxerutil.cc",
        "mkvmuxer/mkvwriter.cc",
        "mkvparser/mkvparser.cc",
        "mkvparser/mkvreader.cc",
    ],
    hdrs = [
        "common/file_util.h",
        "common/hdr_util.h",
        "common/indent.h",
        "common/libwebm_util.h",
        "common/video_frame.h",
        "common/vp9_header_parser.h",
        "common/vp9_level_stats.h",
        "common/webm_constants.h",
        "common/webm_endian.h",
        "common/webmids.h",
        "mkvmuxer/mkvmuxer.h",
        "mkvmuxer/mkvmuxertypes.h",
        "mkvmuxer/mkvmuxerutil.h",
        "mkvmuxer/mkvwriter.h",
        "mkvparser/mkvparser.h",
        "mkvparser/mkvreader.h",
    ],
    copts = [
        "-Iexternal/libwebm",
        "-std=c++17",
    ],
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
)

cc_library(
    name = "webm-parser",
    srcs = glob([
        "webm_parser/src/*.h",
        "webm_parser/src/*.cc",
    ]),
    hdrs = glob([
        "webm_parser/include/webm/*.h",
    ]),
    copts = [
        "-Iexternal/libwebm/webm_parser",
    ],
    includes = [
        "webm_parser/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
)
