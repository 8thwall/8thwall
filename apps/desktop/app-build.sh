#!/bin/bash
set -e

mkdir -p app/dist

bazel build //apps/desktop/app:start
cp ../../bazel-bin/apps/desktop/app/start.js app/dist/start.js

# Generate _start.js with baked environment variables for packaged app
cat >app/dist/_start.js <<EOF
Object.assign(process.env, {
  DEPLOY_STAGE: '$DEPLOY_STAGE',
  RELEASE: '$RELEASE',
})

require('./start.js')
EOF

bazel build //apps/desktop/app:preload
cp ../../bazel-bin/apps/desktop/app/preload.js app/dist/preload.js

bazel build //apps/desktop:builder
