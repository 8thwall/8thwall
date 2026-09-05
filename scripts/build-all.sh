#!/bin/bash --norc
source ~/.nvm/nvm.sh
set -eu

ROOT="$(realpath "$(dirname "$0")/..")"

bazel build //reality/app/xr/js:serve-xr --config=wasmrelease

nvm use desktop
cd "$ROOT/apps/desktop"
npm run app:build
nvm use default

cd "$ROOT/apps/image-target-cli"
npm ci
npm run test

cd "$ROOT/packages/xrextras"
npm ci
npm run build

cd "$ROOT/packages/landing-page"
npm ci
npm run build

cd "$ROOT/packages/coaching-overlay"
npm ci
npm run build

cd "$ROOT/packages/dev8"
npm ci
npm run build

cd "$ROOT/c8/ecs"
npm run test

cd "$ROOT/reality/cloud/xrhome"
npm ci --legacy-peer-deps
npm run dist:desktop
npm run ts:check
npm run test

echo "All Builds Passed!"
