#!/bin/bash --norc

set -u

export RUNFILES_DIR=${RUNFILES_DIR:-$0.runfiles}
WORKSPACE_NAME=${TEST_WORKSPACE:-_main}
OBJDUMP="$RUNFILES_DIR/$WORKSPACE_NAME/bzl/llvm/llvm-objdump"
DLL="$RUNFILES_DIR/$WORKSPACE_NAME/bzl/examples/windows/dll-example.dll"

RESULT=0

echo Testing for public symbol c8_exampleIntMethod
"$OBJDUMP" -p "$DLL" | grep -q "c8_exampleIntMethod"
if [ $? -eq 0 ]; then
echo passed
else
echo failed
RESULT=1
fi

echo Testing for public symbol c8_exampleStringMethod
"$OBJDUMP" -p "$DLL" | grep -q "c8_exampleStringMethod"
if [ $? -eq 0 ]; then
echo passed
else
echo failed
RESULT=1
fi

exit $RESULT
