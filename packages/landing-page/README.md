# Landing Page

Fallback page displayed when an 8th Wall experience cannot run on the current device

![Preview of the landing page showing a splash image and QR code](./docs/preview.png)

## Usage

See https://8thwall.org/docs/engine/guides/landing-pages for a complete guide to customizing the landing page.

### Option 1: Script tag

```html
<script src="https://cdn.jsdelivr.net/npm/@8thwall/landing-page@1/dist/landing-page.js" crossorigin="anonymous"></script>
```

### Option 2: npm

```bash
npm install @8thwall/landing-page
```

You will need to copy the included artifacts into your dist folder, for example in webpack:

```js
new CopyWebpackPlugin({
  patterns: [
    {
      from: 'node_modules/@8thwall/landing-page/dist',
      to: 'external/landing-page',
    }
  ]
})
```

You can then load the library by adding the following to index.html:

```html
<script src="./external/landing-page/landing-page.js"></script>
```

When you import the package, a simple helper for accessing window.LandingPage is provided. The expectation is still that the script tag is added to the HTML. Note that configuring the landing page in this way is not required for A-Frame projects as the configuration is passed through the component schema.

```js
import * as LandingPage from '@8thwall/landing-page'
LandingPage.configure({
  font: 'Arial',
})
```

## Development

- Start a local server with `npm run watch`
- Try out the test project at `https://localhost:9002/`. Query parameters allow custom presets and overrides, for example:
  - https://localhost:9002/?video
  - https://localhost:9002/?helmet&basic=0&textColor=red
  - https://localhost:9002/?portrait
  - See [test-parameters.ts](./src/test-parameters.ts) for full list
- In order to use your local version in a project, replace the existing script tag with `<script crossorigin="anonymous" src="https://localhost:9002/landing8.js"></script>`
