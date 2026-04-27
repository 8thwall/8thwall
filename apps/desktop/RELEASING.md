# Releasing

## 1. Credentials

We use an Apple Developer account to notarize apps for Mac and a Digicert certificate to sign apps for Windows. Publishing to Github Releases also requires a personal access token.

### Apple

1. Sign up for an Apple Developer Account, set the `APPLE_TEAM_ID` variable to match the team ID.
2. Go to https://account.apple.com/account/manage and generate an app specific password, store your email as `APPLE_ID` and the password as `APPLE_APP_SPECIFIC_PASSWORD`
3. Get an Developer ID certificate and install it.
   1. For first time setup, select "Developer ID Certificate" here: https://developer.apple.com/account/resources/certificates/add and follow the necessary process.
   2. Otherwise, get the credentials from a maintainer and install them.
   3. When set up correctly, `security find-identity -p codesigning -v` will show a valid identity matching your `APPLE_TEAM_ID` variable.

### Windows

https://www.electron.build/tutorials/code-signing-windows-apps-on-unix#signing-windows-app-on-maclinux-using-jsign

1. Set up tools
   1. `java --version`, otherwise `brew install openjdk` and add the PATH to your env
   2. `smctl --version`, otherwise download and set up [SMCTL](https://docs.digicert.com/en/software-trust-manager/client-tools/command-line-interface/smctl.html#download-smctl-487706)
   3. `jsign --version`, otherwise `brew install jsign`
2. Create or obtain an EV Signing Certificate
   - Instructions for managing the Signer can be found [here](https://docs.digicert.com/en/digicert-keylocker/certificates/update-signer.html) in case you are not currently the registered signer.
3. Set up `~/digicert` folder with the following contents:
   - `cert_xxx.pem` (matching the keypair alias)
   - `Certificate_pkcs12.p12`
   - `pkcs11properties.cfg` (set up according to [this guide](https://docs.digicert.com/en/digicert-keylocker/ci-cd-integrations-and-deployment-pipelines/scripts/gradle/scripts-for-signing-using-pkcs11-library-on-gradle.html))
3. Set the following variables:
```
DIGICERT_CERTIFICATE_PATH: /path/to/digicert/cert_xxx.pem
DIGICERT_KEYPAIR_ALIAS: key_xxx
PKCS11_CONFIG: /path/to/digicert/pkcs11properties.cfg
SM_CLIENT_CERT_FILE: /path/to/digicert/Certificate_pkcs12.p12
SM_HOST: https://clientauth.one.digicert.com
```

### Github
 
1. Go to https://github.com/settings/personal-access-tokens
2. Click "Generate a new token"
3. Select the releases repo (8thwall/desktop)
4. Grant access to "artifact metadata" and "code"
5. Set the token on your env as `GH_TOKEN`

## 2. Run the build

```sh
./tools/publish-release.sh
```

### Troubleshooting

- If there is a "bad CPU" error, install Rosetta with `softwareupdate --install-rosetta`

### 3. Finalize the release

1. Go to https://github.com/8thwall/desktop/releases
2. The build should be present as a "Draft" release.
3. Verify that all 3 sets of artifacts are included in the artifacts: mac-arm64, mac-x64, and win-x64.
4. Click edit
6. In the main repo, create a tag of the built commit, matching the commit used to build: `git push origin <commit>:refs/tags/desktop-release-1.0.xxx`
7. Add a link to that tag in the body of the release
8. Click publish

### 4. Update download links

The following links are used to download the app for the first time:

https://8th.io/mac-arm64-latest
https://8th.io/mac-intel-latest
https://8th.io/win-x64-latest
 
Update these shortlinks to point to the latest release URLs, which is managed at https://github.com/8thwall/go-links/blob/main/public/_redirects

Merge to `main` and the links will update within 5 minutes.
