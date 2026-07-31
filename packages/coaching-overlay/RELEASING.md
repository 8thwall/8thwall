# Releasing

## Publishing a pre-release build

1. Update the package.json to contain a version like "1.x.x-alpha.3"

```
npm publish --tag alpha --access public --pack-destination tmp
```

## Publishing a production build

1. Update the package.json version to indicate the changes made since last release, following Semver.
2. Run: `npm publish --tag latest --access public --pack-destination tmp --dry-run`
3. If everything looks good, run the command again without `--dry-run`

