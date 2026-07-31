# Coaching Overlay

`CoachingOverlay` is a cross-platform "proto-module", meaning it's:
- Not part of XR8 and the user has to add it to their project with `<meta name="8thwall:package" content="@8thwall.coaching-overlay">`
- Is not available on XRExtras because we don't want other companies to take our code.

## Developing

- Add your IP address to `webpack.config.js`'s `devServer.host` field.
- Install the necessary modules with `npm i`
- Start a local server with `npm run-script watch`

In order to use your local version in a cloud editor project, add `<script crossorigin="anonymous" src="https://x.x.x.x:9003/coaching-overlay.js"></script>`

## Releasing

- Build a release version `npm run build`. Make sure to check that DEVELOPMENT is false in webpack.config.js
- Copy the output `dist/coaching-overlay.js` to `~/repo/prod8/cdn/web/coaching-overlay`.
- Start with it in beta, then give it a version number and promote it to prod
