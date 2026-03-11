import type {
  IOS_AVAILABLE_PERMISSIONS,
  APP_NAE_BUILD_MODES,
  HTML_SHELL_TYPES,
  NAE_ANDROID_EXPORT_TYPES,
  NAE_EXPORT_TYPES,
  NAE_IOS_EXPORT_TYPES,
  SCREEN_ORIENTATIONS,
} from './nae-constants'

type PlistValue = string | boolean | string[]

type BuildStatus = 'SUCCESS' | 'FAILED'

type RefHead = 'staging' | 'production' | 'master' | `${string}-${string}`

type ScreenOrientation = typeof SCREEN_ORIENTATIONS[number]

// This should match https://developer.android.com/guide/topics/manifest/activity-element#screen
type AndroidScreenOrientation =
  | 'portrait'
  | 'reversePortrait'
  | 'landscape'
  | 'reverseLandscape'
  | 'sensor'
  | 'sensorLandscape';

// These represent the values for the iOS Info.plist key `UISupportedInterfaceOrientations`.
// eslint-disable-next-line max-len
// https://developer.apple.com/documentation/bundleresources/information-property-list/uisupportedinterfaceorientations
type IosScreenOrientation =
  | 'UIInterfaceOrientationPortrait'
  | 'UIInterfaceOrientationPortraitUpsideDown'
  | 'UIInterfaceOrientationLandscapeLeft'
  | 'UIInterfaceOrientationLandscapeRight';

type DevCookie = {
  name: string
  token: string
  options: {
    httpOnly: boolean
    signed: boolean
    expires: Date
    domain: string
    path: string
    secure: boolean
    sameSite: 'none' | 'lax' | 'strict'
  }
}

type NaeBuildMode = typeof APP_NAE_BUILD_MODES[number]

// NOTE(paris): This is stored in NaeInfos in PostgreSQL so matches the casing used there.
// WARNING: This type is referenced and used globally in DB, see the warning below
type PermissionRequestStatus = 'REQUESTED' | 'NOT_REQUESTED'

// WARNING: This type is referenced and used globally in DB, see the warning below
type IosAvailablePermissions = typeof IOS_AVAILABLE_PERMISSIONS[number]

// WARNING: This type is globally used in DB schema,
// when updating this type, make sure to also update the value of `permissions` in `NaeInfo` in DB.
// Use permissions table in xrhome or update the value manually in DB.
type Permission = {
  requestStatus: PermissionRequestStatus
  usageDescription?: string
}

// There are three concepts of permissions when dealing with a Tauri app:
// 1. The permissions in the native app, i.e. the Info.plist on iOS or the AndroidManifest.xml
//    on Android. These are the permissions that the app tells the OS it may want, and is what is
//    shown to users on app stores when downloading an app.
// 2. The Tauri permissions, i.e. Tauri.config.json. These are the permissions that the Tauri
//    shell requests from the OS. Some of these may require us to have set values in #1 as well
//    (i.e. camera), but others are just a Tauri safety feature (i.e. file system access).
// 3. The WebView permissions, i.e. the runtime permissions that the web app requests from the
//    browser. These are the permissions that the web app may request at runtime, and are
//    independent of the native app permissions, though some may overlap (i.e. camera).
//
// This Permissions type is to define #1, i.e. the permissions we include in the native app.
type Permissions = {
  [K in
    | IosAvailablePermissions
  ]?: Permission
}

type AppInfo = {
  workspace: string
  appName: string
  refHead: RefHead
  naeBuildMode: NaeBuildMode
  bundleIdentifier: string
  versionCode?: number
  versionName: string
  appDisplayName: string
  screenOrientation: ScreenOrientation
  statusBarVisible: boolean
  // Path to a directory containing the app icon or other resources. Will be copied into the app in
  // platform-specific ways.
  resourceDir: string
  permissions?: Permissions
}

type IosExportType = typeof NAE_IOS_EXPORT_TYPES[number]

type AndroidExportType = typeof NAE_ANDROID_EXPORT_TYPES[number]

type ExportType = typeof NAE_EXPORT_TYPES[number]

type AndroidClientConfig = {
  // TODO(paris): This was moved to AppInfo, remove from here once xrhome is updated.
  packageName: string
  // TODO(paris): This was moved to AppInfo, remove from here once xrhome is updated.
  versionCode?: number
  // TODO(paris): This was moved to AppInfo, remove from here once xrhome is updated.
  versionName: string
}

type AndroidConfig = {
  minSdkVersion: number
  targetSdkVersion: number
  ksPath: string
  ksAlias: string
  ksPass: string
  oculusSplashScreen?: string
  manifest: string
} & AndroidClientConfig

type AppleClientConfig = {
  // TODO(paris): This was moved to AppInfo, remove from here once xrhome is updated.
  bundleIdentifier: string
  teamIdentifier: string
  // TODO(paris): This was moved to AppInfo, remove from here once xrhome is updated.
  versionCode?: number
  // TODO(paris): This was moved to AppInfo, remove from here once xrhome is updated.
  versionName: string
  signingType: AppleSigningType
}

