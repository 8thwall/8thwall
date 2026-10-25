# Windows Build Log

Target environment: native Windows 10 (`HYPERION`), Git Bash, branch `christoph9-windows` rebased onto `origin/main` at `4edfa48`.

## Latest run

- Command: `./scripts/build-bazel.sh`
- Result: running on native Windows.
- Summary: running the complete three-group target list from `scripts/build-bazel.sh`; each Bazel invocation uses `--keep_going`, and the script continues through all groups before returning a combined status.
- Full output: [2026-10-19-full-build-bazel-keep-going-retry.log](tmp/2026-10-19-full-build-bazel-keep-going-retry.log)

### Tail

```text
Run starting; output will appear in the linked log.
```

## Problems encountered and resolved

1. The Windows checkout was mid-rebase on an old base.
   - Aborted the interrupted rebase, verified the two saved commits, then rebased them onto current `origin/main` (`4edfa48`).
   - Retained current main when obsolete `WORKSPACE` dependency blocks conflicted.
2. `scripts/build-all.sh` unconditionally sourced Unix nvm.
   - Installed Volta 2.0.2 on Windows and verified Node 22.23.3 and Node 18.17.1.
   - Updated the script to use Volta per command on Windows and nvm on macOS/Linux.
   - Added the missing desktop `npm ci` so the desktop phase can run from a clean checkout.
3. Bazel rejected `bazel-contrib-rules_cuda/.bazelignore` because `bazel-*` is not a valid Windows path.
   - Added an archive patch that removes the wildcard entry while retaining the concrete ignored directories.
4. The saved branch's `@nodejs_host//:node_bin` experiment did not work in a repository rule.
   - Added a direct `@nodejs_windows_amd64` input selected only on Windows; Unix retains `@nodejs_host`.
5. The npm fingerprint's multi-process shell pipeline failed with `Insufficient system resources` on Windows.
   - Replaced `find | sort | xargs | sha1sum` with a single hermetic Node process that computes the deterministic fingerprint.
6. `npm-jsxr` pulled a Git-hosted `ua-parser-js` package whose prepare step installs native fuzzing development dependencies.
   - Disabled lifecycle scripts only for this immutable repository dependency; its required `src` and `dist` files are already checked in at the pinned commit.
7. The release workspace-status command was a Bash script, which Bazel attempted to run through `cmd.exe`.
   - Added extension-neutral `stamp-build` launchers: a shell script on Unix and a `.cmd` implementation resolved through `PATHEXT` on Windows.
   - This avoids Bazel option-ordering issues between the automatic Windows config and the explicit release config.
8. Bazel repository mapping hung while the Rosetta configuration rule launched `bash` on Windows.
   - The Windows process resolver selected the WSL launcher at `C:\Windows\System32\bash.exe`, which waited indefinitely.
   - Restricted the Rosetta CPU probe to macOS, the only platform where its result is used.
9. Bazel attempted to execute generated and WebAssembly toolchain `.sh` files as native Windows programs.
   - Added Windows batch launchers that invoke the configured Git Bash executable.
   - Split the WebAssembly C++ toolchain by execution OS so macOS/Linux retain shell tools and Windows receives batch tools.
10. Cap'n Proto's Windows system-library flag was listed under `defines`, producing the invalid compiler argument `-D-lAdvAPI32`.
    - Moved `-lAdvAPI32` to `linkopts`, where it reaches link actions without corrupting compile actions.
11. The TypeScript bootstrap passed Git Bash `/c/...` paths to native Windows Node, preventing type resolution.
    - Convert the resolved npm module path to a native mixed Windows path with `cygpath` when available.
12. Emscripten could not find Python in Bazel's strict Windows action path.
    - Preserve the Windows client `PATH` for Windows actions; setup documentation will require Python to be installed on it.
    - The Emscripten launchers resolve `python.exe` first and set `PYTHON`, avoiding the empty Microsoft Store `python3` shim.
13. Generated Node tool wrappers were extensionless Bash scripts, causing Win32 error 193 during JavaScript bundling.
    - Node and Node-tool rules now emit `.cmd` launchers on Windows and invoke the hermetic Node executable through its runfiles tree.
    - The outer tool wrapper resets `RUNFILES_DIR` to its own `.cmd.runfiles` tree so Bazel cannot supply the extensionless Unix directory name.
    - Replaced the internal `<default>` polyfill sentinel with `default`; angle brackets are redirection operators in `cmd.exe` action command lines.
