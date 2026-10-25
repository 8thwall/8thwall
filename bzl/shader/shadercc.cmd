@echo off
set "BASH_EXE=%BAZEL_SH%"
if not defined BASH_EXE if exist "C:\Program Files\Git\bin\bash.exe" set "BASH_EXE=C:\Program Files\Git\bin\bash.exe"
if not defined BASH_EXE if exist "%ProgramFiles%\Git\bin\bash.exe" set "BASH_EXE=%ProgramFiles%\Git\bin\bash.exe"
if not defined BASH_EXE if exist "%LocalAppData%\Programs\Git\bin\bash.exe" set "BASH_EXE=%LocalAppData%\Programs\Git\bin\bash.exe"
if not defined BASH_EXE (
  echo Git Bash was not found. Set BAZEL_SH to its bash.exe path. 1>&2
  exit /b 1
)
"%BASH_EXE%" "%~dp0shadercc.sh" %*
