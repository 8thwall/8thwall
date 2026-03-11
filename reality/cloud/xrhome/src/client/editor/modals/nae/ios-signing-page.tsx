import React from 'react'
import {useTranslation} from 'react-i18next'
import {useDispatch} from 'react-redux'

import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import type {Steps} from './export-flow'
import {RowTextField} from '../../../studio/configuration/row-text-field'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {TextButton} from '../../../ui/components/text-button'
import {FloatingMenuButton} from '../../../ui/components/floating-menu-button'
import type {
  AppleSigningType,
  SigningFilesInfoResponse,
  SigningInfo,
  UploadSigningFilesResponse,
} from '../../../../shared/nae/nae-types'
import {getCsrPem} from '../../../../shared/nae/nae-utils'
import naeActions from '../../../studio/actions/nae-actions'
import useActions from '../../../common/use-actions'
import useCurrentApp from '../../../common/use-current-app'
import {RowUploadDrop} from '../../../studio/configuration/row-upload-drop'
import {Icon} from '../../../ui/components/icon'
import {useStudioMenuStyles} from '../../../studio/ui/studio-menu-styles'
import {useStyles as useNaeModalsStyles} from './nae-modals-styles'
import {createThemedStyles} from '../../../ui/theme'
import {IconButton} from '../../../ui/components/icon-button'
import {useStyles as useRowStyles} from '../../../studio/configuration/row-styles'
import Accordion from '../../../widgets/accordion/accordion'
import {gray3, gray4} from '../../../static/styles/settings'
import {combine} from '../../../common/styles'
import {TooltipIcon} from '../../../widgets/tooltip-icon'
import {TextNotification} from '../../../ui/components/text-notification'
import ColoredMessage from '../../../messages/colored-message'
import {validateBundleId, processUploadError} from './validation'
import {StandardFieldContainer} from '../../../ui/components/standard-field-container'
import {CertificateOption} from './certificate-option'
import {SelectMenu} from '../../../studio/ui/select-menu'
import {StandardLink} from '../../../ui/components/standard-link'

const dropInstructions = '& .drop-instructions'
const accordionRow = '& + .accordion-row'
const before = '&::before'
const after = '&::after'
const lastStep = '&:last-child'
const button = '& > div > button'

const useStyles = createThemedStyles(theme => ({
  menuButton: {
    'display': 'flex',
    'justifyContent': 'space-between',
    'alignItems': 'center',
    'width': '100%',
    'wordBreak': 'break-word',
    '& svg': {
      'minWidth': '16px',
      'color': theme.fgMuted,
      '&:hover': {
        color: theme.fgMain,
      },
    },
  },
  selectMenuNewButton: {
    'color': theme.linkBtnFg,
    'fontWeight': 500,
  },
  selectMenuPortal: {
    zIndex: 3000,
    position: 'relative',
  },
  leading12: {
    fontSize: '12px',
  },
  page: {
    display: 'flex',
    flexDirection: 'column',
    gap: '2.5rem',
  },
  gap05: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5rem',
  },
  gap075: {
    display: 'flex',
    gap: '0.75rem',
  },
  dropContent: {
    display: 'flex',
    gap: '0.5rem',
  },
  uploadIcon: {
    display: 'flex',
    alignItems: 'center',
    fontSize: '12px',
    gap: '0.375rem',
    padding: '0.25rem 0.375rem',
    color: theme.linkBtnFg,
  },
  uploadDrop: {
    border: 'none',
    padding: '0.5rem',
    fontSize: '12px',
    [dropInstructions]: {
      marginTop: '0',
    },
  },
  uploadedFile: {
    border: `1px solid ${theme.publishModalText}`,
    borderRadius: '2px',
    display: 'flex',
    width: 'fit-content',
    paddingLeft: '0.5rem',
    fontSize: '12px',
    paddingRight: '0.5rem',
    color: `${theme.publishModalText} !important`,
  },
  fileName: {
    textOverflow: 'ellipsis',
    flexGrow: 1,
    paddingRight: '0.5rem',
  },
  accordion: {
    backgroundColor: 'transparent !important',
    borderRadius: '0.5rem',
    border: `1px solid ${theme.subtleBorder} !important`,
    padding: '0.75rem 1rem !important',
    [accordionRow]: {
      marginTop: '0 !important',
    },
  },
  accordionTitle: {
    color: theme.publishModalText,
    fontSize: '12px',
    fontWeight: 600,
    lineHeight: '16px',
  },
  textXs: {
    fontSize: '12px',
  },
  textGray: {
    color: gray4,
  },
  italic: {
    fontStyle: 'italic',
  },
  textMuted: {
    color: theme.fgMuted,
  },
  disabled: {
    color: gray3,
    opacity: 0.5,
    pointerEvents: 'none',
    [button]: {
      // Allow buttons (like cancel button) inside disabled elements to remain clickable
      pointerEvents: 'auto',
      opacity: 1,
    },
  },
  textCenter: {
    display: 'flex',
    alignItems: 'center',
  },
  flexEnd: {
    display: 'flex',
    justifyContent: 'flex-end',
  },
  flexCol: {
    display: 'flex',
    flexDirection: 'column',
  },
  errorContainer: {
    padding: '1rem 0',
  },
  gap15: {
    gap: '1.5rem',
  },
  fileExpiration: {
    fontSize: '12px',
    marginLeft: '0.5rem',
  },
  progressContainer: {
    position: 'relative',
    paddingLeft: '1.25rem',
  },
  divider: {
    position: 'relative',
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    [after]: {
      content: '""',
      flex: 1,
      height: '1px',
      backgroundColor: theme.modalContainerBg,
    },
  },
  stepWithLine: {
    position: 'relative',
    [before]: {
      content: '""',
      position: 'absolute',
      left: '-1.5rem',
      top: '0.375rem',
      width: '0.75rem',
      height: '0.75rem',
      borderRadius: '50%',
      border: `0.1875rem solid ${theme.publishModalText}`,
      backgroundColor: 'transparent',
      zIndex: 2,
    },
    [after]: {
      content: '""',
      position: 'absolute',
      // NOTE(xiaokai): offset the left distance with the half width of the border
      left: 'calc(-1.125rem - 0.5px)',
      top: '1rem',
      bottom: '-2rem',
      border: `0.5px dashed ${theme.subtleBorder}`,
      zIndex: 1,
    },
    [lastStep]: {
      [after]: {
        display: 'none',
      },
    },
  },
  errorList: {
    margin: '0.5rem 0 0 0',
    paddingLeft: '1.5rem',
  },
  errorItem: {
    marginBottom: '0.25rem',
  },
  tooltipContent: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5rem',
    maxWidth: '18.75rem',
    minWidth: '12.5rem',
    padding: '0.5rem',
  },
  tooltipSection: {
    display: 'flex',
    flexDirection: 'column',
  },
  tooltipLabel: {
    fontWeight: 'bold',
    marginBottom: '0.25rem',
  },
  tooltipValue: {
    marginBottom: '0',
    fontSize: '0.75rem',
    wordBreak: 'break-word',
  },
  tooltipScrollContainer: {
    maxHeight: '4.5rem',  // Show ~3 lines at 1.5rem each
    overflowY: 'auto',
    fontSize: '0.6875rem',
    lineHeight: '1.25rem',
    cursor: 'default',
    userSelect: 'text',
  },
  tooltipScrollItem: {
    padding: '0.125rem 0',
  },
  cancelButton: {
    background: 'none',
    border: 'none',
    cursor: 'pointer',
    textDecoration: 'underline',
  },
}))

