@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "TEST_PROGRAM=%~1"
if not defined TEST_PROGRAM exit /b 2

set "TEST_ARGS="
shift
:collect_args
if "%~1"=="" goto run_test
set "TEST_ARGS=!TEST_ARGS! "%~1""
shift
goto collect_args

:run_test
for %%F in ("%TEST_PROGRAM%") do set "TEST_EXTENSION=%%~xF"

if /I "!TEST_EXTENSION!"==".cmd" (
  call "%TEST_PROGRAM%" !TEST_ARGS!
  exit /b !ERRORLEVEL!
)
if /I "!TEST_EXTENSION!"==".bat" (
  call "%TEST_PROGRAM%" !TEST_ARGS!
  exit /b !ERRORLEVEL!
)
if /I "!TEST_EXTENSION!"==".sh" (
  "%BAZEL_SH%" "%TEST_PROGRAM%" !TEST_ARGS!
  exit /b !ERRORLEVEL!
)
if /I "!TEST_EXTENSION!"==".exe" (
  "%TEST_PROGRAM%" !TEST_ARGS!
  exit /b !ERRORLEVEL!
)

node "%TEST_PROGRAM%" !TEST_ARGS!
exit /b !ERRORLEVEL!
