load("//bzl/utils:embedded.bzl", "embedded_data")

licenses(["permissive"])

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "face-detection-front",
    srcs = [
        "face_detection_front.tflite",
    ],
)

embedded_data(
    name = "face-detection-front-embedded",
    srcs = [
        ":face-detection-front",
    ],
)

filegroup(
    name = "face-landmark",
    srcs = [
        "face_landmark.tflite",
    ],
)

embedded_data(
    name = "face-landmark-embedded",
    srcs = [
        ":face-landmark",
    ],
)

filegroup(
    name = "face-landmark-attention",
    srcs = [
        "face_landmark_with_attention.tflite",
    ],
)

embedded_data(
    name = "face-landmark-attention-embedded",
    srcs = [
        ":face-landmark-attention",
    ],
)

# NOTE(paris): Removed before open sourcing.
# filegroup(
#     name = "hand-landmark",
#     srcs = [
#         "hand_landmark_lite.tflite",
#     ],
# )
#
# embedded_data(
#     name = "hand-landmark-embedded",
#     srcs = [
#         ":hand-landmark",
#     ],
# )

filegroup(
    name = "iris-landmark",
    srcs = [
        "iris_landmark.tflite",
    ],
)

embedded_data(
    name = "iris-landmark-embedded",
    srcs = [
        ":iris-landmark",
    ],
)

# NOTE(paris): Removed before open sourcing.
# filegroup(
#     name = "palm-detection",
#     srcs = [
#         "palm_detection_lite.tflite",
#     ],
# )
#
# embedded_data(
#     name = "palm-detection-embedded",
#     srcs = [
#         ":palm-detection",
#     ],
# )
