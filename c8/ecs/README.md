# ECS

## Development

1. Follow the shared setup steps in [CONTRIBUTING.md](../../CONTRIBUTING.md)
2. `cd c8/ecs`
3. `npm install`
4. Make your desired changes - most game engine code is in src/runtime
5. Add unit tests if relevant
6. If imports were added or removed, update the `BUILD` files to reflect that
7. `npm run ecs:prepare` (re-run this any time c++ files are edited)
8. `npm run runtime:serve` 
9. A URL will be printed, like https://localhost:8004. First visite that URL first in the browser to accept the certificate. You can then take that url and add a script tag to your test project like `<script src="https://localhost:8004/runtime.js"></script>`.

You can also use the included demo app for a self-contained test by running `bazel run --config=wasm //c8/ecs:serve-runtime`, and opening the logged URL in the form of https://x.x.x.x:8889/c8/ecs/demo/index.html. To see further changes, rerun the command.

## Testing

`cd c8/ecs && npm run test`

## Creating a Release

The following will create a zip that contains the distributable version of the engine.

```bash
bazel build //c8/ecs:bundle
```
