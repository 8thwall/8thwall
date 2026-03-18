#!/bin/bash
set -euo   pipefail

ROOT=$(git rev-parse --show-toplevel)

cd $ROOT/reality/cloud/xrhome
npm ci --legacy-peer-deps
BUILDIF_FLAG_LEVEL=launch npm run dist:desktop

cd $ROOT/apps/desktop
npm ci
TS=$(date +%Y%m%d%H%M)
npm version 1.0.$TS --no-git-tag-version
RELEASE=true npm run publish:prod:mac
RELEASE=true npm run publish:prod:win
