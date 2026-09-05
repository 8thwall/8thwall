#!/bin/bash --norc

# Lints JS/TS files with ESLint (formatting included via eslint-plugin-prettier), and Bazel files
# with buildifier.
#
#   ./scripts/lint.sh                        # files changed since merge-base with main
#   ./scripts/lint.sh --since <commit>       # files changed since <commit>
#   ./scripts/lint.sh --all                  # all tracked + untracked-not-ignored files
#   ./scripts/lint.sh --check-only           # report issues without auto-fixing
#   ./scripts/lint.sh a.ts BUILD x.bzl       # only the given files, routed by name
#
# Flags can be combined, e.g. "./scripts/lint.sh --check-only --since HEAD~1".
#
# Chosen files come from `git`: those changed since the merge-base with main (default), since a
# given commit (--since), or all tracked plus untracked-but-not-ignored files (--all), so
# anything excluded by .gitignore is skipped. Each tool applies its own ignores on top (eslint.config.js `ignores`, which reads
# .prettierignore). All applicable tools run; exits non-zero if any fails.

set -eu

cd "$(dirname "$0")/.."

CHECK_ONLY=0
ALL=0
SINCE=""
FILE_ARGS=()
while [ $# -gt 0 ]; do
  case "$1" in
    --check-only)
      CHECK_ONLY=1
      ;;
    --all)
      ALL=1
      ;;
    --since)
      if [ $# -lt 2 ]; then
        echo "Error: --since requires a commit." >&2
        exit 2
      fi
      SINCE="$2"
      shift
      ;;
    --*)
      echo "Unknown argument: $1"
      exit 1
      ;;
    *)
      FILE_ARGS+=("$1")
      ;;
  esac
  shift
done

if [ "$CHECK_ONLY" = 1 ]; then
  ESLINT_MODE=()
  BUILDIFIER_MODE=(-mode=check -lint=warn)
else
  ESLINT_MODE=(--fix)
  BUILDIFIER_MODE=(-lint=fix)
fi

bazel build //bzl/linter:eslint >/dev/null
ESLINT="bazel-bin/bzl/linter/eslint"

if [ "${#FILE_ARGS[@]}" -gt 0 ]; then
  FILES="$(printf '%s\n' "${FILE_ARGS[@]}")"
elif [ -n "$SINCE" ]; then
  FILES="$(git diff --name-only --diff-filter=MAR "$SINCE"
    git ls-files --others --exclude-standard)"
elif [ "$ALL" = 1 ]; then
  FILES="$(git ls-files --cached --others --exclude-standard)"
else
  FILES="$(git diff --name-only --diff-filter=MAR "$(git merge-base main HEAD)"
    git ls-files --others --exclude-standard)"
fi

STATUS=0

JS_FILES="$(printf '%s\n' "$FILES" | grep -E '\.m?[tj]sx?$')" || true
if [ -n "$JS_FILES" ]; then
  echo "Running ESLint..."
  printf '%s\n' "$JS_FILES" | xargs -n 100 "$ESLINT" --no-warn-ignored ${ESLINT_MODE[@]+${ESLINT_MODE[@]}} -- || STATUS=1
fi

BAZEL_FILES="$(printf '%s\n' "$FILES" | grep -E '(WORKSPACE|/BUILD|\.bzl|\.bazel)$')" || true
if [ -n "$BAZEL_FILES" ]; then
  echo "Running Buildifier..."
  printf '%s\n' "$BAZEL_FILES" | xargs -n 100 buildifier ${BUILDIFIER_MODE[@]+${BUILDIFIER_MODE[@]}} || STATUS=1
fi

exit "$STATUS"
