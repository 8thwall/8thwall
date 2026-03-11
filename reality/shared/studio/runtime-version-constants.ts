const CDN_BASE_URL = 'https://cdn.8thwall.com'
const S3_BUCKET_NAME = '8w-us-west-2-web'
const S3_BUILD_PREFIX = 'web/ecs/build/'
const S3_RELEASE_PREFIX = 'web/ecs/version'
const S3_VERSION_HISTORY_KEY = 'web/ecs/version/version-history.json'
const S3_VERSION_HISTORY_BACKUPS_PREFIX = 'web/ecs/version/version-history-backups'
const S3_REGION = 'us-west-2'

// Note(Dale): This is the initial runtime version used projects with unset runtime versions.
// In the future, we will add an error when the runtime version is not set.
const INITIAL_RUNTIME_VERSION = {
  type: 'version',
  level: 'major',
  major: 1,
} as const

export {
  CDN_BASE_URL,
  S3_BUCKET_NAME,
  S3_BUILD_PREFIX,
  S3_RELEASE_PREFIX,
  S3_VERSION_HISTORY_KEY,
  S3_VERSION_HISTORY_BACKUPS_PREFIX,
  S3_REGION,
  INITIAL_RUNTIME_VERSION,
}
