load("//bzl/utils:aliased-file.bzl", "aliased_file")
load("@rules_pkg//pkg:mappings.bzl", "pkg_files")

pkg_files(
    name = "resources",
    srcs = [
        "dom-tablet-button.glb",
        "dom-tablet-frame.glb",
        "face-ear-model.tflite",
        "face-mesh-model.tflite",
        "face-model.tflite",
        "powered-by.svg",
        "semantics-model.tflite",
        ":media-recorder-artifact",
        ":semantics-worker-artifact",
    ],
    prefix = "resources",
    visibility = ["//visibility:public"],
)

aliased_file(
    name = "semantics-worker-artifact",
    filename = "semantics-worker.js",
    source_target = "//reality/app/xr/js/src/semantics:semantics-worker",
)

aliased_file(
    name = "media-recorder-artifact",
    filename = "media-worker.js",
    source_filename = "media-recorder-js-asm.js",
    source_target = "//c8/media:media-recorder-js-asm",
)
