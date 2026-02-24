#!/bin/bash --norc

set -eu

bazel run --run_under="cd $PWD && " //bzl/linter:eslint -- "$@"
