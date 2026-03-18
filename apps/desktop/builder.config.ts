// @rule(js_binary)
// @attr(export_library = 1)
// @attr(externals = "*")
// @attr(target = "node")
// @package(npm-desktop)
// @attr(esnext = 1)
// @attr(commonjs = 1)

import {STUDIO_HUB_PROTOCOL} from './app/desktop-protocol'

const {RELEASE} = process.env

const suffix = process.env.DEPLOY_STAGE === 'prod' ? '' : ` (${process.env.DEPLOY_STAGE})`
const name = `8th Wall${suffix}`

export default {
  appId: STUDIO_HUB_PROTOCOL,
  productName: name,
  executableName: name,
  // eslint-disable-next-line no-template-curly-in-string
  artifactName: '8th-Wall-${version}-${os}-${arch}.${ext}',
  extraMetadata: {
    name,
  },
  directories: {
    output: 'out',
  },
  electronFuses: {
    runAsNode: true,
    enableCookieEncryption: true,
    enableNodeOptionsEnvironmentVariable: false,
    enableNodeCliInspectArguments: false,
    enableEmbeddedAsarIntegrityValidation: true,
    onlyLoadAppFromAsar: true,
  },
  files: [
    'app/dist/**/*',
    'node_modules/**/*',
    '!node_modules/npm/**/*',
    '!node_modules/**/test/**/*',
    '!node_modules/**/tests/**/*',
    '!node_modules/**/docs/**/*',
    '!node_modules/**/*.md',
    '!node_modules/**/*.txt',
    '!node_modules/**/*.yaml',
    '!node_modules/**/*.yml',
    '!test/**/*',
    '!tools/**/*',
    '!projects/**/*',
    '!*.md',
    '!.git/**/*',
    '!BUILD',
    '!bazel-*/**/*',
    '!*.sh',
  ],
  extraResources: [
    {
      from: '../../reality/cloud/xrhome/desktop-dist',
      to: 'desktop-dist',
    },
    {
      from: 'build_package/new-project.zip',
      to: 'new-project.zip',
    },
    {
      from: 'node_modules/npm',
      to: 'app.asar.unpacked/node_modules/npm',
    },
    {
      from: 'node_modules/npm/node_modules',
      to: 'app.asar.unpacked/node_modules/npm/node_modules',
    },
  ],
  asar: true,
  asarUnpack: [
    '**/node_modules/better-sqlite3/**/*',
  ],
  generateUpdatesFilesForAllChannels: true,
  mac: {
    category: 'public.app-category.developer-tools',
    target: ['dmg', 'zip'],
    icon: 'assets/icon.icns',
    entitlements: 'entitlements/osx/entitlements.plist',
    extendInfo: {
      NSCameraUsageDescription:
        '8th Wall needs camera access to enable AR experiences and content creation.',
      NSMicrophoneUsageDescription:
        '8th Wall needs microphone access for recording audio during content creation.',
      NSLocationUsageDescription:
        '8th Wall needs location access to provide location-based data during content creation.',
    },
  },
  win: {
    target: 'nsis',
    icon: 'assets/icon.ico',
    signtoolOptions: {
      sign: './windows-sign.js',
    },
  },
  nsis: {
    // eslint-disable-next-line no-template-curly-in-string
    artifactName: '8th-Wall-Setup-${version}-${os}-${arch}.${ext}',
  },
  publish: RELEASE === 'true'
    ? [
      {
        provider: 'github',
        owner: '8thwall',
        repo: 'desktop',
      },
    ]
    : [],
}
