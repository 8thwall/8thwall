# Developer Connection

This script is bundled into Studio development builds so the app can communicate with 

## Development version build
Run a webpack dev server that auto rebuild your dev8.js on save.

```bash
npm install
npm run serve
```

## Production version build
The script can be built from this repository by running

```bash
npm install
npm run build
```

## Using a locally build version on a project

Next, find your local ip by running something like

```bash
ifconfig en0
```

Then go to a cloud editor project and add this line to `head.html`:

```html
<script src="https://10.8.8.[your-ip]:8888/dev8.js">
```

Next, navigate to `https://10.8.8.[your-ip]:8888/dev8.js` in the browser on your phone to accept
https certificates.
