#!/bin/bash --norc
set -e
source ~/.nvm/nvm.sh
nvm use 18

read -p "Ensure your local prod8 repo is next to the niantic/ monorepo and is on a new client/branch. Press enter to continue..."

cd "$(dirname "$0")/../../.."
monorepo_path=$(pwd)

# verify prod8 repo is sibling of monorepo
{
  cd "$monorepo_path/../prod8" &&
  prod8_path=$(pwd) &&
  echo "Found prod8 repo at $prod8_path"
} || {
  echo "Could not find prod8 repo as a sibling of the niantic/ repo. Enter the path to the prod8 repo root:" &&
  read -r prod8_path
  cd "$prod8_path" || {
    echo "Invalid path: '$prod8_path'. Exiting."
    exit 1
  }
}

dev8_path="$monorepo_path/apps/client/public/web/dev8"
ecs_path="$monorepo_path/c8/ecs"
release_path="$prod8_path/cdn/web/ecs/release"

echo "Purging prod8 ecs release directory..."
rm -rf "$release_path/"

read -p "Release directory cleaned. Push these changes to a prod8 PR now to ensure files will be fully reuploaded. Press enter to continue..."

# build and copy dev8
echo "Building dev8..."
cd "$dev8_path"
rm -rf dist
npm ci
npm run build
mkdir -p "$release_path/dev8"
cp dist/* "$release_path/dev8"

# build and copy runtime
echo "Building runtime.js..."
cd "$ecs_path"
rm -rf dist
npm ci
npm run runtime:build
cp "$ecs_path/dist/runtime.js" "$release_path/runtime.js"

cp "$ecs_path/gen/ecs-definition-file.ts" "$release_path/ecs-definition-file.ts"

echo "Success! Verify that dev8/, runtime.js, and ecs-definition-file.ts have been copied to the prod8 release directory, check the README for any manual changes that need to be made, and push these changes to the PR."
