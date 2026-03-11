import type {TFunction} from 'react-i18next'

import type {
  HtmlShell,
  AppleSigningValidationResult,
  CsrUploadError,
} from '../../../../shared/nae/nae-types'
import {
  validateBundleIdUtil,
  validateVersionNameUtil,
  validateAppDisplayName as validateAppDisplayNameUtil,
  getBundleIdAndDisplayNameWithSuffix,
  validatePermissionUsageDescriptionUtil,
} from '../../../../shared/nae/nae-utils'

type ValidationResult = {
  // Whether the validation succeeded.
  success: boolean
  // The UI we should show to the user. Null to show nothing.
  uiResult?: {
    status: 'danger' | 'warning'
    message: string
  }
}

const validateAppDisplayName = (
  appDisplayName: string,
  platform: HtmlShell,
  t: TFunction<'cloud-editor-pages'[]>
): ValidationResult => {
  const {displayName: appDisplayNameWithSuffix} = getBundleIdAndDisplayNameWithSuffix(
    '',
    appDisplayName,
    platform,
    // Note that we hard-code to staging here b/c it adds the longest suffix. And we need to make
    // sure that this name will work with any suffix which reality/app/nae/build.ts adds.
    'staging'
  )
  const result = validateAppDisplayNameUtil(appDisplayNameWithSuffix)

  if (result === 'success') {
    return {success: true}
  }
  if (result === 'empty') {
    return {success: false}
  }

  const validationResult: ValidationResult = {success: false}
  switch (result) {
    case 'invalid-char':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.app_display_name.invalid_char'),
      }
      break
    case 'char-needs-escape':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.app_display_name.char_needs_escape'),
      }
      break
    case 'unicode-char':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.app_display_name.unicode_char'),
      }
      break
    case 'leading-or-trailing-whitespace':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.app_display_name.leading_trailing_whitespace'),
      }
      break
    case 'too-long':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.app_display_name.invalid_length'),
      }
      break
    default:
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.app_display_name.invalid'),
      }
      break
  }
  return validationResult
}

const validateBundleId = (
  bundleId: string,
  platform: HtmlShell,
  t: TFunction<'cloud-editor-pages'[]>
): ValidationResult => {
  const {bundleId: bundleIdWithSuffix} = getBundleIdAndDisplayNameWithSuffix(
    bundleId,
    '',
    platform,
    // Note that we hard-code to staging here b/c it adds the longest suffix. And we need to make
    // sure that this name will work with any suffix which reality/app/nae/build.ts adds.
    'staging'
  )
  const result = validateBundleIdUtil(platform, bundleIdWithSuffix)

  if (result === 'success') {
    return {success: true}
  }
  if (result === 'empty') {
    return {success: false}
  }

  const validationResult: ValidationResult = {success: false}
  switch (result) {
    case 'too-few-parts':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.bundle_id.too_few_parts'),
      }
      break
    case 'empty-part':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.bundle_id.empty_part'),
      }
      break
    case 'first-char-of-part-not-letter':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.bundle_id.invalid_first_char'),
      }
      break
    case 'invalid-char':
      validationResult.uiResult = {
        status: 'danger',
        message: t((platform === 'android' || platform === 'quest')
          ? 'editor_page.export_modal.android_bundle_id.invalid_char'
          : 'editor_page.export_modal.apple_bundle_id.invalid_char'),
      }
      break
    case 'too-long':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.bundle_id.too_long'),
      }
      break
    default:
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.bundle_id.invalid'),
      }
      break
  }
  return validationResult
}

const validateVersionName = (
  versionName: string,
  t: TFunction<'cloud-editor-pages'[]>
): ValidationResult => {
  const result = validateVersionNameUtil(versionName)

  if (result) {
    return {success: true}
  }

  const validationResult: ValidationResult = {success: false}
  validationResult.uiResult = {
    status: 'danger',
    message: t('editor_page.export_modal.android_version_name.invalid'),
  }
  return validationResult
}

