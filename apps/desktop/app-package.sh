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
export ARCH=${ARCH:-arm64}

export DEPLOY_STAGE="$DEPLOY_STAGE"

if [ "$RELEASE" = "true" ] && [ "$DEPLOY_STAGE" = "prod" ]; then
  export S3_BUCKET_KEY="8w-us-west-2-web"
  export DEPLOY_PATH="web/desktop/rc/${PLATFORM}/${ARCH}"
  export CDN_URL="https://cdn.8thwall.com/web/desktop/latest/${PLATFORM}/${ARCH}"
else
  export S3_BUCKET_KEY="8w-us-west-2-web-test"
  export DEPLOY_PATH="web/desktop-test/${DEPLOY_STAGE}/${PLATFORM}/${ARCH}"
  export CDN_URL="https://cdn-dev.8thwall.com/${DEPLOY_PATH}"
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
  npx electron-builder --config ../../bazel-bin/apps/desktop/builder.js --${PLATFORM} --${ARCH}
  ;;
"publish")
  echo "Publishing Electron app with electron-builder..."
  npx electron-builder --config ../../bazel-bin/apps/desktop/builder.js --${PLATFORM} --${ARCH} --publish always
  ;;
*)
  echo "Error: Unknown BUILDER_COMMAND '$BUILDER_COMMAND'. Use 'start', 'build', or 'publish'"
  exit 1
  ;;
esac
