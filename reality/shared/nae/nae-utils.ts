import {
  fromAttributes,
} from '../typed-attributes'

import type {
  AttributesForRaw,
} from '../typed-attributes'

import type {DynamoDbApi} from '../dynamodb'

import {
  APP_NAE_BUILD_MODES,
  NAE_DEFAULT_ICONS,
  NAE_PREVIEW_ICON_URL_SIZE,
  NAE_APP_DISPLAY_NAME_MAX_LENGTH,
  NAE_EXPORT_TYPES,
  NAE_BUNDLE_ID_MAX_LENGTH,
} from './nae-constants'
import type {
  AndroidBundleIdValidationResult,
  AppDisplayNameValidationResult,
  ExportType,
  HistoricalBuildData,
  HtmlShell,
  NaeBuildMode,
  RefHead,
  ScreenOrientation,
  SigningInfo,
  SigningFilesInfoStatusResult,
  SigningConfigData,
  CertificateInfo,
  AuthKeyInfo,
  UploadAuthKeyData,
  CertificateProjection,
  SigningConfigsResult,
  AppleSigningType,
  SigningConfigProjection,
  PermissionUsageDescriptionValidationResult,
} from './nae-types'

const getCsrPem = (csrBase64: string): string => `-----BEGIN CERTIFICATE REQUEST-----\n${
  csrBase64.match(/.{1,64}/g)?.join('\n') || ''
}\n-----END CERTIFICATE REQUEST-----\n`

const validateAppName = (appName: string): boolean => {
  // Based on reality/cloud/xrhome/src/shared/app-utils.ts, but without the length check
  const appNamePattern = '^[a-z0-9][a-z0-9-]*$'
  const appNameRegex = new RegExp(appNamePattern)
  return appNameRegex.test(appName)
}

