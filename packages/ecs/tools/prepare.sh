#!/bin/bash
set -e

ROOT=$(git rev-parse --show-toplevel)

cd "$ROOT/packages/ecs"

rm -rf tmp
mkdir -p tmp/dist

cp package.json LICENSE README.md tmp
cp tools/entry.js tmp/index.js

version_identifier=$(cat package.json | jq -r '.version')
timestamp=$(echo "console.log(Date.now().toString(36))" | node)
git_commit=$(git log -n 1 --format="%H")

cd tmp

bazel run //c8/ecs/tools:generate-metadata --config=wasmrelease -- "$timestamp" "$git_commit" "$version_identifier" > "metadata.json"

bazel build //c8/ecs:bundle-without-metadata --config=wasmrelease

cd dist
unzip ../../../../bazel-bin/c8/ecs/bundle-without-metadata.zip > /dev/null

mv runtime.d.ts ../index.d.ts

cd "$ROOT/packages/dev8"
npm ci
npm run build
mv "$ROOT/packages/dev8/dist" "$ROOT/packages/ecs/tmp/dev8"
