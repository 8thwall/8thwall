#!/bin/bash --norc

# This is wrapper for calling llvm's libtool, 'llvm-libtool-darwin' through the
# 'xcrun' wrapper in XCode, which chooses the correct platform header and
# tools. In addition, it has facilities for rewriting dependency-file paths,
# rpaths, and OSO symbols to ensure relative paths, sandbox path stripping, and
# build repeatibility.

for arg do
  shift
  [ "$arg" = "-s" ] && continue
  set -- "$@" "$arg"
done

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

# Run llvm-libtool-darwin and silence warnings about different linker inputs with the same basename.
$BAZEL_OUTPUT_BASE/$LLVM_USR/bin/llvm-libtool-darwin "$@" 2> >(
  awk '
  BEGIN {skip = 0}
  /(warning: )?file .* was specified multiple times\./ {skip = 2; next}
  skip > 0 && /^in: / {next} # Skip "in:" lines
  skip == 2 && /^$/ {skip = 1; next} # Skip middle trailing blank lines
  skip == 1 && /^$/ {skip = 0; next} # Skip ending trailing blank line
  {print} # Print all other lines
  '
)