type AppleConfig = {
  certificate: string
  provisioningProfile: string
  minOsVersion: string

  // NOTE(lreyna): These are used for rust codesign
  p12FilePath: string
  p12Password: string
} & AppleClientConfig

type HtmlShell = typeof HTML_SHELL_TYPES[number]

type HtmlAppConfig = {
  projectUrl: string
  shell: HtmlShell
  android?: AndroidConfig
  apple?: AppleConfig
  niaEnvAccessCode?: string
  devCookie?: DevCookie
  appInfo?: AppInfo
  additionalItems?: string[]
}

/**
 * Represents a request to build a NAE (Native App Export) application.
 */
interface NaeBuilderRequest {
  shellVersion?: string
  // When testing different shell versions, we might want to force the Lambda to delete the existing
  // shell version from EFS and unarchive it again. Set to true to do that.
  removeExistingShellVersion?: boolean
  requestUuid: string
  projectUrl: string
  shell: HtmlShell
  android?: AndroidClientConfig
  apple?: AppleClientConfig
  appInfo: AppInfo
  appUuid: string
  accountUuid: string
  buildRequestTimestampMs: number
  exportType: ExportType
  iconId?: string
  launchScreenIconId?: string
  repoId: string
  requestRef: RefHead
}

// Note: If changing fields here, please also update the corresponding Athena view
//   reality/cloud/aws/athena/dynamodb/nae_lambda_builder_latest.sql
//   If defining new partition and/or sort keys, consider creating separate views.
type HistoricalBuildData = {
  requestUuid: string
  publisher: string
  displayName: string
  bundleId: string
  versionName: string
  versionCode: number
  buildRequestTimestampMs: number
  buildStartTimestampMs: number
  buildEndTimestampMs: number
  platform: HtmlShell
  exportType: ExportType
  buildMode: NaeBuildMode
  commitId: string
  refHead: RefHead
  sizeBytes: number
  buildId: string
  status: BuildStatus
  shellVersion: string
  iconId: string
  screenOrientation: ScreenOrientation
  statusBarVisible: boolean
}

type BuildNotificationData = HistoricalBuildData & Record<string, unknown> & {
  repoId: string
  repositoryName: string
  branch: string
  errorMessage?: string
}

type UploadSigningFilesData = {
  certificateBase64: string
  p12CertificateBase64: string
  certificateCommonName: string
  p12CertificatePassword: string
  certificateExpirationDate: string
  provisioningProfileBase64: string
  provisioningProfileName: string
  provisioningProfileExpirationDate: string
  teamIdentifier: string
}

type CertificateFileData = {
  cerCertificateBase64: string
  p12CertificateBase64: string
  certificateCommonName: string
  p12CertificatePassword: string
  certificateExpirationDate: string
  certificateSigningRequestBase64: string
  certificateSigningRequestPrivateKeyBase64: string
  signingType: AppleSigningType
}

type SigningConfigData = {
  accountUuid: string
  appUuid: string
  certificateUuid: string
  provisioningProfileBase64: string
  provisioningProfileName: string
  provisioningProfileExpirationDate: string
  teamIdentifier: string
  deviceUdids?: string[]
  entitlements: Record<string, PlistValue>
}

type SigningConfigProjection = SigningConfigData & {
  pk: string
  sk: AppleSigningType
}

type CertificateProjection = {
  sk: string
  certificateCommonName: string
  certificateExpirationDate: string
  signingType: AppleSigningType
}

type SigningConfigsResult = {
  developmentConfig?: SigningConfigData
  distributionConfig?: SigningConfigData
}

type CsrData = {
  certificateSigningRequestBase64: string
  certificateSigningRequestPrivateKeyBase64: string
}

type UploadAuthKeyData = {
  authKeyBase64: string
  keyId: string
  issuerId: string
  authKeyFileName: string
}

type AndroidBundleIdValidationResult =
  | 'success'
  | 'empty'
  | 'too-few-parts'
  | 'empty-part'
  | 'first-char-of-part-not-letter'
  | 'invalid-char'
  | 'too-long';

type AppDisplayNameValidationResult =
  | 'success'
  | 'empty'
  | 'too-long'
  | 'char-needs-escape'
  | 'unicode-char'
  | 'leading-or-trailing-whitespace'
  | 'invalid-char';

type PermissionUsageDescriptionValidationResult =
  | 'success'
  | 'empty'
  | 'too-long'
  | 'char-needs-escape'
  | 'unicode-char'
  | 'leading-or-trailing-whitespace'
  | 'missing-letter-or-digit'
  | 'invalid-char';

type AppleSigningType = 'development' | 'distribution'

type CsrValidationResult =
  | 'valid'
  | 'certificate-csr-mismatch'
  | 'unexpected-error-csr-validation'

type P12GenerationError =
  | 'p12-generation-failed'

