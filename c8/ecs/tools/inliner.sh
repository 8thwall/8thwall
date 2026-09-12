#!/bin/bash
set -e

cd $(bazel info workspace 2>/dev/null)

build_files=$(find c8/ecs -path c8/ecs/node_modules -prune -o -name "BUILD" -print)

echo "$build_files" | xargs bazel run --run_under="cd $PWD && " //bzl/inlinerjs

if [[ "$@" == *"--cc"* ]]; then
  cc_files=$(find c8/ecs -path c8/ecs/node_modules -prune -o -name "*.cc" -print)
  bazel build //bzl/inliner
  echo "$cc_files" | xargs ./bazel-bin/bzl/inliner/inliner
else 
  echo "Skipping C++ files, enable with: npm run inliner -- --cc"
fi

if type buildifier &> /dev/null; then
  echo "$build_files" | xargs buildifier -lint=fix
else
  echo "buildifier not found, skipped formatting."
fi
