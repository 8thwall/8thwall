# ECS

The game engine behind 8th Wall Studio. ECS is short for "Entity Component System". It is based on FLECS for state management, three.js for rendering, and Jolt for physics.

## Usage

The easiest way to get started with ECS is to install the desktop app from https://8thwall.org/downloads and start a fresh project. You can also check out examples [here](https://8th.io/examples).

Documentation: https://8thwall.org/docs/studio/essentials/overview

### Option 1: Script tag

```html
<script src="https://cdn.jsdelivr.net/npm/@8thwall/ecs@3/dist/runtime.js" crossorigin="anonymous"></script>
```

### Option 2: npm

```
npm install @8thwall/ecs
```

You will need to copy the included artifacts into your dist folder, for example in webpack:

```js
new CopyWebpackPlugin({
  patterns: [
    {
      from: 'node_modules/@8thwall/ecs/dist',
      to: 'external/ecs',
    }
  ]
})
```

You can then load the SDK by adding the following to index.html:

```html
<script src="./external/ecs/runtime.js"></script>
```

When importing the package, you will get a simple helper for accessing `window.ecs`. This will only resolve if the script tag is included in the HTML.

```js
import * as ecs from '@8thwall/ecs'

ecs.registerComponent({name: 'example'})
```

## Development

See [c8/ecs/README.md](../../c8/ecs/README.md)