type NewCertificate = {
  type: 'new-certificate'
  certificate: File
  provisioningProfile: File
  csrBase64: string
  csrPrivateKeyBase64: string
}

type UploadExistingCertificate = {
  type: 'upload-existing-certificate'
  p12Certificate: File
  p12Password: string
  provisioningProfile: File
}

type UpdateProvisioningProfile = {
  type: 'update-provisioning-profile'
  certificateUuid: string
  provisioningProfile: File
}

type NaeSaveData = NewCertificate | UploadExistingCertificate | UpdateProvisioningProfile

const getSaveData = (
  certificate: File | null,
  provisioningProfile: File | null,
  certificateSelection: string,
  csrBase64: string | null,
  csrPrivateKeyBase64: string | null,
  p12Certificate: File | null,
  p12Password: string,
  showUploadExistingCertificate: boolean
): NaeSaveData | null => {
  if (!provisioningProfile) {
    return null
  }
  if (showUploadExistingCertificate && p12Certificate && p12Password) {
    return {
      type: 'upload-existing-certificate',
      p12Certificate,
      p12Password,
      provisioningProfile,
    }
  }
  if (certificate && csrBase64 && csrPrivateKeyBase64) {
    return {
      type: 'new-certificate',
      certificate,
      provisioningProfile,
      csrBase64,
      csrPrivateKeyBase64,
    }
  }
  if (certificateSelection) {
    return {
      type: 'update-provisioning-profile',
      certificateUuid: certificateSelection,
      provisioningProfile,
    }
  }
  return null
}

interface INaeRowUploadDrop {
  file: File | null
  onDrop: (file: File | null) => void
  label: string
  fileAccept: string
  disabled?: boolean
}

const NaeRowUploadDrop: React.FC<INaeRowUploadDrop> = ({
  file,
  onDrop,
  label,
  fileAccept,
  disabled = false,
}) => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useStyles()

  const renderFileContent = () => {
    if (file) {
      return (
        <div className={classes.uploadedFile}>
          <div className={combine(classes.fileName, classes.textCenter)}>
            {file.name}
          </div>
          <IconButton
            text={t('button.close')}
            stroke='close'
            size={0.625}
            onClick={(event) => {
              event.preventDefault()
              event.stopPropagation()
              onDrop(null)
            }}
          />
        </div>
      )
    }

    return (
      <div
        className={combine(
          classes.italic,
          classes.textMuted,
          classes.textCenter
        )}
      >
        {t('editor_page.export_modal.click_or_drag_and_drop_to_upload_file')}
      </div>
    )
  }

  return (
    <RowUploadDrop
      id={label}
      label={label}
      onDrop={onDrop}
      disabled={disabled}
      dropMessage={label}
      dropContent={(
        <div className={combine(classes.dropContent, disabled && classes.disabled)}>
          <div className={classes.uploadIcon}>
            <Icon
              stroke='upload'
              size={0.75}
            />
            {t('editor_page.export_modal.upload')}
          </div>
          {renderFileContent()}
        </div>
      )}
      dropClassName={classes.uploadDrop}
      fileAccept={fileAccept}
    />
  )
}

interface IDeleteSigningButton {
  signingType: AppleSigningType
  hasExistingConfig: boolean
  isPendingDeletion: boolean
  onDeleteClick: (signingType: AppleSigningType) => void
}

