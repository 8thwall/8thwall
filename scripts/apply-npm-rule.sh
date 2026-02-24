#!/bin/bash --norc

set -eu

# Example Usage

# Install dependencies from @npm-eslint to the current directory
#   ./reality/scripts/apply-npm-rule.sh npm-eslint

# Install dependencies from @npm-c8 to reality/scripts
#   ./reality/scripts/apply-npm-rule.sh npm-c8 reality/scripts

package_name=$1
target_dir="${2-.}" # Optional

install_target="@$package_name//:$package_name"
echo "$install_target"

bazel build "$install_target"

target_modules="$target_dir/node_modules"

echo "Removing target_modules ($target_modules)"
rm -rf $target_modules

echo "Finding repo_name"
repo_name=$(basename `git rev-parse --show-toplevel`)

echo "Copying node_modules to target_models"
cp -r "bazel-$repo_name/external/$package_name/node_modules/" $target_modules
echo "Installed $package_name to $target_dir"
