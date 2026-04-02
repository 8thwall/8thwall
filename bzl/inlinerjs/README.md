# InlinerJS

Easily update BUILD files based on imports

## Usage

```bash
bazel run --run_under="cd $PWD && " //bzl/inlinerjs -- path/to/file.ts
```

You can also pass the BUILD file like so:

```bash
bazel run --run_under="cd $PWD && " //bzl/inlinerjs -- path/to/BUILD
```

All typescript files in the folder will be synced into the BUILD file.

Adding `--no-new` will only update rules that are already present.

TBD: VSCode integration

## Configuration

Out of the box, `inlinerjs` generates a `js_library` rule for each file, with each import as a `dep`.

The following heuristics are applied:

1. `import` statements in the file become `deps` if they resolve to a file that exists within the repo.
1. Rules ending in `-test` will default to `js_test` rather than `js_library`.
1. `@repo/reality/engine/api/mesh.capnp` maps to `//reality/engine/api:mesh.capnp-ts`.

```typescript
// build-args.ts
import type {BackendInfo} from './backend/backend-info'
import type {FunctionWebpackConfigName} from './backend/function-config-name'
import type {AppInfo, PwaInfo} from './db-types'
```

Automatically generates:

```python
js_library(
    name = "build-args",
    srcs = [
        "build-args.ts",
    ],
    deps = [
        "//reality/cloud/aws/lambda/studio-deploy:db-types",
        "//reality/cloud/aws/lambda/studio-deploy/backend:backend-info",
        "//reality/cloud/aws/lambda/studio-deploy/backend:function-config-name",
    ],
)
```

When more configuration is needed, use the following:

| Format | Description | |
| ------- | ----------- | -- |
| `// @name(my-name)` | Override `name = "..."` in the BUILD file | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/src/runtime/plugin-entry.ts#L2)
| `// @rule(js_binary)` | Override the rule used | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/tools/fix-duplication.ts#L1)
| `// @attr(polyfill = "none")` | Specify additional attributes | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/tools/fix-duplication.ts#L2)
| `// @attr[](data = "file.png")` | Specify additional attributes as an array | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/tools/generate-metadata.ts#L5-L9)
| `// @dep(//bzl/examples/hello-js)` | Specify dependencies that are not inferred automatically | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/tools/generate-metadata.ts#L4)
| `// @inliner-skip-next` | Ignore the import on the following line | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/src/runtime/world.ts#L22)
| `// @inliner-off` | Disable `inlinerjs` for this file | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/bzl/js/chai-js.js#L1)
| `// @package(npm-lambda)` | Shorthand for `npm_rule = "@npm-lambda//:npm-lambda"` | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/bzl/examples/js/resolve/transitive.ts#L2)
| `// @visibility(//visibility:public)` | Override visibility | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/bzl/js/chai-js-bin.ts#L7)
| `// @sublibrary(:example-lib)` | Mark the library as a sublibrary | [Example](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/src/runtime/world-attribute.ts#L1)

## Gotchas

1. `require('')` syntax is not supported.
1. Passing a BUILD file path on the command line does not inline .js files, since `require('')` is not supported.
1. When changing the type of build (e.x. `js_library` -> `js_binary`), the old config remains in the BUILD file and must be deleted manually.
1. When defining a rule other than `js_library`, the import must be added manually.

## Dealing with circular dependencies

If type definitions are the issue, factor out the types into separate file.

You can also combine all the circularly dependent logic into one file.

For really tricky cases, you can group the files into one `js_library` with multiple `srcs`, which looks like [this](https://github.com/8thwall/8thwall/blob/3a952f8cdcebad7537065dd55ced5fdd7ff44808/c8/ecs/src/runtime/world.ts#L19-L23).

Then you can add `// @sublibrary(:my-lib)` to each of the files which will transitively include that grouped `js_library`, allowing any consumer to import any member of the group, but resolve to the full set regardless.

An example would look like:

```
js_library(
    name = "circular-lib",
    srcs = [
        "circular-dep-1.ts",
        "circular-dep-2.ts",
    ],
)

js_library(
    name = "circular-dep-1",
    deps = [
        ":circular-lib",
    ],
)

js_library(
    name = "circular-dep-2",
    deps = [
        ":circular-lib",
    ],
)
```
