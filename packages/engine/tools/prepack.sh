#!/bin/bash
set -e

ROOT="$(bazel info workspace)"

jq -r '.version' package.json > "$ROOT/.c8version"
rm -rf dist
mkdir dist
cd dist
bazel build //reality/app/xr/js:bundle --config=wasmreleasesimd
unzip "$ROOT/bazel-bin/reality/app/xr/js/bundle.zip"