14. Emscripten wrote drive-qualified backslash header paths that did not match the toolchain's Unix builtin include declarations.
    - Normalize Windows dependency files to forward slashes and rewrite toolchain paths to the same synthetic root used by the native Windows C++ toolchain.
    - Convert the MSYS-form output-base match prefixes to native mixed paths so they match Emscripten's normalized `C:/...` dependency records.
    - Declare the `BAZEL_SH`-prefixed synthetic paths too, because Bazel's Windows dependency parser expands leading-slash paths through the Bash interpreter.
    - Preserve dependency-file line-continuation backslashes while converting the backslashes inside Windows paths; standalone `/` continuation tokens were the source of the misleading `bash.exe` dependency.
15. Windows webpack actions type-checked the entire repository instead of the requested bundle.
    - Windows uses local Bazel execution, so the generated `tsconfig.json`'s implicit `**/*` include could see every source in the execroot, unlike an input-only Unix sandbox.
    - Enable `ts-loader`'s `onlyCompileBundledFiles` on Windows and give the generated TypeScript project an explicit entry-file root list, so it checks the webpack graph instead of unrelated checkout files or ambient declarations.
    - The checkout was cloned with `core.symlinks=false`; clean-machine instructions must enable Git symlink support before checkout.
16. A Python code-generation rule tried to interpret a Windows `py_binary` launcher as Python source.
    - Execute the Bazel tool directly instead of prefixing it with `python3`; this works with the script launcher on Unix and the native launcher on Windows.
17. Native `capnp.exe` could not spawn raw shell-script codegen plugins on Windows.
    - Unix plugins are executable `sh_binary` targets instead of filegroups.
    - Windows selects native `py_binary` launchers that synchronously run the hermetic Node executable, avoiding raw `.sh` execution and `cmd.exe`'s rejection of Cap'n Proto's `\\?\C:\...` working directory.
18. Emscripten shortened a generated external-repository header to a source-style `external/...` dependency path.
    - Compare such entries with the action's `bazel-out/<config>/bin/external/...` tree and restore the generated path when the corresponding generated artifact exists.
    - Do not rely on the bare path being absent: Bazel's action execroot can expose a resolving `external/...` alias even though dependency validation requires the declared `bazel-out/...` path.
19. The custom shader rule exposed `shadercc.sh` directly as its executable, producing Win32 error 193.
    - Select a `.cmd` launcher on Windows and retain `shadercc.sh` as an explicit action tool so the launcher can delegate to the configured Git Bash executable.
    - Unix platforms continue to select and execute the original shell script.
20. The embedded-data generator depended on `hexdump`, which Git Bash does not include.
    - Replace its byte formatting pipeline with `od`, `tr`, and `sed`, which are available on the existing Unix build hosts and in Git for Windows.
    - Validate the generated C++ by building and compiling the focused embedded render-shader target.
21. A large Emscripten archive exceeded `cmd.exe`'s command-line limit before reaching the Git Bash archiver wrapper.
    - Enable Bazel parameter-file support and the separate `archive_param_file` feature for the Windows WebAssembly C++ toolchain so long object lists are not expanded through the batch entry point.
    - Keep the Unix WebAssembly toolchain behavior unchanged.
22. Emscripten's post-link phase could not execute its configured Node command from the Bazel action.
    - The archive fix is confirmed: the full build generated and consumed parameter files successfully before reaching this independent failure.
    - `@nodejs_host` exposed an extensionless Unix launcher and a Windows symlink whose target was an MSYS `/c/...` path; native Python subprocess creation reported `[WinError 5] Access is denied`.
    - Configure Emscripten with the real `nodejs_windows_amd64/bin/nodejs/node.exe` executable on Windows; the focused media WebAssembly target now links successfully.
23. Regenerating the Emscripten toolchain repository launched WSL's `bash.exe` and hung during its content fingerprint.
    - On Windows, resolve the fingerprint shell from `BAZEL_SH` with Git for Windows as the fallback, while Unix hosts continue to resolve `bash` normally.
    - Declare `BAZEL_SH` as a repository-rule environment dependency so configuration changes invalidate the generated repository correctly.
