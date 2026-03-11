const UNSUPPORTED_FEATURES_ANDROID = [
  'editor_page.native_publish_modal.supported_features.camera',
  'editor_page.native_publish_modal.supported_features.ar_features',
  'editor_page.native_publish_modal.supported_features.gps',
  'editor_page.native_publish_modal.supported_features.css',
  'editor_page.native_publish_modal.supported_features.media_recorder_api',
  'editor_page.native_publish_modal.supported_features.video_textures',
] as const

const UNSUPPORTED_FEATURES_IOS = [
  'editor_page.native_publish_modal.supported_features.vibration',
  'editor_page.native_publish_modal.supported_features.ar_features_ios',
] as const

const UNSUPPORTED_FEATURES_HTML = [
  'editor_page.native_publish_modal.supported_features.ar_features',
] as const

export {UNSUPPORTED_FEATURES_ANDROID, UNSUPPORTED_FEATURES_HTML, UNSUPPORTED_FEATURES_IOS}
