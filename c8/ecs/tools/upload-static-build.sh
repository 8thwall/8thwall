#!/bin/bash --norc
set -e

monorepo_path=$(bazel info workspace)
ecs_path="$monorepo_path/c8/ecs"
timestamp=$(echo "console.log(Date.now().toString(36))" | node)
commit_id=$(git rev-parse HEAD)
stamped_name="$timestamp-$commit_id"
tag_name="ecs-release-$timestamp"

$ecs_path/tools/build-artifacts.sh "$timestamp" "$commit_id"

aws s3 cp \
  "$ecs_path/dist" \
  "s3://<REMOVED_BEFORE_OPEN_SOURCING>/web/ecs/build/$stamped_name" \
  --recursive \
  --cache-control 'public,max-age=31536000'

git push origin "HEAD:refs/tags/$tag_name"

echo "Uploaded stamped production build to:"
echo "  Runtime: https://cdn.8thwall.com/web/ecs/build/$stamped_name/runtime.js"
echo "  Dev8: https://cdn.8thwall.com/web/ecs/build/$stamped_name/dev8/dev8.js"
echo "  ECS Definition: https://cdn.8thwall.com/web/ecs/build/$stamped_name/ecs-definition-file.ts"
echo "  Metadata: https://cdn.8thwall.com/web/ecs/build/$stamped_name/metadata.json"
echo "  Git tag: https://github.com/8thwall/code8/commits/$tag_name"
