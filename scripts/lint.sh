#!/bin/bash --norc

set -eu

cd "$(dirname "$0")/.."

# Get list of changed files
BAZEL_FILES=$(git diff --name-only $(git merge-base main HEAD) | grep -E "(WORKSPACE|\/BUILD|\.bzl|\.bazel)$" || true)
JS_FILES=$(git diff --name-only $(git merge-base main HEAD) | grep -E "\.[tj]sx?$" | grep -v "ecs/resources" || true)

# Only run commands if there are matching files
if [[ -n "$BAZEL_FILES" ]]; then
  echo "$BAZEL_FILES" | xargs buildifier -lint=fix
fi

if [[ -n "$JS_FILES" ]]; then
  echo "$JS_FILES" | xargs ./eslint.sh --fix
fi