type AppleSigningValidationResult =
  | CsrValidationResult
  | P12GenerationError
  | 'invalid-certificate'
  | 'invalid-provisioning-profile'
  | 'certificate-expired'
  | 'provisioning-profile-expired'
  | 'certificate-not-in-profile'
  | 'files-signing-type-mismatch'
  | 'request-signing-type-mismatch'
  | 'bundle-id-mismatch'
  | 'unexpected-null-certificate-or-profile'
  | 'unexpected-null-provisioning-profile-expiration'
  | 'missing-body-fields'
  | 'signing-files-not-found'
  | 'csr-not-found'
  | 'invalid-request-signing-type'
  | 'unexpected-null-p12-or-password'
  | 'request-invalid-content-type'
  | 'unexpected-multiple-values-for-field'
  | 'request-body-missing-fields'

type CsrUploadError =
  | 'request-body-missing-fields'
  | 'unexpected-error-csr-generation'

type SigningFilesInfoStatusResult =
  | 'valid'
  | 'signing-files-not-found'
  | 'certificate-expired'
  | 'provisioning-profile-expired'
  | 'both-signing-files-expired'

type UploadToAppStoreConnectStatusResult =
  | 'success'
  | 'missing-request-body-fields'

type GenerateCsrRequest = {
  appUuid: string
  signingType: AppleSigningType
}

type UploadToAppStoreConnectRequest = {
  appUuid: string
  buildId: string
}

type UploadToAppStoreConnectResponse = {
  status: UploadToAppStoreConnectStatusResult
}

type GenerateCsrResponse = {
  certificateSigningRequestBase64?: string
  certificateSigningRequestPrivateKeyBase64?: string
  errors?: CsrUploadError[]
}

type DownloadCsrResponse = {
  csrPem: string
}

type UploadSigningFilesRequest = {
  appUuid: string
  signingType: AppleSigningType
  certificateBase64: string
  bundleIdentifier: string
  provisioningProfileBase64: string
}

type UploadSigningFilesResponse = {
  validationErrors: AppleSigningValidationResult[]
}

type SigningInfo = {
  certificateUuid?: string
  certificateCommonName?: string
  certificateExpiration?: Date
  provisioningProfileName?: string
  provisioningProfileExpiration?: Date
  deviceUdids?: string[]
  entitlements?: Record<string, PlistValue>
  status?: SigningFilesInfoStatusResult
  availableCertificates?: CertificateInfo[]
}

type CertificateInfo = {
  certificateUuid: string
  certificateCommonName: string
  certificateExpirationDate: string
  signingType: AppleSigningType
}

type AuthKeyInfo = {
  keyId: string
  issuerId: string
  keyFileName: string
}

type SigningFilesInfoResponse = {
  developmentSigningInfo?: SigningInfo
  distributionSigningInfo?: SigningInfo
  authKeyInfo?: AuthKeyInfo
}

type UpdateProvisioningProfileRequest = {
  appUuid: string
  signingType: AppleSigningType
  bundleIdentifier: string
  provisioningProfileBase64: string
}

type UploadAuthKeyResponse = {}

type AuthKeyInfoResponse = {
  keyId: string
  keyFileName: string
  issuerId: string
}

type IconSize = {
  fileName: string
  // All icons are square.
  size: number
}

type CreateAssetCarRequest = {
  appIconName: 'AppIcon'
  platform: 'iphoneos'
  contentsJson: string
  iconId: string
  iconSizes: IconSize[]
  minimumDeploymentTarget: string
  verbose?: boolean
}

type CreateAssetCarResponse = {
  assetCarFileBase64?: string
  error?: string
}

type DeleteCertificateResponse = {
  success: boolean
  conflictingAppTitles?: string[]
}

export type {
  AndroidConfig,
  AndroidClientConfig,
  AndroidExportType,
  AndroidScreenOrientation,
  AppInfo,
  AppleConfig,
  AppleClientConfig,
  DevCookie,
  ExportType,
  HtmlAppConfig,
  HtmlShell,
  IosAvailablePermissions,
  IosExportType,
  IosScreenOrientation,
  NaeBuilderRequest,
  NaeBuildMode,
  RefHead,
  ScreenOrientation,
  HistoricalBuildData,
  BuildNotificationData,
  BuildStatus,
  AndroidBundleIdValidationResult,
  AppDisplayNameValidationResult,
  Permissions,
  PermissionUsageDescriptionValidationResult,
  AppleSigningType,
  CsrUploadError,
  GenerateCsrRequest,
  GenerateCsrResponse,
  UploadSigningFilesResponse,
  SigningFilesInfoResponse,
  UploadAuthKeyResponse,
  AuthKeyInfoResponse,
  AppleSigningValidationResult,
  UploadSigningFilesRequest,
  UploadSigningFilesData,
  UploadAuthKeyData,
  UpdateProvisioningProfileRequest,
  CsrData,
  CsrValidationResult,
  P12GenerationError,
  DownloadCsrResponse,
  SigningConfigProjection,
  SigningInfo,
  SigningFilesInfoStatusResult,
  AuthKeyInfo,
  CertificateFileData,
  CertificateInfo,
  SigningConfigData,
  CertificateProjection,
  SigningConfigsResult,
  CreateAssetCarRequest,
  CreateAssetCarResponse,
  IconSize,
  PlistValue,
  DeleteCertificateResponse,
  UploadToAppStoreConnectRequest,
  UploadToAppStoreConnectResponse,
}
