#!/bin/bash --norc
source ~/.nvm/nvm.sh
set -eu

TMP_DIR="$(mktemp -d)"
BUILD_LOGS="$TMP_DIR/output.log"

handle_fail() {
  printf '\e8%-10s' "failed on: "
  tail -n 20 "$BUILD_LOGS"
  echo "Full logs at $BUILD_LOGS"
  exit 1
}

run_checked() {
  echo "$@" > "$BUILD_LOGS"
  "$@" >> "$BUILD_LOGS" 2>&1 || handle_fail
}

start_job() {
  local job_name="$1"
  SECONDS=0
  printf "%-20s\e7running" "$job_name" 
}

finish_job() {
  printf '\e8%-10s %3ss\n' "passed" "$SECONDS"
  rm "$BUILD_LOGS"
}

start_job_in() {
  cd "$(git rev-parse --show-toplevel)/$1"
  start_job "$(basename "$1")"
}


start_job bazel
run_checked ./scripts/build-bazel.sh
finish_job

start_job_in "apps/desktop"
run_checked nvm use desktop
run_checked npm run app:build
run_checked nvm use default
finish_job

start_job_in "apps/image-target-cli"
run_checked npm ci
run_checked npm run test
finish_job

start_job_in "packages/xrextras"
run_checked npm ci
run_checked npm run build
finish_job

start_job_in "packages/landing-page"
run_checked npm ci
run_checked npm run build
finish_job

start_job_in "packages/coaching-overlay"
run_checked npm ci
run_checked npm run build
finish_job

start_job_in "packages/dev8"
run_checked npm ci
run_checked npm run build
finish_job

start_job_in "c8/ecs"
run_checked npm run test
finish_job

start_job_in "reality/cloud/xrhome"
run_checked npm ci --legacy-peer-deps
run_checked npm run dist:desktop
run_checked npm run ts:check
run_checked npm run test
finish_job

echo "All Builds Passed!"
rm -rf "$TMP_DIR"
