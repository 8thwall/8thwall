#!/bin/bash --norc
set -eu

ROOT="$(realpath "$(dirname "$0")/..")"

bazel build //reality/app/xr/js:serve-xr --config=wasmrelease
bazel test //c8/ecs/... --config=node
bazel test --keep_going  --test_output=errors \
  -- \
  //apps/... \
  //c8/...  \
  //reality/... \

echo "Bazel Tests Passed!"
