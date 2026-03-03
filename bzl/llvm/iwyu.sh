#!/bin/bash --norc

# First run and print clang-style diagnostics.
/opt/homebrew/bin/iwyu_tool.py -o clang "${@:2}"

# Second run output iwyu style diagnostics to an output file and return true,
# since iwyu_tool.py returns a non-zero status on success.
/opt/homebrew/bin/iwyu_tool.py "${@:2}" > $1 || true
