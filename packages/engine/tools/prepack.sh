#!/bin/bash
set -e

ROOT="$(bazel info workspace)"

jq -r '.version' package.json > "$ROOT/.c8version"

DIST="$ROOT/packages/engine/dist"
rm -rf "$DIST"
mkdir "$DIST"
cd "$DIST"

bazel build //reality/app/xr/js:bundle --config=wasmreleasesimd
unzip "$ROOT/bazel-bin/reality/app/xr/js/bundle.zip"
