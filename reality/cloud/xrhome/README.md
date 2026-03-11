# XRHome

React project that contains the frontend code for the Desktop App.

## Development

### General Setup

1. Follow instructions from [CONTRIBUTING.md](../../CONTRIBUTING.md)

### Node Setup

To manage multiple node versions, [install nvm](https://github.com/nvm-sh/nvm#installing-and-updating), then:

```
nvm install 18.17.1
nvm alias xrhome 18.17.1
nvm use xrhome
```

See the [Desktop App README.md ](../../../apps/desktop/README.md).


### Type Checking

```bash
npm run ts:check
```

```bash
npm run ts:check-file src/client/common/index.ts
```

### Testing

```bash
npm run test
```

```bash
npm run test-file test/replace-asset-test.ts
```
