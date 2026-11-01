#!/usr/bin/env bash

set -u

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
landing_page="$repo_root/packages/landing-page"
xrextras_landing="$repo_root/packages/xrextras/src/startflow/landing"

diff_pair() {
  local label="$1"
  local left="$2"
  local right="$3"
  shift 3

  printf '\n===== %s =====\n' "$label"
  if [[ ! -e "$left" || ! -e "$right" ]]; then
    [[ -e "$left" ]] && printf 'Only in landing-page: %s\n' "${left#"$repo_root/"}"
    [[ -e "$right" ]] && printf 'Only in xrextras: %s\n' "${right#"$repo_root/"}"
    return
  fi

  git diff --no-index --no-ext-diff "$@" -- "$left" "$right"
  local status=$?
  if ((status > 1)); then
    exit "$status"
  fi
}

diff_pair \
  'components' \
  "$landing_page/src/components" \
  "$xrextras_landing/components" \
  "$@"
diff_pair \
  '3d' \
  "$landing_page/src/3d" \
  "$xrextras_landing/3d" \
  "$@"
diff_pair \
  'parameters' \
  "$landing_page/src/parameters.ts" \
  "$xrextras_landing/parameters.ts" \
  "$@"
diff_pair \
  'styles' \
  "$landing_page/src/styles.scss" \
  "$xrextras_landing/styles.module.scss" \
  "$@"
diff_pair \
  'hooks' \
  "$landing_page/src/hooks" \
  "$xrextras_landing/hooks" \
  "$@"
diff_pair \
  'resources' \
  "$landing_page/resources" \
  "$xrextras_landing/resources" \
  "$@"
