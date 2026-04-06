# Releasing

## Publishing a pre-release build

1. Update the package.json version to the expected version number
2. `./tools/prepare.sh`
2. `cd tmp`
3. `npm publish --tag alpha --access public`

## Publishing a production build

1. Update the package.json version to indicate the changes made since last release, following Semver. Make sure the version does not have a build suffix.
2. `./tools/prepare.sh`
2. `cd tmp`
3. `npm publish --tag latest --access public --dry-run`
4. If the preview looks good, run again without `--dry-run`