const DeleteSigningButton: React.FC<IDeleteSigningButton> = ({
  signingType,
  hasExistingConfig,
  isPendingDeletion,
  onDeleteClick,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-editor-pages'])

  if (!hasExistingConfig) {
    return null
  }

  if (isPendingDeletion) {
    return (
      <button
        type='button'
        className={combine(classes.textGray, classes.textXs, classes.cancelButton)}
        onClick={(e) => {
          e.stopPropagation()
          onDeleteClick(signingType)
        }}
      >
        {t('editor_page.export_modal.pending_deletion_cancel')}
      </button>
    )
  }

  return (
    <IconButton
      text={t('button.delete')}
      stroke='delete12'
      size={0.875}
      onClick={(e) => {
        e.stopPropagation()
        onDeleteClick(signingType)
      }}
      a8='click;cloud-editor-export-flow;delete-signing-config'
    />
  )
}

interface ISigningContent {
  certificate?: File
  onChangeCertificate: (file?: File) => void
  provisioningProfile?: File
  isUpdatingProvisioningProfile: boolean
  setIsUpdatingProvisioningProfile: React.Dispatch<React.SetStateAction<boolean>>
  onChangeProvisioningProfile: (file?: File) => void
  signingType: AppleSigningType
  existingSigningInfo?: SigningInfo
  selectedCertificateUuid: string
  onCertificateSelectionChange: (certificateUuid: string) => void
  showAddNewCertificate: boolean
  setShowAddNewCertificate: React.Dispatch<React.SetStateAction<boolean>>
  showUploadExistingCertificate: boolean
  setShowUploadExistingCertificate: React.Dispatch<React.SetStateAction<boolean>>
  p12Certificate: File | null
  onChangeP12Certificate: (file: File | null) => void
  p12Password: string
  onChangeP12Password: (password: string) => void
  csrBase64: string | null
  setCsrBase64: React.Dispatch<React.SetStateAction<string | null>>
  setCsrPrivateKeyBase64: React.Dispatch<React.SetStateAction<string | null>>
  setSigningErrors: React.Dispatch<React.SetStateAction<string[] | null>>
  onRefreshSigningInfo: () => void
}

const SigningContent: React.FC<ISigningContent> = ({
  certificate,
  onChangeCertificate,
  provisioningProfile,
  isUpdatingProvisioningProfile,
  setIsUpdatingProvisioningProfile,
  onChangeProvisioningProfile,
  signingType,
  existingSigningInfo,
  selectedCertificateUuid,
  onCertificateSelectionChange,
  showAddNewCertificate,
  setShowAddNewCertificate,
  showUploadExistingCertificate,
  setShowUploadExistingCertificate,
  p12Certificate,
  onChangeP12Certificate,
  p12Password,
  onChangeP12Password,
  csrBase64,
  setCsrBase64,
  setCsrPrivateKeyBase64,
  setSigningErrors,
  onRefreshSigningInfo,
}) => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const naeModalsStyles = useNaeModalsStyles()
  const dispatch = useDispatch()
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()
  const rowStyles = useRowStyles()
  const app = useCurrentApp()
  const {generateCsr} = useActions(naeActions)

  const [isGeneratingCsr, setIsGeneratingCsr] = React.useState(false)

  // Get available certificates for this signing type
  const availableCertificates = existingSigningInfo?.availableCertificates || []

  // Find selected certificate
  const selectedCertificate = availableCertificates.find(
    cert => cert.certificateUuid === selectedCertificateUuid
  )

  const selectedCertificateDisplayName = selectedCertificate
    ? t('editor_page.export_modal.certificate_option', {
      certificateName: selectedCertificate.certificateCommonName,
      date: new Date(selectedCertificate.certificateExpirationDate).toLocaleDateString(),
    })
    : null

  const handleCertificateChange = (value: string) => {
    onCertificateSelectionChange(value)
    setShowUploadExistingCertificate(false)
    setShowAddNewCertificate(false)
    // Clear provisioning profile since we changed certificate
    onChangeProvisioningProfile(null)
  }

  const handleDownloadCsr = async (csrData: string) => {
    if (csrData) {
      // Convert base64 to PEM format
      const csrPem = getCsrPem(csrData)

      const blob = new Blob([csrPem], {type: 'application/pkcs10'})
      const url = window.URL.createObjectURL(blob)
      const a = document.createElement('a')
      a.href = url
      a.download = `${app.appName}_${signingType}_certificate_request.csr`
      document.body.appendChild(a)
      a.click()
      document.body.removeChild(a)
      window.URL.revokeObjectURL(url)
    } else {
      dispatch({
        type: 'ERROR',
        msg: t('editor_page.export_modal.download_certificate_signing_request_failed'),
      })
    }
  }

  const handleGenerateCsr = async () => {
    setIsGeneratingCsr(true)
    const {
      certificateSigningRequestBase64,
      certificateSigningRequestPrivateKeyBase64,
    } = await generateCsr({
      appUuid: app.uuid,
      signingType,
    })

    setCsrBase64(certificateSigningRequestBase64)
    setCsrPrivateKeyBase64(certificateSigningRequestPrivateKeyBase64)
    handleDownloadCsr(certificateSigningRequestBase64)
    setIsGeneratingCsr(false)
  }

  const getExpirationText = (expiration: Date) => {
    const now = new Date()
    const expirationDate = new Date(expiration)
    const isExpired = expirationDate < now
    const millisecondsPerDay = 1000 * 60 * 60 * 24
    const daysUntilExpiry =
      Math.ceil((expirationDate.getTime() - now.getTime()) / millisecondsPerDay)

    if (isExpired) {
      return t('editor_page.export_modal.signing_file_expired_notice', {
        days: Math.abs(daysUntilExpiry),
      })
    } else if (daysUntilExpiry <= 30) {
      return t('editor_page.export_modal.signing_file_expiration_warning', {
        days: daysUntilExpiry,
      })
    } else {
      return t('editor_page.export_modal.signing_file_expiration_date', {
        date: expirationDate.toLocaleDateString(),
      })
    }
  }

  const trigger = (
    <button
      type='button'
      className={combine(
        'style-reset', rowStyles.select, rowStyles.preventOverflow
      )}
    >
      <div className={rowStyles.selectText}>
        {selectedCertificateDisplayName || t('editor_page.export_modal.choose_certificate')}
      </div>
      <div className={rowStyles.chevron} />
    </button>
  )

  const updatingProvisioningProfile = showAddNewCertificate ||
    showUploadExistingCertificate ||
    !existingSigningInfo ||
    (selectedCertificateUuid !== existingSigningInfo.certificateUuid) ||
    isUpdatingProvisioningProfile

  return (
    <Accordion.Content className={combine(classes.gap15, classes.flexCol)}>
      <div className={combine(classes.progressContainer, classes.flexCol, classes.gap15)}>
        {!showAddNewCertificate && !showUploadExistingCertificate && existingSigningInfo
          ? (
            <div className={classes.stepWithLine}>
              <div className={naeModalsStyles.inputGroup}>
                <div className={combine(classes.textMuted, classes.divider)}>
                  {t('editor_page.export_modal.ios_signing.select_certificate')}
                </div>
                <div className={classes.textGray}>
                  {t('editor_page.export_modal.ios_signing.select_certificate_description')}
                </div>
                <StandardFieldContainer>
                  <SelectMenu
                    id={`certificate-${signingType}-select-menu`}
                    disabled={availableCertificates.length === 0}
                    trigger={trigger}
                    menuWrapperClassName={combine(
                      menuStyles.studioMenu, rowStyles.selectMenuContainer, classes.selectMenuPortal
                    )}
                    placement='bottom-start'
                    margin={8}
                    minTriggerWidth
                  >
                    {collapse => (
                      <>
                        {availableCertificates.map(cert => (
                          <CertificateOption
                            key={cert.certificateUuid}
                            certificateUuid={cert.certificateUuid}
                            certificateName={t('editor_page.export_modal.certificate_option', {
                              certificateName: cert.certificateCommonName,
                              date: new Date(cert.certificateExpirationDate).toLocaleDateString(),
                            })}
                            isSelected={selectedCertificateUuid === cert.certificateUuid}
                            onSelect={() => {
                              handleCertificateChange(cert.certificateUuid)
                              collapse()
                            }}
                            onDeleteSuccess={() => {
                              if (selectedCertificateUuid === cert.certificateUuid) {
                                handleCertificateChange('')
                              }
                              onRefreshSigningInfo()
                            }}
                            collapse={collapse}
                          />
                        ))}
                        <div className={menuStyles.divider} />
                        <FloatingMenuButton
                          onClick={() => {
                            setShowAddNewCertificate(true)
                            setShowUploadExistingCertificate(false)
                            onCertificateSelectionChange('')
                            onChangeProvisioningProfile(null)
                            collapse()
                          }}
                        >
                          <div className={combine(classes.menuButton, classes.selectMenuNewButton)}>
                            {t('editor_page.export_modal.ios_signing.add_new_certificate')}
                            <Icon stroke='plus' />
                          </div>
                        </FloatingMenuButton>
                        <FloatingMenuButton
                          onClick={() => {
                            setShowUploadExistingCertificate(true)
                            setShowAddNewCertificate(false)
                            onCertificateSelectionChange('')
                            onChangeProvisioningProfile(null)
                            collapse()
                          }}
                        >
                          <div className={combine(classes.menuButton, classes.selectMenuNewButton)}>
                            {t('editor_page.export_modal.ios_signing.upload_existing_certificate')}
                            <Icon stroke='upload' />
                          </div>
                        </FloatingMenuButton>
                      </>
                    )}
                  </SelectMenu>
                </StandardFieldContainer>
              </div>
            </div>
          )
          : null}

        {((showAddNewCertificate || !existingSigningInfo) && !showUploadExistingCertificate) && (
          <>
            <div className={classes.stepWithLine}>
              <div className={naeModalsStyles.inputGroup}>
                <div className={combine(classes.textMuted, classes.divider)}>
                  {t('editor_page.export_modal.create_certificate_signing_request')}
                </div>
                <div className={classes.textGray}>
                  {!existingSigningInfo
                    ? t('editor_page.export_modal' +
                      '.create_certificate_signing_request_description_with_p12')
                    : t('editor_page.export_modal.create_certificate_signing_request_description')}
                </div>
                <div className={classes.gap075}>
                  <PrimaryButton
                    a8='click;cloud-editor-export-flow;create-csr'
                    color='purple'
                    height='tiny'
                    loading={isGeneratingCsr}
                    disabled={!!csrBase64}
                    onClick={handleGenerateCsr}
                  >
                    {t('editor_page.export_modal.create_certificate_signing_request')}
                  </PrimaryButton>
                  {!existingSigningInfo && (
                    <PrimaryButton
                      a8='click;cloud-editor-export-flow;upload-existing-p12'
                      color='purple'
                      height='tiny'
                      onClick={() => {
                        setShowUploadExistingCertificate(true)
                        setShowAddNewCertificate(false)
                        onCertificateSelectionChange('')
                        onChangeProvisioningProfile(null)
                      }}
                    >
                      {t('editor_page.export_modal.upload_existing_p12_button')}
                    </PrimaryButton>
                  )}
                  {existingSigningInfo &&
                    <PrimaryButton
                      a8='click;cloud-editor-export-flow;cancel-add-certificate'
                      color='secondary'
                      height='tiny'
                      onClick={() => {
                        setSigningErrors(null)
                        onChangeCertificate(null)
                        onChangeProvisioningProfile(null)
                        setCsrBase64(null)
                        setCsrPrivateKeyBase64(null)
                        setShowAddNewCertificate(false)
                        // Restore existing certificate selection if available
                        if (existingSigningInfo.certificateUuid) {
                          onCertificateSelectionChange(existingSigningInfo.certificateUuid)
                        }
                      }}
                    >
                      {t('editor_page.native_publish_modal.cancel')}
                    </PrimaryButton>
                   }
                </div>
                {csrBase64 && (
                  <div className={classes.textGray}>
                    {t('editor_page.export_modal.create_certificate_signing_request_warning')}
                  </div>
                )}
              </div>
            </div>
            <div className={classes.stepWithLine}>
              <div className={naeModalsStyles.inputGroup}>
                <div className={combine(classes.textMuted, classes.divider)}>
                  {t('editor_page.export_modal.upload_certificate')}
                </div>
                <div className={classes.textGray}>
                  {t('editor_page.export_modal.upload_certificate_description')}
                  <StandardLink
                    href='https://developer.apple.com/account/resources/certificates/add'
                    target='_blank'
                  >
                    {' https://developer.apple.com/account/resources/certificates/add. '}
                  </StandardLink>
                  {t('editor_page.export_modal.upload_certificate_description_continued', {
                    /* eslint-disable local-rules/hardcoded-copy */
                    certificateNameFirst: signingType === 'development'
                      ? 'Apple Development'
                      : 'Apple Distribution',
                    certificateNameSecond: signingType === 'development'
                      ? 'iOS App Development'
                      : 'iOS Distribution (App Store Connect and Ad Hoc)',
                    /* eslint-enable local-rules/hardcoded-copy */
                  })}
                </div>
                <NaeRowUploadDrop
                  file={certificate}
                  onDrop={onChangeCertificate}
                  label={t('editor_page.export_modal.certificate')}
                  fileAccept='.cer'
                  disabled={!csrBase64}
                />
              </div>
            </div>
          </>
        )}
        {showUploadExistingCertificate && (
          <>
            <div className={classes.stepWithLine}>
              <div className={naeModalsStyles.inputGroup}>
                <div className={combine(classes.textMuted, classes.divider)}>
                  {t('editor_page.export_modal.upload_existing_certificate')}
                </div>
                <div className={classes.textGray}>
                  {t('editor_page.export_modal.upload_existing_certificate_description')}
                </div>
                <NaeRowUploadDrop
                  file={p12Certificate}
                  onDrop={onChangeP12Certificate}
                  label={t('editor_page.export_modal.p12_certificate')}
                  fileAccept='.p12'
                />
              </div>
            </div>
            <div className={classes.stepWithLine}>
              <div className={naeModalsStyles.inputGroup}>
                <div className={combine(classes.textMuted, classes.divider)}>
                  {t('editor_page.export_modal.enter_p12_password')}
                </div>
                <div className={classes.textGray}>
                  {t('editor_page.export_modal.enter_p12_password_description')}
                </div>
                <RowTextField
                  id={`p12-password-${signingType}`}
                  label={t('editor_page.export_modal.password')}
                  placeholder={t('editor_page.export_modal.p12_certificate_password_placeholder')}
                  value={p12Password}
                  onChange={e => onChangeP12Password(e.target.value)}
                  type='password'
                />
                <div className={classes.flexEnd}>
                  <PrimaryButton
                    a8='click;cloud-editor-export-flow;cancel-upload-existing-p12'
                    color='secondary'
                    height='tiny'
                    onClick={() => {
                      setSigningErrors(null)
                      onChangeP12Certificate(null)
                      onChangeP12Password('')
                      setShowUploadExistingCertificate(false)
                      // If no existing signing info, also reset the add new certificate state
                      if (!existingSigningInfo) {
                        setShowAddNewCertificate(false)
                      }
                      // Restore existing certificate selection if available
                      if (existingSigningInfo?.certificateUuid) {
                        onCertificateSelectionChange(existingSigningInfo.certificateUuid)
                      }
                    }}
                  >
                    {t('editor_page.native_publish_modal.cancel')}
                  </PrimaryButton>
                </div>
              </div>
            </div>
          </>
        )}
        <div className={classes.stepWithLine}>
          <div className={naeModalsStyles.inputGroup}>

            <div className={combine(classes.textMuted, classes.divider)}>
              {updatingProvisioningProfile
                ? t('editor_page.export_modal.upload_provisioning_profile')
                : t('editor_page.export_modal.provisioning_profile')}
            </div>
            { updatingProvisioningProfile
              ? (
                <>
                  <div className={classes.textGray}>
                    {t('editor_page.export_modal.provisioning_profile_description', {
                      /* eslint-disable local-rules/hardcoded-copy */
                      profileName: signingType === 'development'
                        ? 'iOS App Development'
                        : 'App Store Connect',
                      /* eslint-enable local-rules/hardcoded-copy */
                    })}
                    <StandardLink
                      href='https://developer.apple.com/account/resources/profiles/add'
                      target='_blank'
                    >
                      {' https://developer.apple.com/account/resources/profiles/add. '}
                    </StandardLink>
                    {t('editor_page.export_modal.provisioning_profile_description_continued')}
                  </div>
                  <NaeRowUploadDrop
                    file={provisioningProfile}
                    onDrop={onChangeProvisioningProfile}
                    label={t('editor_page.export_modal.provisioning_profile')}
                    fileAccept='.mobileprovision'
                    disabled={showAddNewCertificate && !certificate}
                  />
                  {isUpdatingProvisioningProfile &&
                    <div className={classes.flexEnd}>
                      <TextButton
                        a8='click;cloud-editor-export-flow;cancel-update-provisioning-profile'
                        height='tiny'
                        onClick={() => {
                          setSigningErrors(null)
                          onChangeProvisioningProfile(null)
                          setIsUpdatingProvisioningProfile(false)
                        }}
                      >
                        {t('editor_page.native_publish_modal.cancel')}
                      </TextButton>
                    </div>}
                </>
              )
              : (
                <div className={combine(classes.gap075, classes.menuButton)}>
                  <div className={classes.uploadedFile}>
                    <div className={combine(classes.fileName, classes.textCenter)}>
                      <div className={classes.textCenter}>
                        {existingSigningInfo.provisioningProfileName}
                        {(existingSigningInfo.deviceUdids ||
                          existingSigningInfo.entitlements) && (
                            <TooltipIcon
                              // eslint-disable-next-line local-rules/hardcoded-copy
                              position='right center'
                              content={(
                                <div className={classes.tooltipContent}>
                                  {existingSigningInfo.deviceUdids?.length > 0 && (
                                    <div className={classes.tooltipSection}>
                                      <div className={classes.tooltipLabel}>
                                        {t('editor_page.export_modal.provisioning_profile' +
                                          '.provisioned_devices')}{' '}
                                        ({existingSigningInfo.deviceUdids.length}):
                                      </div>
                                      <div className={classes.tooltipScrollContainer}>
                                        {existingSigningInfo.deviceUdids.map(device => (
                                          <div key={device} className={classes.tooltipScrollItem}>
                                            {device}
                                          </div>
                                        ))}
                                      </div>
                                    </div>
                                  )}

                                  {existingSigningInfo.entitlements &&
                                    Object.keys(existingSigningInfo.entitlements).length > 0 && (
                                      <div className={classes.tooltipSection}>
                                        <div className={classes.tooltipLabel}>
                                          {t('editor_page.export_modal.provisioning_profile' +
                                            '.entitlements')}{' '}
                                          ({Object.keys(existingSigningInfo.entitlements).length}):
                                        </div>
                                        <div className={classes.tooltipScrollContainer}>
                                          {Object.entries(existingSigningInfo.entitlements).map(
                                            ([key, value]) => (
                                              <div key={key} className={classes.tooltipScrollItem}>
                                                <strong>{key}:</strong>{' '}
                                                {Array.isArray(value)
                                                  ? value.join(', ')
                                                  : String(value)}
                                              </div>
                                            )
                                          )}
                                        </div>
                                      </div>
                                  )}
                                </div>
                              )}
                              wide
                              hoverable
                            />
                        )}
                      </div>
                      <div className={combine(classes.fileExpiration, classes.textGray)}>
                        {getExpirationText(existingSigningInfo.provisioningProfileExpiration)}
                      </div>
                    </div>
                  </div>
                  <TextButton
                    a8='click;cloud-editor-export-flow;set-update-provisioning-profile'
                    height='tiny'
                    onClick={() => {
                      setIsUpdatingProvisioningProfile(true)
                    }}
                  >
                    {t('editor_page.export_modal.update_provisioning_profile')}
                  </TextButton>
                </div>
              )}

          </div>
        </div>
      </div>
    </Accordion.Content>
  )
}

