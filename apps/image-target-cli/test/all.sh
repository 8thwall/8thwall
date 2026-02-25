#!/bin/bash
set -e

cd $(dirname "$0")/..

for file in test/*.txt; do
  echo "Running OVERWRITE_FILES=true cat $file | node ./src/index.js"
  cat "$file" | OVERWRITE_FILES=true node ./src/index.js >/dev/null
done
