# Active task: native Windows build support

Last updated: 2026-10-19 (America/Los_Angeles)

## Goal

Make a clean Windows checkout able to run `scripts/build-all.sh` from Git Bash, then build and run `apps/desktop` as a native Windows Electron application. Leave the repository with straightforward Windows setup instructions and a repeatable clean-checkout workflow.

The work must remain based on the latest `main`. Commit each completed milestone, and merge each exact milestone commit into the Windows branch so both histories stay synchronized. Do not push unless explicitly requested. Preserve unrelated user changes, especially the pre-existing edits to `apps/desktop/package.json` and `apps/desktop/package-lock.json`.

## Machines and repositories

- Local/control machine: macOS, repository `/Users/christoph8/repo/8thwall`, branch `b2`.
- Windows build machine: `HYPERION`, Windows user `cmb`, LAN address `192.168.1.137`.
- Windows repository: `C:\Users\cmb\Repos\8thwall` (`/c/Users/cmb/Repos/8thwall` in Git Bash), branch `christoph9-windows`.
- Windows OpenSSH is online and accepts the existing key at `~/.ssh/id_ed25519_8thwall_windows` on the Mac.
- The Windows task branch was rebased onto `origin/main` at `4edfa48`. Before final validation, fetch again and verify that this is still the current `origin/main`; rebase if main advanced.

The two checkouts share Git history but not uncommitted files. Current task edits are made locally and copied to the Windows checkout for native testing. Completed Mac milestones are transferred by Git bundle and merged into `christoph9-windows`; do not rewrite or discard that branch's earlier commits. Windows pre-sync changes are preserved in named stashes.

## Connecting from the Mac

Run Git Bash explicitly through SSH so all shell scripts see the intended environment:

```sh
ssh -i "$HOME/.ssh/id_ed25519_8thwall_windows" cmb@192.168.1.137 \
  '"C:\Program Files\Git\bin\bash.exe" -lc "cd /c/Users/cmb/Repos/8thwall && git status --short --branch"'
```

For focused Bazel commands, set `BAZEL_SH` to a path that does not contain spaces:

```sh
ssh -i "$HOME/.ssh/id_ed25519_8thwall_windows" cmb@192.168.1.137 \
  '"C:\Program Files\Git\bin\bash.exe" -lc "cd /c/Users/cmb/Repos/8thwall && export BAZEL_SH=C:/Progra~1/Git/usr/bin/bash.exe && bazel build //some:target"'
```

Copy a changed file to the matching Windows path with `scp`, for example:

```sh
scp -i "$HOME/.ssh/id_ed25519_8thwall_windows" bzl/utils/gen-embed-cc.sh \
  'cmb@192.168.1.137:C:/Users/cmb/Repos/8thwall/bzl/utils/gen-embed-cc.sh'
```

## Windows setup assumptions

The current machine has Git for Windows/Git Bash, OpenSSH Server, Volta, and the required native compiler/Bazel prerequisites installed. Volta has Node 18.17.1 and Node 22 available; `scripts/build-all.sh` selects the required version per phase.

Clean-machine documentation still needs to be finalized, but it must cover at least:

- Enable Windows Developer Mode so Git can create repository symlinks without an elevated shell.
- Clone with symlinks enabled, for example `git -c core.symlinks=true clone ...`, and verify `git config core.symlinks true` before checkout.
- Install Git for Windows with Git Bash, Python (a real `python.exe`, not only the Microsoft Store shim), Volta, the repository's Bazel/Bazelisk prerequisite, and the Visual Studio C++ build tools/Windows SDK required by Bazel and Electron.
- Make Git Bash's `bash.exe` available as `BAZEL_SH`; the repository defaults and generated launchers currently expect the standard Git installation paths.
- Run builds from Git Bash, not WSL. Native Windows execution is required so Electron produces and runs Windows binaries.

The existing Windows checkout was cloned with `core.symlinks=false`, so it is useful for iterative debugging but is not sufficient as the final clean-checkout proof. Perform final validation in a new symlink-correct clone or worktree after preserving all task changes.

## Logging convention

Keep [LOG.md](LOG.md) current throughout the task. Update `Latest run` before starting each command so it always identifies the active run. The section must contain:

- the exact command;
- result and concise summary;
- a short output tail;
- a link to the complete output under the ignored Windows checkout directory `tmp/`.

Append newly diagnosed/resolved issues to the numbered problem list. Do not replace the full logs with summaries. Build logs use the filename convention `tmp/YYYY-MM-DD-<description>.log` in `C:\Users\cmb\Repos\8thwall`; `tmp/` is ignored by Git and keeps links from `LOG.md` directly clickable on HYPERION.

## Current status

