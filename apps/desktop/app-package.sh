#!/bin/bash
set -e

if [ -z "$DEPLOY_STAGE" ]; then
  echo "Error: DEPLOY_STAGE environment variable is required"
  exit 1
fi

if [ -z "$BUILDER_COMMAND" ]; then
  echo "Error: BUILDER_COMMAND is required (start, build, or publish)"
  exit 1
fi

export PLATFORM=${PLATFORM:-mac}

export DEPLOY_STAGE="$DEPLOY_STAGE"

ARCH=""
if [ "$PLATFORM" == "win" ]; then
  ARCH="--x64"
fi

# NOTE(christoph): When we run a release build, we build for both x64 and arm64 so that
# latest-mac.yml can be used for both architectures. For non-release builds, we just build for
# the default.
if [ "$RELEASE" == "true" ] && [ "$PLATFORM" == "mac" ]; then
  ARCH="--x64 --arm64"
fi

# build electron app
./app-build.sh

mkdir -p build_package
cp new-project.zip build_package/

case "$BUILDER_COMMAND" in
"start")
  npx electron-builder install-app-deps
  electron .
  ;;
"build")
  echo "Building Electron app with electron-builder..."
  npx electron-builder --config ../../bazel-bin/apps/desktop/builder.js --${PLATFORM} ${ARCH}
  ;;
"publish")
  echo "Publishing Electron app with electron-builder..."
  npx electron-builder --config ../../bazel-bin/apps/desktop/builder.js --${PLATFORM} ${ARCH} --publish always
  ;;
*)
  echo "Error: Unknown BUILDER_COMMAND '$BUILDER_COMMAND'. Use 'start', 'build', or 'publish'"
  exit 1
  ;;
esac
