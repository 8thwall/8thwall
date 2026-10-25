@echo off
setlocal
set "C8_VERSION=0.0.0.0"
if exist ".c8version" set /p C8_VERSION=<".c8version"
echo STABLE_C8_RELEASE_BUILD_ID %C8_VERSION%