interface IIosSigningPage {
  setCurrentStep: (step: Steps) => void
  bundleId: string
  setBundleId: (bundleId: string) => void
  getDefaultBundleId: () => string
  existingSigningInfo: SigningFilesInfoResponse
  setExistingSigningInfo: (info: SigningFilesInfoResponse) => void
}

const IosSigningPage: React.FC<IIosSigningPage> = ({
  setCurrentStep,
  bundleId,
  setBundleId,
  getDefaultBundleId,
  existingSigningInfo,
  setExistingSigningInfo,
}) => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const naeModalsStyles = useNaeModalsStyles()
  const app = useCurrentApp()
  const classes = useStyles()

  const {
    updateNaeInfo,
    uploadAuthKey,
    uploadSigningFiles,
    signingFilesInfo,
    deleteSigningConfig,
  } = useActions(naeActions)

  const findInitialCertificateUuid = (signingInfo?: SigningInfo): string => {
    if (!signingInfo) {
      return ''
    }
    const currentCert = signingInfo.availableCertificates.find(
      cert => cert.certificateCommonName === signingInfo.certificateCommonName
    )
    return currentCert ? currentCert.certificateUuid : ''
  }

  const [isSaving, setIsSaving] = React.useState(false)

  const [developmentCertificate, setDevelopmentCertificate] = React.useState<File | null>(null)
  const [developmentProvisioningProfile,
    setOnDeviceProvisioningProfile] = React.useState<File | null>(null)
  const [distributionCertificate, setDistributionCertificate] = React.useState<File | null>(null)
  const [distributionProvisioningProfile,
    setDistributionProvisioningProfile] = React.useState<File | null>(null)
  const [selectedDevelopmentCertificateUuid,
    setSelectedDevelopmentCertificateUuid] = React.useState(findInitialCertificateUuid(
    existingSigningInfo.developmentSigningInfo
  ))
  const [selectedDistributionCertificateUuid,
    setSelectedDistributionCertificateUuid] = React.useState(findInitialCertificateUuid(
    existingSigningInfo.distributionSigningInfo
  ))
  const [showDevelopmentAddNewCertificate,
    setShowDevelopmentAddNewCertificate] = React.useState(false)
  const [showDevelopmentUploadExistingCertificate,
    setShowDevelopmentUploadExistingCertificate] = React.useState(false)
  const [showDistributionAddNewCertificate,
    setShowDistributionAddNewCertificate] = React.useState(false)
  const [showDistributionUploadExistingCertificate,
    setShowDistributionUploadExistingCertificate] = React.useState(false)
  const [isUpdatingDevelopmentProvisioningProfile,
    setIsUpdatingDevelopmentProvisioningProfile] = React.useState(false)
  const [isUpdatingDistributionProvisioningProfile,
    setIsUpdatingDistributionProvisioningProfile] = React.useState(false)
  const [keyId, setKeyId] = React.useState('')
  const [issuerId, setIssuerId] = React.useState('')
  const [authKey, setAuthKey] = React.useState<File | null>(null)
  const [appStoreConnectOpen, setAppStoreConnectOpen] = React.useState(false)
  const [developmentSigningOpen, setDevelopmentSigningOpen] = React.useState(false)
  const [distributionSigningOpen, setDistributionSigningOpen] = React.useState(false)
  const [developmentCsrBase64, setDevelopmentCsrBase64] = React.useState<string | null>(null)
  const [developmentCsrPrivateKeyBase64,
    setDevelopmentCsrPrivateKeyBase64] = React.useState<string | null>(null)
  const [distributionCsrBase64, setDistributionCsrBase64] = React.useState<string | null>(null)
  const [distributionCsrPrivateKeyBase64,
    setDistributionCsrPrivateKeyBase64] = React.useState<string | null>(null)
  const [developmentSigningErrors,
    setDevelopmentSigningErrors] = React.useState<string[] | null>(null)
  const [distributionSigningErrors,
    setDistributionSigningErrors] = React.useState<string[] | null>(null)
  const [developmentP12Certificate,
    setDevelopmentP12Certificate] = React.useState<File | null>(null)
  const [developmentP12Password, setDevelopmentP12Password] = React.useState('')
  const [distributionP12Certificate,
    setDistributionP12Certificate] = React.useState<File | null>(null)
  const [distributionP12Password, setDistributionP12Password] = React.useState('')
  const [pendingDeletions, setPendingDeletions] = React.useState<Set<AppleSigningType>>(new Set())

  const handleDeleteSigningConfig = (signingType: AppleSigningType) => {
    setPendingDeletions((prev) => {
      const newSet = new Set([...prev])
      if (newSet.has(signingType)) {
        newSet.delete(signingType)
      } else {
        newSet.add(signingType)
        // Close the accordion when marking for deletion
        if (signingType === 'development') {
          setDevelopmentSigningOpen(false)
        } else if (signingType === 'distribution') {
          setDistributionSigningOpen(false)
        }
      }
      return newSet
    })
  }

  const refreshSigningInfo = React.useCallback(async () => {
    const signingResponse = await signingFilesInfo(app.uuid)
    if (signingResponse) {
      setExistingSigningInfo(signingResponse)
      return signingResponse
    }
    return null
  }, [app.uuid, signingFilesInfo])

  React.useEffect(() => {
    const {authKeyInfo} = existingSigningInfo
    if (authKeyInfo) {
      setKeyId(authKeyInfo.keyId)
      setIssuerId(authKeyInfo.issuerId)
      setAuthKey(new File([], authKeyInfo.keyFileName))
    }
  }, [existingSigningInfo.authKeyInfo])

  const bundleIdValidationResult = validateBundleId(bundleId, 'ios', t)

  const handleUploadAuthKey = async () => {
    await uploadAuthKey({
      appUuid: app.uuid,
      keyId,
      issuerId,
      authKey,
    })
  }

  const getPendingChanges = React.useMemo(() => ({
    development: getSaveData(
      developmentCertificate,
      developmentProvisioningProfile,
      selectedDevelopmentCertificateUuid,
      developmentCsrBase64,
      developmentCsrPrivateKeyBase64,
      developmentP12Certificate,
      developmentP12Password,
      showDevelopmentUploadExistingCertificate
    ),
    distribution: getSaveData(
      distributionCertificate,
      distributionProvisioningProfile,
      selectedDistributionCertificateUuid,
      distributionCsrBase64,
      distributionCsrPrivateKeyBase64,
      distributionP12Certificate,
      distributionP12Password,
      showDistributionUploadExistingCertificate
    ),
    authKey,
    deletions: pendingDeletions,
  }), [
    developmentCertificate,
    developmentProvisioningProfile,
    selectedDevelopmentCertificateUuid,
    developmentCsrBase64,
    developmentCsrPrivateKeyBase64,
    developmentP12Certificate,
    developmentP12Password,
    showDevelopmentUploadExistingCertificate,
    distributionCertificate,
    distributionProvisioningProfile,
    selectedDistributionCertificateUuid,
    distributionCsrBase64,
    distributionCsrPrivateKeyBase64,
    distributionP12Certificate,
    distributionP12Password,
    showDistributionUploadExistingCertificate,
    authKey,
    pendingDeletions,
  ])

  const uploadSigning = async (
    saveData: ReturnType<typeof getSaveData>,
    signingType: 'development' | 'distribution'
  ): Promise<UploadSigningFilesResponse | null> => {
    if (!saveData) {
      return null
    }
    if (saveData.type === 'new-certificate') {
      return uploadSigningFiles({
        appUuid: app.uuid,
        // @ts-expect-error TODO(christoph): Clean up
        accountUuid: app.AccountUuid,
        bundleIdentifier: bundleId,
        signingType,
        certificate: saveData.certificate,
        provisioningProfile: saveData.provisioningProfile,
        certificateSigningRequestBase64: saveData.csrBase64,
        certificateSigningRequestPrivateKeyBase64: saveData.csrPrivateKeyBase64,
      })
    }
    if (saveData.type === 'update-provisioning-profile') {
      return uploadSigningFiles({
        appUuid: app.uuid,
        // @ts-expect-error TODO(christoph): Clean up
        accountUuid: app.AccountUuid,
        bundleIdentifier: bundleId,
        signingType,
        certificateUuid: saveData.certificateUuid,
        provisioningProfile: saveData.provisioningProfile,
      })
    }
    if (saveData.type === 'upload-existing-certificate') {
      return uploadSigningFiles({
        appUuid: app.uuid,
        // @ts-expect-error TODO(christoph): Clean up
        accountUuid: app.AccountUuid,
        bundleIdentifier: bundleId,
        signingType,
        certificate: saveData.p12Certificate,
        certificateP12Password: saveData.p12Password,
        provisioningProfile: saveData.provisioningProfile,
      })
    }
    return null
  }

  const handleSave = async () => {
    setIsSaving(true)
    await updateNaeInfo({
      appUuid: app.uuid,
      platform: 'ios',
      bundleId,
    })

    const {development, distribution, authKey: pendingAuthKey, deletions} = getPendingChanges
    if (!development && !distribution && !pendingAuthKey && deletions.size === 0) {
      // Return early if we only wanted to save the bundleIdentifier.
      setIsSaving(false)
      return
    }

    // Clear any previous errors
    setDevelopmentSigningErrors(null)
    setDistributionSigningErrors(null)
    let developmentSucceeded = false
    let distributionSucceeded = false
    let authKeySucceeded = false
    let deletionSucceeded = false

    // Process deletions first
    if (deletions.size > 0) {
      await Promise.all(
        Array.from(deletions).map(signingType => deleteSigningConfig(app.uuid, signingType))
      )
      deletionSucceeded = true
      if (deletions.has('development')) {
        setSelectedDevelopmentCertificateUuid('')
      }
      if (deletions.has('distribution')) {
        setSelectedDistributionCertificateUuid('')
      }
      setPendingDeletions(new Set())
    }

    if (development) {
      try {
        await uploadSigning(development, 'development')
        developmentSucceeded = true
        setDevelopmentSigningErrors(null)
      } catch (error) {
        const errors = processUploadError(error, t)
        setDevelopmentSigningErrors(errors)
      }
    }

    if (distribution) {
      try {
        await uploadSigning(distribution, 'distribution')
        distributionSucceeded = true
        setDistributionSigningErrors(null)
      } catch (error) {
        const errors = processUploadError(error, t)
        setDistributionSigningErrors(errors)
      }
    }

    if (pendingAuthKey && BuildIf.NAE_IOS_APP_CONNECT_API_20250822) {
      try {
        await handleUploadAuthKey()
        authKeySucceeded = true
      } catch (error) {
        // TODO(akashmahesh): Handle errors when Auth Key Upload is enabled.
      }
    }

    // Refresh signing info if any operation succeeded
    let updatedSigningInfo = existingSigningInfo
    if (developmentSucceeded || distributionSucceeded || authKeySucceeded || deletionSucceeded) {
      const refreshedInfo = await refreshSigningInfo()
      if (refreshedInfo) {
        updatedSigningInfo = refreshedInfo
      }
    }

    // Clear state for successful uploads
    if (developmentSucceeded) {
      setDevelopmentCertificate(null)
      setOnDeviceProvisioningProfile(null)
      setIsUpdatingDevelopmentProvisioningProfile(false)
      setShowDevelopmentAddNewCertificate(false)
      setShowDevelopmentUploadExistingCertificate(false)
      setSelectedDevelopmentCertificateUuid(updatedSigningInfo.developmentSigningInfo
        ? updatedSigningInfo.developmentSigningInfo.certificateUuid
        : '')
      setDevelopmentP12Certificate(null)
      setDevelopmentP12Password('')
      setDevelopmentCsrBase64(null)
      setDevelopmentCsrPrivateKeyBase64(null)
    }
    if (distributionSucceeded) {
      setDistributionCertificate(null)
      setDistributionProvisioningProfile(null)
      setIsUpdatingDistributionProvisioningProfile(false)
      setShowDistributionAddNewCertificate(false)
      setShowDistributionUploadExistingCertificate(false)
      setSelectedDistributionCertificateUuid(updatedSigningInfo.distributionSigningInfo
        ? updatedSigningInfo.distributionSigningInfo.certificateUuid
        : '')
      setDistributionP12Certificate(null)
      setDistributionP12Password('')
      setDistributionCsrBase64(null)
      setDistributionCsrPrivateKeyBase64(null)
    }
    if (authKeySucceeded) {
      setAuthKey(null)
      setKeyId('')
      setIssuerId('')
    }

    setIsSaving(false)
  }

  const hasSomethingToSave = () => {
    const {development, distribution, authKey: pendingAuthKey, deletions} = getPendingChanges
    return Boolean(
      development || distribution || pendingAuthKey || deletions.size > 0 ||
      bundleId !== getDefaultBundleId()
    )
  }

  return (
    <PublishPageWrapper
      headline={t('editor_page.native_publish_modal.ios_configure_signing_headline')}
      headlineType='mobile'
      onBack={() => setCurrentStep('start')}
      actionButton={(
        <PrimaryButton
          a8='click;cloud-editor-export-flow;save-ios-signing'
          color='purple'
          height='small'
          disabled={!bundleIdValidationResult.success || !hasSomethingToSave()}
          onClick={handleSave}
          loading={isSaving}
        >
          {t('button.save', {ns: 'common'})}
        </PrimaryButton>
      )}
    >
      <div className={classes.page}>
        <div className={classes.gap05}>
          <div
            className={naeModalsStyles.inputGroup}
          >
            <RowTextField
              id='bundleId'
              label={t('editor_page.export_modal.bundle_id')}
              placeholder='com.company.app_name'
              value={bundleId}
              onChange={e => setBundleId(e.target.value)}
              onBlur={() => {
                if (bundleId === '') {
                  setBundleId(getDefaultBundleId())
                }
              }}
            />
            {bundleIdValidationResult.uiResult &&
              <TextNotification type={bundleIdValidationResult.uiResult.status}>
                {bundleIdValidationResult.uiResult.message}
              </TextNotification>
            }
          </div>
          <div className={combine(classes.leading12, classes.textMuted)}>
            {t('editor_page.export_modal.apple_certificate_and_provisioning_profile.title')}
          </div>
          <div className={combine(classes.leading12, classes.textGray)}>
            {t('editor_page.export_modal.apple_certificate_and_provisioning_profile.description')}
          </div>
          <Accordion className={classes.accordion}>
            <Accordion.Title
              active={developmentSigningOpen}
              onClick={() => {
                if (!pendingDeletions.has('development')) {
                  setDevelopmentSigningOpen(!developmentSigningOpen)
                }
              }}
              className={combine(
                classes.accordionTitle,
                pendingDeletions.has('development') && classes.disabled
              )}
            >
              <div className={classes.menuButton}>
                {t('editor_page.export_modal.development_signing.title')}
                <TooltipIcon
                  // eslint-disable-next-line local-rules/hardcoded-copy
                  position='right center'
                  content={t('editor_page.export_modal.development_signing.tooltip')}
                />
                <DeleteSigningButton
                  signingType='development'
                  hasExistingConfig={!!existingSigningInfo.developmentSigningInfo?.certificateUuid}
                  isPendingDeletion={pendingDeletions.has('development')}
                  onDeleteClick={handleDeleteSigningConfig}
                />
              </div>
            </Accordion.Title>
            {developmentSigningErrors && (
              <div className={classes.errorContainer}>
                <ColoredMessage
                  key='development-errors'
                  color='red'
                  // eslint-disable-next-line local-rules/hardcoded-copy
                  iconName='exclamation triangle'
                >
                  <div>
                    <strong>{t('editor_page.export_modal.validation_errors')}:</strong>
                    <ul className={classes.errorList}>
                      {developmentSigningErrors.map(error => (
                        <li key={error} className={classes.errorItem}>
                          {error}
                        </li>
                      ))}
                    </ul>
                  </div>
                </ColoredMessage>
              </div>
            )}
            <SigningContent
              certificate={developmentCertificate}
              onChangeCertificate={setDevelopmentCertificate}
              provisioningProfile={developmentProvisioningProfile}
              isUpdatingProvisioningProfile={isUpdatingDevelopmentProvisioningProfile}
              setIsUpdatingProvisioningProfile={setIsUpdatingDevelopmentProvisioningProfile}
              onChangeProvisioningProfile={setOnDeviceProvisioningProfile}
              signingType='development'
              existingSigningInfo={existingSigningInfo.developmentSigningInfo}
              selectedCertificateUuid={selectedDevelopmentCertificateUuid}
              onCertificateSelectionChange={setSelectedDevelopmentCertificateUuid}
              showAddNewCertificate={showDevelopmentAddNewCertificate}
              setShowAddNewCertificate={setShowDevelopmentAddNewCertificate}
              showUploadExistingCertificate={showDevelopmentUploadExistingCertificate}
              setShowUploadExistingCertificate={setShowDevelopmentUploadExistingCertificate}
              p12Certificate={developmentP12Certificate}
              onChangeP12Certificate={setDevelopmentP12Certificate}
              p12Password={developmentP12Password}
              onChangeP12Password={setDevelopmentP12Password}
              csrBase64={developmentCsrBase64}
              setCsrBase64={setDevelopmentCsrBase64}
              setCsrPrivateKeyBase64={setDevelopmentCsrPrivateKeyBase64}
              setSigningErrors={setDevelopmentSigningErrors}
              onRefreshSigningInfo={refreshSigningInfo}
            />
          </Accordion>
          <Accordion className={classes.accordion}>
            <Accordion.Title
              active={distributionSigningOpen}
              onClick={() => {
                if (!pendingDeletions.has('distribution')) {
                  setDistributionSigningOpen(!distributionSigningOpen)
                }
              }}
              className={combine(
                classes.accordionTitle,
                pendingDeletions.has('distribution') && classes.disabled
              )}
            >
              <div className={classes.menuButton}>
                {t('editor_page.export_modal.distribution_signing.title')}
                <TooltipIcon
                  // eslint-disable-next-line local-rules/hardcoded-copy
                  position='right center'
                  content={t('editor_page.export_modal.distribution_signing.tooltip')}
                />
                <DeleteSigningButton
                  signingType='distribution'
                  hasExistingConfig={!!existingSigningInfo.distributionSigningInfo?.certificateUuid}
                  isPendingDeletion={pendingDeletions.has('distribution')}
                  onDeleteClick={handleDeleteSigningConfig}
                />
              </div>
            </Accordion.Title>
            {distributionSigningErrors && (
              <div className={classes.errorContainer}>
                <ColoredMessage
                  key='distribution-errors'
                  color='red'
                  // eslint-disable-next-line local-rules/hardcoded-copy
                  iconName='exclamation triangle'
                >
                  <div>
                    <strong>{t('editor_page.export_modal.validation_errors')}:</strong>
                    <ul className={classes.errorList}>
                      {distributionSigningErrors.map(error => (
                        <li key={error} className={classes.errorItem}>
                          {error}
                        </li>
                      ))}
                    </ul>
                  </div>
                </ColoredMessage>
              </div>
            )}
            <SigningContent
              certificate={distributionCertificate}
              onChangeCertificate={setDistributionCertificate}
              provisioningProfile={distributionProvisioningProfile}
              isUpdatingProvisioningProfile={isUpdatingDistributionProvisioningProfile}
              setIsUpdatingProvisioningProfile={setIsUpdatingDistributionProvisioningProfile}
              onChangeProvisioningProfile={setDistributionProvisioningProfile}
              signingType='distribution'
              existingSigningInfo={existingSigningInfo.distributionSigningInfo}
              selectedCertificateUuid={selectedDistributionCertificateUuid}
              onCertificateSelectionChange={setSelectedDistributionCertificateUuid}
              showAddNewCertificate={showDistributionAddNewCertificate}
              setShowAddNewCertificate={setShowDistributionAddNewCertificate}
              showUploadExistingCertificate={showDistributionUploadExistingCertificate}
              setShowUploadExistingCertificate={setShowDistributionUploadExistingCertificate}
              p12Certificate={distributionP12Certificate}
              onChangeP12Certificate={setDistributionP12Certificate}
              p12Password={distributionP12Password}
              onChangeP12Password={setDistributionP12Password}
              csrBase64={distributionCsrBase64}
              setCsrBase64={setDistributionCsrBase64}
              setCsrPrivateKeyBase64={setDistributionCsrPrivateKeyBase64}
              setSigningErrors={setDistributionSigningErrors}
              onRefreshSigningInfo={refreshSigningInfo}
            />
          </Accordion>
        </div>

        {BuildIf.NAE_IOS_APP_CONNECT_API_20250822 &&
          <div className={classes.gap05}>
            <div className={combine(classes.leading12, classes.textMuted)}>
              {t('editor_page.export_modal.app_store_connect_api.accordion_title')}
            </div>
            <Accordion className={classes.accordion}>
              <Accordion.Title
                active={appStoreConnectOpen}
                onClick={() => { setAppStoreConnectOpen(!appStoreConnectOpen) }}
                className={classes.accordionTitle}
              >
                <div className={classes.gap075}>
                  {t('editor_page.export_modal.app_store_connect_api.accordion_title')}
                  <TooltipIcon
                    // eslint-disable-next-line local-rules/hardcoded-copy
                    position='right center'
                    content={t('editor_page.export_modal.app_store_connect_api.tooltip')}
                  />
                </div>
              </Accordion.Title>
              <Accordion.Content className={naeModalsStyles.inputGroup}>
                <p className={combine(classes.textXs, classes.textGray)}>
                  {t('editor_page.export_modal.app_store_connect_api.description')}
                </p>
                <RowTextField
                  id='keyId'
                  label={t('editor_page.export_modal.key_id')}
                  placeholder={t('editor_page.export_modal.key_id_placeholder')}
                  value={keyId}
                  onChange={e => setKeyId(e.target.value)}
                />
                <RowTextField
                  id='issuerId'
                  label={t('editor_page.export_modal.issuer_id')}
                  placeholder={t('editor_page.export_modal.issuer_id_placeholder')}
                  value={issuerId}
                  onChange={e => setIssuerId(e.target.value)}
                />

                <NaeRowUploadDrop
                  file={authKey}
                  onDrop={setAuthKey}
                  label={t('editor_page.export_modal.auth_key')}
                  fileAccept='.p8'
                />
              </Accordion.Content>
            </Accordion>
          </div>
        }
      </div>
    </PublishPageWrapper>
  )
}

export {IosSigningPage}