// https://developer.android.com/guide/topics/resources/string-resource#FormattingAndStyling
// Escape special characters in the display app name for Android
// NOTE(lreyna): This also prevents issues when passing the request payload to `exec`
const validateAppDisplayName = (displayAppName: string): AppDisplayNameValidationResult => {
  if (!displayAppName) {
    return 'empty'
  }

  const invalidChars = /[\t\n\r]/g
  if (invalidChars.test(displayAppName)) {
    return 'invalid-char'
  }

  const checkEscaped = /(?<!\\)[&<>"'@?]/g
  if (checkEscaped.test(displayAppName)) {
    return 'char-needs-escape'
  }

  const unicodeEscapeRegex = /U\+[0-9A-Fa-f]{4}/g
  if (unicodeEscapeRegex.test(displayAppName)) {
    return 'unicode-char'
  }

  if (displayAppName.trim() !== displayAppName) {
    return 'leading-or-trailing-whitespace'
  }

  if (displayAppName.length > NAE_APP_DISPLAY_NAME_MAX_LENGTH) {
    return 'too-long'
  }

  return 'success'
}

const NAE_PERMISSION_USAGE_DESCRIPTION_MAX_CHARACTERS = 200

const validatePermissionUsageDescriptionUtil = (
  usageDescription: string
): PermissionUsageDescriptionValidationResult => {
  if (!usageDescription) {
    return 'success'
  }

  const invalidChars = /[\t\n\r]/g
  if (invalidChars.test(usageDescription)) {
    return 'invalid-char'
  }

  const checkEscaped = /(?<!\\)[&<>"'@?]/g
  if (checkEscaped.test(usageDescription)) {
    return 'char-needs-escape'
  }

  const unicodeEscapeRegex = /U\+[0-9A-Fa-f]{4}/g
  if (unicodeEscapeRegex.test(usageDescription)) {
    return 'unicode-char'
  }

  const suspiciousUnicodeChars =
    /[\u202A-\u202E\u2066-\u2069\u200B-\u200D\uFEFF\u2028\u2029\uE000-\uF8FF]/
  if (suspiciousUnicodeChars.test(usageDescription)) {
    return 'unicode-char'
  }

  if (usageDescription.trim() !== usageDescription) {
    return 'leading-or-trailing-whitespace'
  }

  if (usageDescription.length > NAE_PERMISSION_USAGE_DESCRIPTION_MAX_CHARACTERS) {
    return 'too-long'
  }

  if (!/[A-Za-z0-9\u00C0-\uFFFF]/.test(usageDescription)) {
    return 'missing-letter-or-digit'
  }

  return 'success'
}

const validateBuildMode =
  (buildMode: string) => APP_NAE_BUILD_MODES.includes(buildMode as NaeBuildMode)

const validateExportType =
  (exportType: string) => NAE_EXPORT_TYPES.includes(exportType as ExportType)

const isLetter = (ch: string): boolean => (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')

const isDigit = (ch: string): boolean => ch >= '0' && ch <= '9'

const bundleIdSeparator = (
  platform: HtmlShell
) => ((platform === 'android' || platform === 'quest') ? '_' : '-')

// From: https://developer.android.com/build/configure-app-module
// - It must have at least two segments (one or more dots).
// - Each segment must start with a letter.
// - All characters must be alphanumeric or an underscore [a-zA-Z0-9_].
const validateBundleIdUtil = (
  platform: HtmlShell, bundleId: string
): AndroidBundleIdValidationResult => {
  if (!bundleId) {
    return 'empty'
  }

  if (bundleId.length > NAE_BUNDLE_ID_MAX_LENGTH) {
    return 'too-long'
  }

  const parts = bundleId.split('.')
  if (parts.length < 2) {
    return 'too-few-parts'
  }

  for (const part of parts) {
    if (part.length === 0) {
      return 'empty-part'
    }

    const firstChar = part[0]
    if (!isLetter(firstChar)) {
      return 'first-char-of-part-not-letter'
    }

    const separator = bundleIdSeparator(platform)
    for (const ch of part) {
      if (!isLetter(ch) && !isDigit(ch) && ch !== separator) {
        return 'invalid-char'
      }
    }
  }

  return 'success'
}

const validateVersionNameUtil = (versionName: string): boolean => {
  // A simple regex to validate semantic versioning (e.g., 1.0.0, 1.2.3)
  const semverRegex = /^\d+\.\d+\.\d+$/
  return semverRegex.test(versionName)
}

/**
 * Sanitizes a bundle ID to ensure it follows valid naming conventions.
 *
 * This function processes each segment of a dot-separated bundle ID by:
 * - Converting leading digits (0-9) to their corresponding English words
 * - Replacing hyphens with underscores throughout the entire bundle ID
 *
 * @param bundleId - The bundle identifier string to sanitize (e.g., "com.example.1app-name")
 * @returns The sanitized bundle ID with valid segment naming (e.g., "com.example.oneapp_name")
 *
 * @example
 * ```typescript
 * sanitizeBundleId("com.example.1app-name") // Returns "com.example.oneapp_name"
 * ```
 */
const sanitizeBundleId = (platform: HtmlShell, bundleId: string): string => bundleId
  .split('.')
  .map(segment => segment.replace(/^[0-9]/, (match) => {
    // Convert Western digits to words to make valid bundle ID segments
    const words = [
      'zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine',
    ]
    return words[parseInt(match, 10)] || match
  }))
  .join('.')
  .replace(/[-_]/g, bundleIdSeparator(platform))

const naeS3Bucket = (name: string) => `8w-us-west-2-nae-builds-lambda-${name}`

const naeS3Key = (
  appUuid: string,
  buildId: string,
  appExtension: string
) => `${appUuid}/${buildId}.${appExtension}`

const naeHistoricalBuildPk = (appUuid: string) => `nae-build:app:${appUuid}`

const makeNaeNameForEnv = (
  name: string, dataRealm: string
) => `nae-lambda-builder-${name}-${dataRealm}`

const naeDdbTableName = (dataRealm: string) => makeNaeNameForEnv(
  'historical-build-table', dataRealm
)

const getIconUrl = (iconId: string, size: number) => (
  `https://cdn.8thwall.com/images/nae/icons/${iconId}-${size}x${size}`
)

const getPreviewIconUrl = (iconId: string) => getIconUrl(iconId, NAE_PREVIEW_ICON_URL_SIZE)

const getRandomIcon = () => NAE_DEFAULT_ICONS[Math.floor(Math.random() * NAE_DEFAULT_ICONS.length)]

const getDownloadedFileName = (
  appName: string, appExtension: string
): string => `${appName}.${appExtension}`

const defaultHistoricalBuildData = (): HistoricalBuildData => ({
  // We do not leave any empty fields b/c SNS will fail with if a field is an empty string:
  //  "The message attribute 'commitId' must contain non-empty message attribute value for message
  //  attribute type 'String'."
  requestUuid: 'UNKNOWN',
  publisher: 'UNKNOWN',
  displayName: 'UNKNOWN',
  bundleId: 'UNKNOWN',
  versionName: 'UNKNOWN',
  versionCode: 0,
  buildRequestTimestampMs: 0,
  buildStartTimestampMs: 0,
  buildEndTimestampMs: 0,
  platform: 'UNKNOWN' as HtmlShell,
  exportType: 'UNKNOWN' as ExportType,
  buildMode: 'UNKNOWN' as NaeBuildMode,
  commitId: 'UNKNOWN',
  refHead: 'UNKNOWN' as RefHead,
  sizeBytes: 0,
  buildId: 'UNKNOWN',
  status: 'FAILED',
  shellVersion: 'UNKNOWN',
  iconId: 'UNKNOWN',
  screenOrientation: 'UNKNOWN' as ScreenOrientation,
  statusBarVisible: false,
})

// Here we update the bundleId and displayName with a suffix depending on the environment, as we
// want users to be able to install multiple versions of the app on their devices. Note that we
// only do this for Android, as we use a different uploadKey for environment (dev vs. staging vs.
// prod). Whereas for iOS, we do not (we only have different signing for local development vs app
// store, not for each environment). So changing the bundleId would break on iOS.
const getBundleIdAndDisplayNameWithSuffix = (
  bundleId: string, displayName: string, platform: HtmlShell, refHead: RefHead
) => {
  if (platform !== 'android') {
    return {bundleId, displayName}
  }
  let bundleIdSuffix = ''
  let appDisplayNameSuffix = ''
  if (refHead === 'staging') {
    bundleIdSuffix = '.staging'
    appDisplayNameSuffix = ' (Staging)'
  } else if (refHead === 'master') {
    bundleIdSuffix = '.latest'
    appDisplayNameSuffix = ' (Latest)'
  } else if (refHead !== 'production') {
    bundleIdSuffix = '.dev'
    appDisplayNameSuffix = ' (Dev)'
  }
  return {
    bundleId: `${bundleId}${bundleIdSuffix}`,
    displayName: `${displayName}${appDisplayNameSuffix}`,
  }
}

// Replace all non-alphanumeric characters and spaces with an underscore.
const cleanupStringForBundleId = (platform: HtmlShell, str: string) => str
  .replace(/[^a-zA-Z0-9]/g, bundleIdSeparator(platform))
  .toLowerCase()

const determineSigningStatus = (
  certificateExpired: boolean,
  profileExpired: boolean
): SigningFilesInfoStatusResult => {
  if (certificateExpired && profileExpired) {
    return 'both-signing-files-expired'
  }
  if (certificateExpired) {
    return 'certificate-expired'
  }
  if (profileExpired) {
    return 'provisioning-profile-expired'
  }
  return 'valid'
}

const createSigningInfoFromSeparateData = (
  config: SigningConfigData | undefined,
  certificateData: CertificateInfo | undefined,
  availableCertificates: CertificateInfo[]
): SigningInfo | undefined => {
  if (!config || !certificateData) {
    return availableCertificates.length > 0 ? {availableCertificates} : undefined
  }
  const certificateExpiration = new Date(certificateData.certificateExpirationDate)
  const provisioningProfileExpiration = new Date(config.provisioningProfileExpirationDate)

  const certificateExpired = Date.now() > certificateExpiration.getTime()
  const profileExpired = Date.now() > provisioningProfileExpiration.getTime()

  return {
    certificateUuid: certificateData.certificateUuid,
    certificateCommonName: certificateData.certificateCommonName,
    certificateExpiration,
    provisioningProfileName: config.provisioningProfileName,
    provisioningProfileExpiration,
    deviceUdids: config.deviceUdids,
    entitlements: config.entitlements,
    status: determineSigningStatus(certificateExpired, profileExpired),
    availableCertificates,
  }
}

const toCertificateInfo = (items: CertificateProjection[]): CertificateInfo[] => items.map(
  item => ({
    certificateUuid: item.sk,
    certificateCommonName: item.certificateCommonName,
    certificateExpirationDate: item.certificateExpirationDate,
    signingType: item.signingType,
  })
)

const toAuthKeyInfo = (authKeyData: UploadAuthKeyData): AuthKeyInfo => ({
  keyId: authKeyData.keyId,
  issuerId: authKeyData.issuerId,
  keyFileName: authKeyData.authKeyFileName,
})

const getSigningConfigs = async (
  ddbClient: Partial<DynamoDbApi>,
  tableName: string,
  appUuid: string
): Promise<SigningConfigsResult> => {
  if (!ddbClient.query) {
    throw new Error('[nae-utils] DynamoDb client does not support query operation')
  }

  const result = await ddbClient.query({
    TableName: tableName,
    KeyConditionExpression: 'pk = :pk',
    ExpressionAttributeValues: {
      ':pk': {S: appUuid},
    },
  })

  const configs = result.Items
    ? result.Items.map((item: AttributesForRaw<SigningConfigProjection>) => fromAttributes(item))
    : []

  let developmentConfig: SigningConfigData | undefined
  let distributionConfig: SigningConfigData | undefined

  if (result.Items) {
    result.Items.forEach((item: AttributesForRaw<SigningConfigProjection>, index: number) => {
      const signingType: AppleSigningType = item.sk.S
      const config = configs[index]

      if (signingType === 'development') {
        if (developmentConfig) {
          // eslint-disable-next-line no-console
          console.error(`Warning: Duplicate development signing files found for pk: ${item.pk}`)
        }
        developmentConfig = config
      } else if (signingType === 'distribution') {
        if (distributionConfig) {
          // eslint-disable-next-line no-console
          console.error(`Warning: Duplicate distribution signing files found for pk: ${item.pk}`)
        }
        distributionConfig = config
      }
    })
  }

  return {developmentConfig, distributionConfig}
}

const getCertificatesForAccount = async (
  ddbClient: Partial<DynamoDbApi>,
  tableName: string,
  accountUuid: string
): Promise<CertificateInfo[]> => {
  if (!ddbClient.query) {
    throw new Error('[nae-utils] DynamoDb client does not support query operation')
  }

  const result = await ddbClient.query({
    TableName: tableName,
    KeyConditionExpression: 'pk = :pk',
    ExpressionAttributeValues: {
      ':pk': {S: accountUuid},
    },
    ProjectionExpression: 'sk, certificateCommonName, certificateExpirationDate, signingType',
  })

  const certItems = result.Items
    ? result.Items.map((item: AttributesForRaw<CertificateProjection>) => fromAttributes(item))
    : []

  return toCertificateInfo(certItems)
}

const getAuthKeyInfo = async (
  ddbClient: Partial<DynamoDbApi>,
  tableName: string,
  appUuid: string
): Promise<AuthKeyInfo | undefined> => {
  if (!ddbClient.query) {
    throw new Error('[nae-utils] DynamoDb client does not support query operation')
  }

  const result = await ddbClient.query({
    TableName: tableName,
    KeyConditionExpression: 'pk = :pk',
    ExpressionAttributeValues: {
      ':pk': {S: appUuid},
    },
    ProjectionExpression: 'keyId, issuerId, authKeyFileName',
    Limit: 1,
  })

  const authKeyItems = result.Items as AttributesForRaw<UploadAuthKeyData>[]
  const authKeyData = authKeyItems?.[0] ? fromAttributes(authKeyItems[0]) : undefined
  return authKeyData ? toAuthKeyInfo(authKeyData) : undefined
}

const createSigningInfoFromConfig = async (
  config: SigningConfigData | undefined,
  signingType: AppleSigningType,
  availableCertificates: CertificateInfo[]
): Promise<SigningInfo> => {
  let certData
  if (config) {
    certData = availableCertificates.find(
      cert => cert.certificateUuid === config.certificateUuid
    )
    if (!certData) {
      // eslint-disable-next-line no-console
      console.error(`[nae-utils] Unexpected missing certificate uuid: ${config.certificateUuid}`)
      return {availableCertificates}
    }
  }

  const availableCertsForType = availableCertificates.filter(
    cert => cert.signingType === signingType
  )

  return createSigningInfoFromSeparateData(config, certData, availableCertsForType)
}

const makeNaeCreditTopicAttributes = (
  requestUuid: string,
  completed: boolean
) => {
  const attributes = {
    feature: {DataType: 'String', StringValue: 'NAE'},
    requestId: {DataType: 'String', StringValue: requestUuid},
  }
  if (completed === true) {
    Object.assign(attributes, {
      completed: {DataType: 'String', StringValue: 'true'},
    })
  } else if (completed === false) {
    Object.assign(attributes, {
      completed: {DataType: 'String', StringValue: 'false'},
    })
  }
  // Undefined completed has a different meaning than false for credit-notifications.
  return attributes
}

const platformToLabel = (platform: HtmlShell): string => {
  switch (platform) {
    case 'android':
      return 'Android'
    case 'ios':
      return 'iOS'
    case 'quest':
      return 'Quest'
    case 'osx':
      return 'OSX'
    case 'html':
      return 'HTML5'
    default:
      return platform
  }
}

const createAssetCarContentsJson = () => ({
  images: [
    {
      filename: '1024.png',
      idiom: 'universal',
      platform: 'ios',
      size: '1024x1024',
    },
    {
      appearances: [
        {appearance: 'luminosity', value: 'dark'},
      ],
      filename: '1024.png',
      idiom: 'universal',
      platform: 'ios',
      size: '1024x1024',
    },
    {
      appearances: [
        {appearance: 'luminosity', value: 'tinted'},
      ],
      filename: '1024.png',
      idiom: 'universal',
      platform: 'ios',
      size: '1024x1024',
    },
  ],
  info: {
    author: 'xcode',
    version: 1,
  },
})

export {
  getCsrPem,
  validateAppName,
  validateAppDisplayName,
  validateBuildMode,
  validateBundleIdUtil,
  validateExportType,
  validateVersionNameUtil,
  naeHistoricalBuildPk,
  naeS3Bucket,
  naeS3Key,
  sanitizeBundleId,
  makeNaeCreditTopicAttributes,
  makeNaeNameForEnv,
  naeDdbTableName,
  getRandomIcon,
  getPreviewIconUrl,
  getIconUrl,
  getDownloadedFileName,
  defaultHistoricalBuildData,
  getBundleIdAndDisplayNameWithSuffix,
  cleanupStringForBundleId,
  platformToLabel,
  getSigningConfigs,
  getCertificatesForAccount,
  getAuthKeyInfo,
  createSigningInfoFromConfig,
  createSigningInfoFromSeparateData,
  validatePermissionUsageDescriptionUtil,
  NAE_PERMISSION_USAGE_DESCRIPTION_MAX_CHARACTERS,
  createAssetCarContentsJson,
}
