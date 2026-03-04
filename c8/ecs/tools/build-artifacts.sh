#!/bin/bash --norc
set -e

monorepo_path=$(bazel info workspace)
dev8_path="$monorepo_path/apps/client/public/web/dev8"
ecs_path="$monorepo_path/c8/ecs"

# Build runtime
echo "Building runtime..."
cd "$ecs_path"
rm -rf dist
mkdir -p dist
npm ci
npm run runtime:build
rm dist/plugin.js
cp gen/ecs-definition-file.ts dist/ecs-definition-file.ts

# Build dev8
echo "Building dev8..."
cd "$dev8_path"
rm -rf dist
npm ci
npm run build
cp -r "$dev8_path/dist" "$ecs_path/dist/dev8"

# Run the TypeScript script using the existing build setup
bazel run //c8/ecs/tools:generate-metadata -- "$1" "$2" > "$ecs_path/dist/metadata.json"

echo "Build complete. Artifacts are in $ecs_path/dist:"
cd "$ecs_path/dist" && find . -type f
