#!/bin/bash
set -e
rm -rf dist
npm run build
cd dist/external/landing-page
timestamp=$(echo "console.log(Date.now().toString(36))" | node)
zip_path="landing-page-$timestamp.zip"
zip -r "$zip_path" ./landing-page.js resources LICENSE -x '**/.*' -x '**/__MACOSX'

echo "Zipped to:"
realpath "$zip_path"
