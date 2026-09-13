#!/bin/bash --norc
set -eu

bazel build //reality/app/xr/js:serve-xr --config=wasmrelease
bazel test //c8/ecs/... --config=node
bazel test --keep_going=0  --test_output=errors \
  -- \
  //... \
  -//bzl/... \
  //bzl/examples/... \
  -//bzl/examples/android-kotlin/... \
  -//bzl/examples/android/... \
  -//bzl/examples/grpc/... \
  -//bzl/examples/proto/... \
  -//bzl/examples/pybind11/...

echo "All Bazel Builds Passed!"
