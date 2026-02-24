#!/bin/bash
set -e

cd $(dirname "$0")/..

OVERWRITE_FILES=true
for file in test/*.txt; do
  echo "Running OVERWRITE_FILES=true cat $file | node ./src/index.js"
  cat "$file" | node ./src/index.js >/dev/null
done
