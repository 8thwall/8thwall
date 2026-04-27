#!/bin/bash
set -euo   pipefail

ROOT=$(git rev-parse --show-toplevel)

check_variable() {
  local env_var=$1
  set +u
  if [ -z "${!env_var}" ]; then
    echo "Error: missing $env_var" >&2
    exit 1
  fi
  set -u
}

check_variable APPLE_TEAM_ID
check_variable APPLE_ID
check_variable APPLE_APP_SPECIFIC_PASSWORD

check_variable DIGICERT_CERTIFICATE_PATH
check_variable DIGICERT_KEYPAIR_ALIAS
check_variable PKCS11_CONFIG
check_variable SM_CLIENT_CERT_FILE
check_variable SM_HOST

cd $ROOT/reality/cloud/xrhome
npm ci --legacy-peer-deps
BUILDIF_FLAG_LEVEL=launch npm run dist:desktop

cd $ROOT/apps/desktop
TS=$(date +%Y%m%d%H%M)
npm version 1.0.$TS --no-git-tag-version

npm ci --os=win32 --cpu=x64
RELEASE=true npm run publish:prod:win

npm ci
RELEASE=true npm run publish:prod:mac
