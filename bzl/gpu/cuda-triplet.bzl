"""module
This is a module to configure cude-triplet for compling cuda code.
A Cuda triplet is formed by 3 components
- cuda toolkit
- cudnn library
- nccl library
and usually the layout is the following:
```
cuda-triplet
├── cuda -> cuda_11.2.2
├── cuda_11.2.2
├── cudnn -> cudnn-linux-x86_64-8.7.0.84_cuda11-archive
├── cudnn-linux-x86_64-8.7.0.84_cuda11-archive
├── nccl -> nccl_2.16.5-1+cuda11.0_x86_64
└── nccl_2.16.5-1+cuda11.0_x86_64
```

The behaviour of this repository can be configured via --//bzl/gpu:cuda-support
This can have three different values:
- "none" - (default) no cuda triple is actually fetched or made ready for use. Any attempt to
                     use the load("//bzl/gpu:cuda.bzl", "cuda_library") macro will fail
                     with a dedicated message suggesting to use either "hermetic" or "system".
- "hermetic" - This configuration will download a cuda-triplet that allow to compile in all
               the host system for any of the v*-linux targets
- "system" - The configuration will link to the sandbox all system libraries and header related to
             the triplet. This assumes that those libraries are installed using the layout of
             the nvidia/cuda:*.*.*-cudnn*-devel-{distro} docker images available
             at https://hub.docker.com/r/nvidia/cuda/tags which is the default system
             installation layout

IMPORTANT!! Every time a different --//bzl/gpu:cuda-support is needed, it is necessary to call
`bazel clean` because by bazel does update repository contexts and it will use the previous layout,
unless you either edit file or you run `bazel clean`.
"""

def _execute(repository_ctx, command, environment = {}, quiet = True, error_message = None):
    """Execute a command and return stdout, or throw an error if it fails."""
    result = repository_ctx.execute(command, environment = environment, quiet = quiet)
    if result.return_code != 0:
        if error_message:
            fail(error_message)
        else:
            fail(result.stderr)
    else:
        return result.stdout.strip()

def _gently_print(repository_ctx, message, environment = {}):
    _execute(
        repository_ctx,
        ["echo", "\033[0;32mINFO: cuda-triplet.bzl - {}\033[0m".format(message)],
        environment = environment,
        quiet = False,
    )

def _cuda_triplet_impl(repository_ctx):
    """Implementation for the cuda_triplet repository rule. Currently only available for Windows
    """

    if "windows" in repository_ctx.os.name:
        _gently_print(repository_ctx, "cuda_triplet is only available for Linux at the moment")
        repository_ctx.file(
            "BUILD",
            content = """# dummy
          """,
            executable = False,
        )
        repository_ctx.file(
            "defines.bzl",
            content = """
# Definitions for {name} cuda_triplet.
CUDA_TRIPLET_ROOT = "NA"
CUDA_TRIPLET_ID = "NA"
          """.format(name = repository_ctx.name),
            executable = False,
        )
        return

    download_cuda = False
    symlink_system_cuda = False

    # On 18th June 2024 Aron (aron@) updated the second row from
    # `ps a | grep bazel` to
    # `ps au | grep '--platforms=//'`.
    # This is for CLion's Bazel plugin to be able to detect the bazel build args correctly.
    #
    # On 8th Nov 2024 Aron (aron@) added `x` to `ps au` to allow JetBrains Gateway to find
    # the remote bazel process.
    repository_ctx.file(
        "get_bazel_invocation.sh",
        content = """#!/bin/bash
export BAZEL_PS=$(ps aux | grep '\\-\\-platforms=//')
echo ${BAZEL_PS##*bazel}
""",
        executable = True,
    )

    cuda_triplet_dir = _execute(repository_ctx, ["pwd"])

    bazel_args = _execute(
        repository_ctx = repository_ctx,
        command = ["{}/{}".format(
            cuda_triplet_dir,
            "get_bazel_invocation.sh",
        )],
    )

    for arg in bazel_args.split(" "):
        if "cuda" in arg and "hermetic" in arg:
            download_cuda = True
            break
        if "cuda" in arg and "system" in arg:
            symlink_system_cuda = True
            break

    if symlink_system_cuda and download_cuda:
        fail("You cannot use at the same time both the system cuda toolkit and the hermetic one. " +
             "Please use one --//bzl/gpu:cuda-support= value")

    if download_cuda:
        repository_ctx.download_and_extract(
            repository_ctx.attr.url,
            sha256 = repository_ctx.attr.sha256,
            stripPrefix = repository_ctx.attr.strip_prefix,
        )

    if symlink_system_cuda:
        repository_ctx.file(
            "symlink_system_cuda.sh",
            content = """#!/bin/bash
# Clean previous versions
rm -f cuda
rm -rf cudnn
rm -rf nccl
# Creating directories
mkdir -p cudnn/include
mkdir -p cudnn/lib
mkdir -p nccl/include
mkdir -p nccl/lib
# CUDA
ln -s /usr/local/cuda cuda
# CUDNN
ls -1 /usr/lib/x86_64-linux-gnu/libcudnn* | while read i; do ln -s $i cudnn/lib/$(basename "$i"); done
ls -1 /usr/include/x86_64-linux-gnu/cudnn* | while read i; do ln -s $i cudnn/include/$(basename "$i"); done
ls -1 /usr/include/cudnn* | while read i; do ln -s $i cudnn/include/$(basename "$i"); done
# NCCL
ls -1 /usr/lib/x86_64-linux-gnu/libnccl* | while read i; do ln -s $i nccl/lib/$(basename "$i"); done
ls -1 /usr/include/nccl* | while read i; do ln -s $i nccl/include/$(basename "$i"); done
""",
            executable = True,
        )

        _execute(
            repository_ctx = repository_ctx,
            command = ["{}/{}".format(
                cuda_triplet_dir,
                "symlink_system_cuda.sh",
            )],
        )

    if not repository_ctx.path(cuda_triplet_dir + "/cuda/targets").exists:
        _gently_print(
            repository_ctx,
            "Cuda was not configured. " +
            "If you need it, please run a 'bazel clean' first and then use either " +
            "'--//bzl/gpu:cuda-support=hermetic' or " +
            "'--//bzl/gpu:cuda-support=system' arguments. " +
            "If you used `system` and you still see this message it might be because " +
            "you don't have cuda in this host system.",
        )

    repository_ctx.file(
        "defines.bzl",
        content = """# Definitions for {name} cuda_triplet.
CUDA_TRIPLET_ROOT = "{cuda_triplet_dir}"
CUDA_TRIPLET_ID = "{cuda_triplet_id}"
""".format(
            name = repository_ctx.name,
            cuda_triplet_dir = cuda_triplet_dir,
            cuda_triplet_id = repository_ctx.name,
        ),
        executable = False,
    )

    if repository_ctx.attr.build_file:
        repository_ctx.template("BUILD", repository_ctx.attr.build_file, executable = False)
    else:
        repository_ctx.file(
            "BUILD",
            content = """
# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "cuda_triplet",
    srcs = [],
)
""",
            executable = False,
        )

# Creates wrapper script to a tool installed locally and available in the
# default shell path. This should ideally be used only for runfiles, as using
# for compile-time tools will affect remote caching.
cuda_triplet = repository_rule(
    implementation = _cuda_triplet_impl,
    attrs = {
        "url": attr.string(),
        "sha256": attr.string(),
        "strip_prefix": attr.string(),
        "build_file": attr.label(allow_single_file = True),
    },
    local = True,
)
