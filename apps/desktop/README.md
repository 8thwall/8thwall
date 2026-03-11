# 8th Wall Desktop

![Screen recording of 8th Wall Desktop and Blender side by side. 8th Wall Desktop's viewport is showing a desert island scene. As changes to a 3D model are saved in Blender, the Studio viewport updates to reflect the new version of the asset.](./docs/preview.gif)

## Usage

- Install from https://8thwall.org/downloads

## Development

### General Setup

Follow general setup instructions from [CONTRIBUTING.md](../../CONTRIBUTING.md)

### Node Setup

**Electron requires node v22! Upgrade your local node version with nvm first before continuing!**

To manage multiple node versions, [install nvm](https://github.com/nvm-sh/nvm#installing-and-updating), then:

```
nvm install 22
nvm alias desktop 22
nvm install 18.17.1
nvm alias xrhome 18.17.1
nvm use desktop
```

### Running locally

2. Run the following in `reality/cloud/xrhome`:
```bash
nvm use xrhome
npm install
npm run dev:desktop:hot
```
3. In a separate terminal,  run the following in `apps/desktop`:
```bash
nvm use desktop
npm install
npm run app:start
```
