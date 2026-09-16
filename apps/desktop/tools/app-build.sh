#!/bin/bash
set -e

rm -rf dist
mkdir -p dist

bazel build //apps/desktop/src/shell:start
cp ../../bazel-bin/apps/desktop/src/shell/start.js dist/start.js

bazel build //apps/desktop/new-project
mkdir -p build_package
cp ../../bazel-bin/apps/desktop/new-project/new-project.zip build_package

# Generate _start.js with baked environment variables for packaged app
cat > dist/_start.js <<EOF
Object.assign(process.env, {
  DEPLOY_STAGE: '$DEPLOY_STAGE',
  RELEASE: '$RELEASE',
})

require('./start.js')
EOF

bazel build //apps/desktop/src/shell:preload
cp ../../bazel-bin/apps/desktop/src/shell/preload.js dist/preload.js

bazel build //apps/desktop:builder