24. Windows-local TypeScript actions emitted undeclared build products into the source checkout.
    - The complete run created 99 untracked `.d.ts` files plus `bzl/js/webpack-build.js`, and overwrote the tracked hand-authored `reality/app/xr/js/buildif.d.ts`.
    - Moved all 100 generated artifacts into the ignored, recoverable `tmp/generated-worktree-2026-10-18/` quarantine and restored `buildif.d.ts` from the current milestone.
    - Declarations are now captured only for rules that request an output, without ts-loader writing beside sources; the TypeScript bootstrap also emits into Bazel's output tree.
25. Windows webpack input scoping excluded required ambient declarations and exposed raw Cap'n Proto schemas.
    - The three XR bundles could not resolve globals such as `BuildIf`, `XR8`, `THREE`, and `window._c8` because the Windows project included only entry files.
    - Native local resolution also selected checkout `.capnp` schema files ahead of declared generated `.capnp.ts` modules and tried to parse the schemas as JavaScript.
    - Webpack now receives the complete declared-source manifest, includes its declared ambient types, and resolves Cap'n Proto imports to declared generated TypeScript files.
    - Each Windows-local bundle writes an output-specific TypeScript config, preventing concurrent actions from racing on a shared execroot-level `tsconfig.json`.
    - The three-target validation passes together and leaves the source checkout clean of generated build products.
26. The `npm-ecs` repository failed in its `patch-package` postinstall command on Windows.
    - The generated `.cmd` lifecycle shim returned “The system cannot find the path specified” even though npm had installed all dependencies.
    - Invoke `patch-package` through the already-running hermetic Node executable instead of relying on command-shim resolution; the next complete ECS run fetched and analyzed all 381 targets.
27. JavaScript CLIs and tests were emitted as extensionless Bash launchers on Windows.
    - Bazel could not execute the npm typecheck generator, and every runnable ECS JavaScript test failed with Win32 error 193.
    - Derive the execution OS from the exec-configured webpack tool rather than the transitioned target configuration, then emit `.cmd` runfiles launchers for Windows JS CLI and test rules while retaining shell launchers on Unix.
    - Preserve Bazel's supplied test `RUNFILES_DIR`, use it to resolve the Mocha reporter configuration, and fall back to the adjacent runfiles tree for direct invocation. The full ECS group now passes all 52 JavaScript tests outside the CRDT data-file cluster.
28. TinyGLTF included its vendored `json.hpp` without declaring it as a source/header.
    - Add the header to the external repository BUILD target so Bazel's Windows include validation accepts it.
29. Moving concurrent webpack configs under their output directories changed how full declaration bundling interpreted relative project paths.
    - Use absolute entry, base, and TypeScript path-mapping locations so `full_dts` generation resolves declared generated modules independently of the config-file directory.
30. Bazel could not directly launch extensionless Emscripten tests or generated shell tests on Windows.
    - Stage a Windows test dispatcher through `--run_under` so `.cmd`/`.bat`, `.sh`, `.exe`, and extensionless JavaScript test executables use the appropriate interpreter.
    - Use the host Node installation for extensionless Emscripten outputs, whose own test runfiles do not contain a Node runtime; both generated `sh_test` launchers and all four Emscripten tests now execute.
31. Windows path separators prevented npm externalization and the Windows test wrapper omitted the Unix wrapper's `NODE_PATH` setup.
    - Recognize both slash styles when deciding whether webpack resolved a module below `node_modules`.
    - Resolve the npm repository fingerprint through the Windows runfiles manifest and derive `NODE_PATH` from its real external-repository location, allowing external packages and Automerge's adjacent WASM file to resolve.
32. GoogleTest aborted during startup when Emscripten `NODERAWFS` could not return a usable Windows current directory.
    - Disable GoogleTest's optional filesystem startup capture for Windows-hosted WebAssembly with its supported `GTEST_HAS_FILE_SYSTEM=0` override.
    - The complete 381-target ECS run now builds successfully and all 61 tests pass on native Windows.

## Remaining work

- Complete the Bazel build and tests.
- Complete all npm build stages under their required Node versions.
- Build and smoke-test the native Electron app and NSIS installer.
- Finalize clean-machine Windows setup documentation.
