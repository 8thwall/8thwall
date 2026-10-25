#!/bin/bash --norc
set -eu

NODE_DEFAULT_VERSION="18.17.1"
NODE_DESKTOP_VERSION="22"

case "$(uname -s)" in
MINGW* | MSYS* | CYGWIN*)
  export BAZEL_SH="$(cygpath -w "$(command -v bash)")"
  if command -v volta >/dev/null 2>&1; then
    VOLTA_BIN="$(command -v volta)"
  elif [ -x "/c/Program Files/Volta/volta.exe" ]; then
    VOLTA_BIN="/c/Program Files/Volta/volta.exe"
  else
    echo "Volta is required on Windows. Install it with: winget install Volta.Volta" >&2
    exit 1
  fi
  ;;
*)
  if [ ! -s "$HOME/.nvm/nvm.sh" ]; then
    echo "nvm is required. See https://github.com/nvm-sh/nvm" >&2
    exit 1
  fi
  # shellcheck source=/dev/null
  source "$HOME/.nvm/nvm.sh"
  ;;
esac

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

run_with_node() {
  local version="$1"
  shift

  if [ -n "${VOLTA_BIN:-}" ]; then
    run_checked "$VOLTA_BIN" run --node "$version" "$@"
  else
    run_checked nvm use "$version"
    run_checked "$@"
  fi
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
run_with_node "$NODE_DESKTOP_VERSION" npm ci
run_with_node "$NODE_DESKTOP_VERSION" npm run app:build
finish_job

start_job_in "apps/image-target-cli"
run_with_node "$NODE_DEFAULT_VERSION" npm ci
run_with_node "$NODE_DEFAULT_VERSION" npm run test
finish_job

start_job_in "packages/xrextras"
run_with_node "$NODE_DEFAULT_VERSION" npm ci
run_with_node "$NODE_DEFAULT_VERSION" npm run build
finish_job

start_job_in "packages/landing-page"
run_with_node "$NODE_DEFAULT_VERSION" npm ci
run_with_node "$NODE_DEFAULT_VERSION" npm run build
finish_job

start_job_in "packages/coaching-overlay"
run_with_node "$NODE_DEFAULT_VERSION" npm ci
run_with_node "$NODE_DEFAULT_VERSION" npm run build
finish_job

start_job_in "packages/dev8"
run_with_node "$NODE_DEFAULT_VERSION" npm ci
run_with_node "$NODE_DEFAULT_VERSION" npm run build
finish_job

start_job_in "c8/ecs"
run_with_node "$NODE_DEFAULT_VERSION" npm run test
finish_job

start_job_in "reality/cloud/xrhome"
run_with_node "$NODE_DEFAULT_VERSION" npm ci --legacy-peer-deps
run_with_node "$NODE_DEFAULT_VERSION" npm run dist:desktop
run_with_node "$NODE_DEFAULT_VERSION" npm run ts:check
run_with_node "$NODE_DEFAULT_VERSION" npm run test
finish_job

echo "All Builds Passed!"
rm -rf "$TMP_DIR"
