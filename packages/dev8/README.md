# Developer Connection

This script is bundled into Studio development builds so the app can communicate with dev8.

## Development version build

Run a webpack dev server that automatically rebuilds `dev8.js` on save.

```bash
npm install
npm run serve
```

The development bundle is hosted at:

```text
https://localhost:9000/dev8.js
```

## Production version build

The script can be built from this repository by running:

```bash
npm install
npm run build
```

## Tests

```bash
npm test
```

## Using a local dev8 build in Studio

1. Start the local dev8 development server:

   ```bash
   npm run serve
   ```

2. Confirm that `https://localhost:9000/dev8.js` is being hosted.

3. Open a project in Studio.

4. In the Studio project, open `config/webpack.config.js` and replace the `createDev8Plugin` line with:

   ```js
   createDev8Plugin({src: 'https://localhost:9000/dev8.js'}),
   ```

5. In Studio, open the **System** log tab and click **Restart server**.

6. Refresh the simulator. It should now use the locally hosted dev8 build.

   To confirm that the local build is being used, you can temporarily add an `alert()` to the local dev8 source, refresh the simulator, and verify that the alert appears.
