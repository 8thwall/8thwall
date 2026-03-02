The original README is README_ORIGNAL.md

This version of the code was created by:

1. Cloning https://github.com/bubkoo/html-to-image
1. Checking out the latest release, tag: 1.9.0 (5c64fb6fb6252608b4d6348009cb1de07ea240c5)
1. Copying `src` from the source repo into `third_party/html-to-image/srcts`
1. Replacing all `async function` with `function`  - `await` isn't used, and `return Promise.resolve(value)` seems to always be used instead of `return value` (with `async` it would still return a promise)
1. Running:
```
tsc srcts/* --outDir src --target es6 --lib DOM \
  --declarationMap false --declaration false  --sourceMap false \
  --module commonjs --moduleResolution node
```
