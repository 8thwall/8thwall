#!/bin/bash
set -euo   pipefail

ROOT=$(git rev-parse --show-toplevel)

cd $ROOT/reality/cloud/xrhome
npm ci --legacy-peer-deps
BUILDIF_FLAG_LEVEL=launch npm run dist:desktop

cd $ROOT/apps/desktop
TS=$(date +%Y%m%d%H%M)
npm version 1.0.$TS --no-git-tag-version

npm ci --os=win32 --cpu=x64
RELEASE=true npm run publish:prod:win

npm ci --cpu=arm64
npm i --cpu=x64
RELEASE=true npm run publish:prod:mac
