#!/bin/bash --norc

set -u

export RUNFILES_DIR=${RUNFILES_DIR:-$0.runfiles}

RESULT=0

echo Testing for public symbol c8_exampleIntMethod
$RUNFILES_DIR/niantic/bzl/llvm/llvm-objdump -p $RUNFILES_DIR/niantic/bzl/examples/windows/dll-example.dll | grep -q "c8_exampleIntMethod"
if [ $? -eq 0 ]; then
echo passed
else
echo failed
RESULT=1
fi

echo Testing for public symbol c8_exampleStringMethod
$RUNFILES_DIR/niantic/bzl/llvm/llvm-objdump -p $RUNFILES_DIR/niantic/bzl/examples/windows/dll-example.dll | grep -q "c8_exampleIntMethod"
if [ $? -eq 0 ]; then
echo passed
else
echo failed
RESULT=1
fi

exit $RESULT
