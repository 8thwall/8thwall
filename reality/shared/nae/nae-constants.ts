type NaeIcon = {
  type: string
  size: [number, number]
}

// Android icon sizes are defined in the Android documentation:
// https://developer.android.com/training/graphics/colorizing-icons
const NAE_ANDROID_ICONS: NaeIcon[] = [
  {type: 'ldpi', size: [36, 36]},
  {type: 'mdpi', size: [48, 48]},
  {type: 'hdpi', size: [72, 72]},
  {type: 'xhdpi', size: [96, 96]},
  {type: 'xxhdpi', size: [144, 144]},
  {type: 'xxxhdpi', size: [192, 192]},
]
const NAE_ANDROID_ICON_SIZES = NAE_ANDROID_ICONS.map(icon => icon.size)
const NAE_ANDROID_ICON_TYPES = NAE_ANDROID_ICONS.map(icon => icon.type)

const NAE_IOS_ICONS: NaeIcon[] = [
  {type: 'icon-60x60', size: [60, 60]},      // iPhone App @1x
  {type: 'icon-120x120', size: [120, 120]},  // iPhone App @2x
  {type: 'icon-180x180', size: [180, 180]},  // iPhone App @3x

  {type: 'icon-20x20', size: [20, 20]},      // iPad Settings @1x
  {type: 'icon-76x76', size: [76, 76]},      // iPad App @1x
  {type: 'icon-152x152', size: [152, 152]},  // iPad App @2x
  {type: 'icon-167x167', size: [167, 167]},  // iPad Pro @2x

  {type: 'icon-29x29', size: [29, 29]},  // Settings @1x
  {type: 'icon-58x58', size: [58, 58]},  // Settings @2x
  {type: 'icon-87x87', size: [87, 87]},  // Settings @3x

  {type: 'icon-40x40', size: [40, 40]},  // Spotlight @1x
  {type: 'icon-80x80', size: [80, 80]},  // Spotlight @2x
]
const NAE_IOS_ICON_SIZES = NAE_IOS_ICONS.map(icon => icon.size)
const NAE_IOS_ICON_TYPES = NAE_IOS_ICONS.map(icon => icon.type)

const NAE_XRHOME_ICON_SIZES: [number, number][] = [
  [512, 512],
  [1024, 1024],
]

const NAE_PREVIEW_ICON_URL_SIZE = 512

const NAE_ANDROID_ICON_MIN_SIZE = 512

const NAE_IOS_ICON_MIN_SIZE = 1024

const NAE_ICON_SIZES: [number, number][] = [
  ...NAE_ANDROID_ICON_SIZES,
  ...NAE_IOS_ICON_SIZES,
  ...NAE_XRHOME_ICON_SIZES,
].reduce((acc: [number, number][], size) => {
  if (!acc.some(([w, h]) => w === size[0] && h === size[1])) {
    acc.push(size)
  }
  return acc
}, [])

const MAX_NAE_ICON_SIZE = 4096

const NAE_ANDROID_EXPORT_TYPES = ['apk', 'aab'] as const

const NAE_IOS_EXPORT_TYPES = ['ipa'] as const

const NAE_HTML_EXPORT_TYPES = ['zip'] as const

const NAE_EXPORT_TYPES = [
  ...NAE_ANDROID_EXPORT_TYPES,
  ...NAE_IOS_EXPORT_TYPES,
  ...NAE_HTML_EXPORT_TYPES,
] as const

const APP_NAE_BUILD_MODES = ['hot-reload', 'static'] as const

const HTML_SHELL_TYPES = ['android', 'quest', 'ios', 'osx', 'html'] as const

// WARNING: This constant is referenced and used globally, see the warning in nae-types.ts
const IOS_AVAILABLE_PERMISSIONS = ['camera', 'location', 'microphone'] as const

const IFRAME_AVAILABLE_PERMISSIONS = [
  'camera',
  'microphone',
  'geolocation',
  'accelerometer',
  'magnetometer',
  'gyroscope',
  'autoplay',
  'clipboard-read',
  'clipboard-write',
  'fullscreen',
]

// This is similar to Unity's ScreenOrientation enum
// https://docs.unity3d.com/ScriptReference/ScreenOrientation.html
const SCREEN_ORIENTATIONS = [
  'portrait',
  'portrait-down',
  'landscape-left',
  'landscape-right',
  'auto',
  'auto-landscape',
] as const

const RECENT_NAE_BUILDS_LIMIT = 6

const NAE_BUILD_COST = 2

// NOTE(lreyna): Google Play Store allows up to 50 characters for the app name,
// but we limit it to 30 characters as a reasonable maximum for display purposes.
const NAE_APP_DISPLAY_NAME_MAX_LENGTH = 30

// Android has a max length of 150 characters, and iOS has 155. We just use 150 as a sane default.
const NAE_BUNDLE_ID_MAX_LENGTH = 150

// These default icons are from the Figma design:
// - https://www.figma.com/design/D2RD4vup3zX0SPubnjcEwW/NAE?node-id=19886-2352
// They are uploaded to AWS with:
// eslint-disable-next-line max-len
// `aws s3 cp default-icon-1.jpeg s3://<REMOVED_BEFORE_OPEN_SOURCING>/images/nae/icons/defaulticon1 --content-type image/jpeg`
const NAE_DEFAULT_ICONS = [
  'defaulticonpng1',
  'defaulticonpng2',
  'defaulticonpng3',
  'defaulticonpng4',
] as const

const CAMERA_PERMISSION_USAGE_DESCRIPTION_DEFAULT =
 'This app requires camera access.'
const LOCATION_PERMISSION_USAGE_DESCRIPTION_DEFAULT =
  'This app requires location access.'
const MICROPHONE_PERMISSION_USAGE_DESCRIPTION_DEFAULT =
  'This app requires microphone access.'

export {
  APP_NAE_BUILD_MODES,
  HTML_SHELL_TYPES,
  IOS_AVAILABLE_PERMISSIONS,
  IFRAME_AVAILABLE_PERMISSIONS,
  NAE_ANDROID_EXPORT_TYPES,
  NAE_ANDROID_ICON_SIZES,
  NAE_ANDROID_ICON_TYPES,
  NAE_ANDROID_ICON_MIN_SIZE,
  NAE_IOS_ICON_MIN_SIZE,
  NAE_PREVIEW_ICON_URL_SIZE,
  NAE_IOS_ICON_SIZES,
  NAE_IOS_ICON_TYPES,
  NAE_APP_DISPLAY_NAME_MAX_LENGTH,
  NAE_BUNDLE_ID_MAX_LENGTH,
  NAE_BUILD_COST,
  NAE_EXPORT_TYPES,
  NAE_ICON_SIZES,
  MAX_NAE_ICON_SIZE,
  NAE_IOS_EXPORT_TYPES,
  SCREEN_ORIENTATIONS,
  RECENT_NAE_BUILDS_LIMIT,
  NAE_DEFAULT_ICONS,
  CAMERA_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
  LOCATION_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
  MICROPHONE_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
}
