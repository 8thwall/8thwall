#!/usr/bin/env bash
set -e

export RUNFILES_DIR=${RUNFILES_DIR:-$0.runfiles}

version_identifier=$1

if [ -z "$version_identifier" ]; then
  echo "Error: version_identifier argument is required."
  exit 1
fi

git_commit=$2

if [ -z "$git_commit" ]; then
  echo "Error: git_commit argument is required."
  exit 1
fi

timestamp=$(echo "console.log(Date.now().toString(36))" | node)

echo "Finalizing bundle:"
echo "  Version Identifier: [$version_identifier]"
echo "  Git Commit: [$git_commit]"
echo "  Timestamp: [$timestamp]"

zip_folder="$RUNFILES_DIR/_main/c8/ecs/"

if [ ! -f "$zip_folder/bundle-without-metadata.zip" ]; then
  echo "Error: bundle-without-metadata.zip not found in $zip_folder"
  exit 1
fi

echo "Generating metadata.json..."
./c8/ecs/tools/generate-metadata "$timestamp" "$git_commit" "$version_identifier" >"$zip_folder/metadata.json"
touch -t 198001010000 "$zip_folder/metadata.json"

echo "Inserting metadata.json to bundle.zip..."

cd "$zip_folder"

bundle_filename="bundle-$version_identifier.zip"
cp bundle-without-metadata.zip "$bundle_filename"
zip --quiet "$bundle_filename" metadata.json

echo "Final bundle is at $(realpath $bundle_filename)"
