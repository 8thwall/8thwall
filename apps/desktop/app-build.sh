#!/bin/bash
set -e

mkdir -p app/dist

bazel build //apps/desktop/shell:start
cp ../../bazel-bin/apps/desktop/shell/start.js app/dist/start.js

bazel build //apps/desktop/new-project
mkdir -p build_package
cp ../../bazel-bin/apps/desktop/new-project/new-project.zip build_package

# Generate _start.js with baked environment variables for packaged app
cat >app/dist/_start.js <<EOF
Object.assign(process.env, {
  DEPLOY_STAGE: '$DEPLOY_STAGE',
  RELEASE: '$RELEASE',
})

require('./start.js')
EOF

bazel build //apps/desktop/shell:preload
cp ../../bazel-bin/apps/desktop/shell/preload.js app/dist/preload.js

bazel build //apps/desktop:builder
