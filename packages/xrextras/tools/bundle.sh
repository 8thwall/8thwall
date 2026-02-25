#!/bin/bash
set -e
rm -rf dist

echo "Building..."
npm run build
timestamp=$(echo "console.log(Date.now().toString(36))" | node)

echo "Zipping xrextras..."
cd dist/external/xrextras
zip_path="xrextras-$timestamp.zip"
zip -r "$zip_path" ./xrextras.js resources LICENSE -x '**/.*' -x '**/__MACOSX'
echo "Zipped xrextras to:"
realpath "$zip_path"
cd ../../..
