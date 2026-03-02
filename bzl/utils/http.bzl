load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Downloads an archive from url and provides a default build filegroup to reference
# the root directory to be used in tests. Note this can only be used on host machine
# tests, will not work on device. Use embedded_data instead.
#
# Add root path to cc_test using below for each archive
#
#   data = ["@my_package_name//:root"],
#   args = ["$(location @my_package_name//:root)"],
#
# Reference path in the test using command line arguments in the order of placement
# in args attribute passed index 1 (index 0 is the binary test executable).
#
# Example:
#
#   WORKSPACE:
#
#   load("//bzl/utils:http.bzl", "http_files")
#
#   http_files(
#     name = "my_package_name",
#     url = "https://website.com/package.zip",
#     sha256 = "fb4b57d5da16523a5d5e5b4849091ee19d4cfe809e013adfede73bcc1c21c16d",
#  )
#
#  BUILD:
#
#  cc_test(
#     name = "my_test
#     data = [ "@my_package_name//:root" ]
#     args = ["$(location @my_package_name//:root)"],
#     ...
#  )
#
#   source_test.cc:
#
#   const std::string file = std::string(gtest_argv[1]) + "/path/to/file.bin"
#
def http_files(
        name,
        build_file_content = """
filegroup(
    name = "root",
    srcs = ["."],
    visibility = ["//visibility:public"],
)

filegroup(
  name = "all-files",
  srcs = glob(["**/*"]),
  visibility = ["//visibility:public"],
)""",
        **kwargs):
    http_archive(
        name = name,
        build_file_content = build_file_content,
        **kwargs
    )
