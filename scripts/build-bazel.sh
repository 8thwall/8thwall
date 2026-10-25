#!/bin/bash --norc
set -u

status=0

run_bazel() {
  if ! bazel "$@"; then
    status=1
  fi
}

run_bazel build --keep_going //reality/app/xr/js:serve-xr --config=wasmrelease
run_bazel test --keep_going //c8/ecs/... --config=node
run_bazel test --keep_going --test_output=errors \
  -- \
  //... \
  -//bzl/... \
  //bzl/examples/... \
  -//bzl/examples/android-kotlin/... \
  -//bzl/examples/android/... \
  -//bzl/examples/grpc/... \
  -//bzl/examples/proto/... \
  -//bzl/examples/pybind11/...

if [ "$status" -ne 0 ]; then
  echo "One or more Bazel target groups failed." >&2
  exit "$status"
fi

echo "All Bazel Builds Passed!"
