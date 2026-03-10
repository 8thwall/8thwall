package(default_visibility = ["//visibility:public"])

cc_library(
    name = "torch",
    srcs = glob(
        [
            "lib/*.so*",
        ],
        exclude = [
            "lib/libnnapi_backend.so",
            "lib/libtorch_python.so",
            "lib/libnvrtc-builtins-4730a239.so.11.3",
            # TODO(peter): Omit libcaffe2_nvrtc.so for now. It is unused for AR Mapping,
            # and introduces a transitive dependency on libcuda.so.
            #
            # This dependency is undesired, otherwise dynamic linking errors occur
            # when mapping algorithms (that don't require GPU resources to execute)
            # are deployed to non-GPU machines.
            #
            # libcuda.so is provided by the host machine's Nvidia driver through the
            # container engine.
            "lib/libcaffe2_nvrtc.so",

            # NOTE: These libs need to be present, because the torch .so ELF files
            # expect these exact names to be exist on dynamic linking at the start
            # of execution.
            #
            # If/once torch is built in the @niantic WORKSPACE from source, we can
            # remove these bundled CUDA shared libs and replace them with the same
            # libs provided by v1-cuda-triplet
            #
            # "lib/libcudart-a7b20f20.so.11.0",
            # "lib/libnvToolsExt-24de1d56.so.1",
            # "lib/libnvrtc-1ea278b5.so.11.2",
        ],
    ),
    hdrs = glob([
        "include/*.h",
        "include/*.hpp",
        "include/**/*.h",
        "include/**/*.hpp",
    ]),
    includes = [
        "include",
        "include/torch/csrc/api/include",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "@the8thwall//bzl/crosstool:v1-linux",
        "@the8thwall//bzl/crosstool:gpu-nvidia",
    ],
)
