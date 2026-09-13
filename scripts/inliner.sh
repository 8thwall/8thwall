#!/bin/bash
set -e

build_files=$(
  git ls-tree -r --name-only HEAD | \
  grep "/BUILD$"
)

bazel build //bzl/inlinerjs --noshow_progress > /dev/null
echo "$build_files" | xargs ./bazel-bin/bzl/inlinerjs/inlinerjs --no-new

cc_files=$(
  git ls-tree -r --name-only HEAD | \
  grep "\.cc$"
)

bazel build //bzl/inliner --noshow_progress > /dev/null
echo "$cc_files" | sort | xargs -n 60 -P 10 ./bazel-bin/bzl/inliner/inliner > /dev/null

if type buildifier &> /dev/null; then
  if buildifier --version | grep "5.1.0"; then
    echo "$build_files" | xargs buildifier -lint=fix
  else 
    echo "Expected buildifier --version to be 5.1.0"
    exit 1
  fi
else
  echo "buildifier not found, skipped formatting."
fi
