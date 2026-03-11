### How to use
To patch a library we use `patch-package`: https://github.com/ds300/patch-package

To make a change:
* Change your file in `./node_modules/`
* Run `patch-package` and specify the package you changed
  * Ex: `~/repo/code8/reality/cloud/xrhome$ ./node_modules/.bin/patch-package aframe`

### A-Frame: Draco loader patch
Here we patch a-frame to set the `dracoDecoderPath` path manually. When updating A-Frame make sure
to update the draco files in `/static/public/draco-xxx-xxxxxxx`. You can find new file versions here: https://unpkg.com/browse/three@0.137.0/examples/js/libs/draco

We patch in `dracoDecoderPath` manually because the the `gltf-model` prop on the `a-scene` wasn't being honored, so the defaults were not getting overridden. During testing we could manually set `gltf-model2` with the code below, but `gltf-model` wouldn't appear next to it in the HTML. Here was this test code:
```js
const SceneTag: any = 'a-scene'

const Scene = React.forwardRef((props, ref) => (
  <SceneTag
    ref={ref}
    {...props}
    gltf-model='dracoDecoderPath: https://unpkg.com/three@0.137.0/examples/js/libs/draco/'
    gltf-model2='dracoDecoderPath: https://unpkg.com/three@0.137.0/examples/js/libs/draco/'
  />
))
```

### A-Frame: Extensions patch
Here we add a patch to expose the extensions so that we can inform users if their model was Draco encoded or not.
