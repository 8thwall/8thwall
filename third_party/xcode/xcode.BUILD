# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.

package(default_visibility = ["//visibility:public"])

# NOTE(paris): Take care not to add many files to this stub or else caching will take a long time.
filegroup(
  name = "stub",
  srcs = [],
)

filegroup(
  name = "all-files",
  srcs = glob(["**/*"]),
)
