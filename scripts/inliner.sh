#!/bin/bash
set -e

build_files=$(
  git ls-tree -r --name-only HEAD | \
  grep "/BUILD$"
)

echo "$build_files" | xargs bazel run --run_under="cd $PWD && " //bzl/inlinerjs -- --no-new

if type buildifier &> /dev/null; then
  if buildifier --version | grep "5.1.0"; then
    echo "Expected buildifier --version to be 5.1.0"
    exit 1
  fi

  echo "$build_files" | xargs buildifier -lint=fix
else
  echo "buildifier not found, skipped formatting."
fi