The port has already addressed Windows repository rules, npm fingerprinting, workspace-status launchers, accidental WSL Bash selection, WebAssembly toolchain launchers and dependency paths, native Node tool wrappers, TypeScript/webpack input scoping, Python code generation, Cap'n Proto plugins, generated TensorFlow headers, the custom shader launcher, long archive command lines, and Emscripten's native Node execution. See [LOG.md](LOG.md) for the detailed problem history.

Milestone commits:

- `4dd6aff9` — `Add native Windows Bazel build support`; merged into the Windows branch by `4d7e968f`.
- `3c632312` — `Fix Emscripten launchers on Windows`; merged into the Windows branch by `c6fcaac3`.
- `ab5f1bfd` — `Isolate Windows webpack actions`; merged into the Windows branch by `12311252`.
- `4eeecb6d` — `Fix ECS builds and JavaScript tests on Windows`; merged into the Windows branch by `71f3ba20`.
- `878a5aa1` — `Run ECS tests natively on Windows`; merged into the Windows branch by `0264bc28`.

Most recent verified focused builds:

- `//reality/engine/deepnets:tflite-debug --config=wasmrelease`: succeeds.
- `//reality/engine/features:gr8-input-frag --config=wasmrelease`: succeeds.
- `//c8/pixels/render/shaders:render-shaders --config=wasmrelease`: succeeds after replacing unavailable Git Bash `hexdump` with portable `od` processing.
- `//c8/media:media-recorder-js-asm-cc --config=wasmrelease`: succeeds after selecting native Node for Emscripten and Git Bash for the toolchain fingerprint.

The first complete `scripts/build-bazel.sh` inventory run has finished. The script applies `--keep_going` to all three configuration-specific Bazel invocations, continues across failed groups, and returns a combined status at the end. The repository-wide phase ran 207 tests: 116 passed, 85 failed to build, four failed locally, and two were skipped. The complete log is linked from `LOG.md` and stored directly under the Windows checkout's `tmp/` directory.

The first targeted cluster, Windows webpack isolation, is fixed and validated. The `xr-tracking`, `xr`, and `xr-face` bundles now receive declared ambient TypeScript definitions, resolve generated `.capnp.ts` modules instead of raw schemas, use per-action TypeScript configs, and emit requested declarations without polluting the source checkout. All three targets pass together. The 100 artifacts from the inventory run remain preserved under `tmp/generated-worktree-2026-10-18/`, and the tracked `buildif.d.ts` is restored.

The complete `//c8/ecs/... --config=node` group now passes on native Windows: all 381 targets build and all 61 tests pass. The final fixes dispatch each Windows test executable through the appropriate interpreter, externalize npm packages with either path separator, derive `NODE_PATH` from the Windows runfiles manifest, and disable GoogleTest's optional filesystem startup capture for Windows-hosted WebAssembly. The decisive log is `tmp/2026-10-19-ecs-group-test-dispatcher-retry-4.log` in the Windows checkout.

Remaining inventoried root causes are outside the completed ECS cluster and include native dependency/compiler configuration failures, Node header layout assumptions, and repository-wide test failures from the original complete run.

## Next steps

1. Continue applying targeted fixes to the remaining complete-run root causes, then retry the complete three-group target list from `scripts/build-bazel.sh` with `--keep_going`.
2. Commit and synchronize each completed fix milestone to `christoph9-windows`.
3. Run the complete `./scripts/build-all.sh`, logging each decisive run.
4. Inspect `apps/desktop/package.json` scripts, run the native Electron application build, launch/smoke-test the app on Windows, and build its Windows installer/package (likely the repository's `make:prod:win` flow; verify rather than assume).
5. Add final README/contributor documentation for a clean Windows setup.
6. Validate from a clean, symlink-enabled checkout based on the then-current `main`.
7. Review the complete diff, run proportional Mac regression checks for shared scripts/rules, and report any validation limits.

## Things to know

- Bazel on Windows cannot directly execute arbitrary extensionless Bash scripts. Custom actions need a platform-selected `.cmd`/native launcher or a Bazel-native executable target, with any delegated `.sh` file declared as an action input.
- Avoid `C:\Windows\System32\bash.exe`; that is the WSL launcher and previously caused repository mapping to hang. Use Git Bash explicitly.
- Bazel action paths may be Windows drive paths, MSYS `/c/...` paths, or execroot-relative paths. Normalize only at the native/shell boundary; preserve dependency-file continuation backslashes.
- A bare `external/<repo>/...` path can resolve within an action execroot even when Bazel dependency validation requires the generated `bazel-out/<config>/bin/external/<repo>/...` path.
- Git Bash lacks some common Unix utilities (confirmed: `hexdump`). Prefer tools included with Git for Windows or hermetic repository tools.
- The Mac checkout may contain active task documentation or fixes between milestones. Never use destructive Git cleanup/reset commands, and never discard preserved Windows stashes.
- Full build output can be large. Stream it through `tee` into the Windows checkout's ignored `tmp/` directory and keep user-facing progress updates concise.
