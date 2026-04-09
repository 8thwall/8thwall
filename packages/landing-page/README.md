# Landing Page

Fallback page displayed when an 8th Wall experience cannot run on the current device

![

## Usage

### Option 1: Script tag

```html
<script src="https://cdn.jsdelivr.net/npm/@8thwall/landing-page@1/dist/landing-page.js" crossorigin="anonymous"></script>
```

### Option 2: npm

```
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

When you import the package, a simple helper for accessing window.LandingPage is provided. The expectation is still that the script tag is added to the HTML. 

```js
import * as LandingPage from '@8thwall/landing-page'
LandingPage.configure({
  font: 'Arial',
})
```

### A-Frame

When using A-Frame, add the `landing-page` component to `<a-scene>`. 

## Configuration



## Development

- Start a local server with `npm run watch`
- Try out the test project at `https://localhost:9002/`
  - Query parameters allow custom presets an overrides, for example https://localhost:9002/?helmet&basic=0 and https://localhost:9002/?helmet&basic=0&textColor=red, see [test-parameters.ts](./src/test-parameters.ts)
- In order to use your local version in, add `<script crossorigin="anonymous" src="https://localhost:9002/landing8.js"></script>`