const validatePermissionUsageDescription = (
  usageDescription: string,
  t: TFunction<'cloud-editor-pages'[]>
): ValidationResult => {
  const result = validatePermissionUsageDescriptionUtil(usageDescription)

  if (result === 'success') {
    return {success: true}
  }

  const validationResult: ValidationResult = {success: false}
  switch (result) {
    case 'invalid-char':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.permission_usage_description.invalid_char'),
      }
      break
    case 'char-needs-escape':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.permission_usage_description.char_needs_escape'),
      }
      break
    case 'unicode-char':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.permission_usage_description.unicode_char'),
      }
      break
    case 'leading-or-trailing-whitespace':
      validationResult.uiResult = {
        status: 'danger',
        message: t(
          'editor_page.export_modal.permission_usage_description.leading_trailing_whitespace'
        ),
      }
      break
    case 'too-long':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.permission_usage_description.invalid_length'),
      }
      break
    case 'missing-letter-or-digit':
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.permission_usage_description.missing_letter_or_digit'),
      }
      break
    default:
      validationResult.uiResult = {
        status: 'danger',
        message: t('editor_page.export_modal.permission_usage_description.invalid'),
      }
      break
  }
  return validationResult
}

const translateAppleSigningError = (
  error: AppleSigningValidationResult | CsrUploadError,
  t: TFunction<'cloud-editor-pages'[]>
): string => {
  switch (error) {
    case 'invalid-certificate':
      return t('editor_page.export_modal.apple_signing_error.invalid_certificate')
    case 'invalid-provisioning-profile':
      return t('editor_page.export_modal.apple_signing_error.invalid_provisioning_profile')
    case 'certificate-expired':
      return t('editor_page.export_modal.apple_signing_error.certificate_expired')
    case 'provisioning-profile-expired':
      return t('editor_page.export_modal.apple_signing_error.provisioning_profile_expired')
    case 'certificate-not-in-profile':
      return t('editor_page.export_modal.apple_signing_error.certificate_not_in_profile')
    case 'files-signing-type-mismatch':
      return t('editor_page.export_modal.apple_signing_error.files_signing_type_mismatch')
    case 'request-signing-type-mismatch':
      return t('editor_page.export_modal.apple_signing_error.request_signing_type_mismatch')
    case 'bundle-id-mismatch':
      return t('editor_page.export_modal.apple_signing_error.bundle_id_mismatch')
    case 'certificate-csr-mismatch':
      return t('editor_page.export_modal.apple_signing_error.certificate_csr_mismatch')
    case 'p12-generation-failed':
      return t('editor_page.export_modal.apple_signing_error.p12_generation_failed')
    case 'signing-files-not-found':
      return t('editor_page.export_modal.apple_signing_error.signing_files_not_found')
    case 'csr-not-found':
      return t('editor_page.export_modal.apple_signing_error.csr_not_found')
    case 'request-body-missing-fields':
      return t('editor_page.export_modal.apple_signing_error.request_body_missing_fields')
    case 'unexpected-error-csr-generation':
      return t('editor_page.export_modal.apple_signing_error.unexpected_error_csr_generation')
    case 'unexpected-error-csr-validation':
      return t('editor_page.export_modal.apple_signing_error.unexpected_error_csr_validation')
    case 'unexpected-null-certificate-or-profile':
      return t(
        'editor_page.export_modal.apple_signing_error.unexpected_null_certificate_or_profile'
      )
    case 'unexpected-null-provisioning-profile-expiration':
      return t(
        'editor_page.export_modal.apple_signing_error.' +
        'unexpected_null_provisioning_profile_expiration'
      )
    case 'invalid-request-signing-type':
      return t('editor_page.export_modal.apple_signing_error.invalid_request_signing_type')
    case 'unexpected-null-p12-or-password':
      return t('editor_page.export_modal.apple_signing_error.unexpected_null_p12_or_password')
    case 'request-invalid-content-type':
      return t('editor_page.export_modal.apple_signing_error.request_invalid_content_type')
    case 'unexpected-multiple-values-for-field':
      return t('editor_page.export_modal.apple_signing_error.unexpected_multiple_values_for_field')
    case 'missing-body-fields':
      return t('editor_page.export_modal.apple_signing_error.missing_body_fields')
    default:
      return t('editor_page.export_modal.apple_signing_error.unknown_error')
  }
}

const processUploadError = (
  error: any, t: TFunction<'cloud-editor-pages'[]>
): string[] => {
  const errors: string[] = []
  try {
    const {validationErrors} = JSON.parse(error.message)
    if (validationErrors?.length > 0) {
      const translatedErrors = validationErrors.map(e => translateAppleSigningError(e, t))
      errors.push(...translatedErrors)
    } else {
      errors.push(error.message)
    }
  } catch {
    errors.push(error.message)
  }
  return errors
}

export {
  type ValidationResult,
  validateAppDisplayName,
  validateBundleId,
  validateVersionName,
  validatePermissionUsageDescription,
  processUploadError,
}
