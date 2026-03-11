#!/bin/bash
set -e

cd $(bazel info workspace 2>/dev/null)

build_files=$(find apps/desktop -path apps/desktop/node_modules -prune -o -name "BUILD" -print)

echo "$build_files" | xargs bazel run --run_under="cd $PWD && " //apps/client/inlinerjs

if type buildifier &>/dev/null; then
  echo "$build_files" | xargs buildifier -lint=fix
else
  echo "buildifier not found, skipped formatting."
fi
