#!/bin/bash --norc
set -e
npx tsc --esModuleInterop --noEmit --jsx react --target es6 --lib es2020 --lib DOM --moduleResolution node src/shared/*.d.ts "$1"  | grep  "$1"
