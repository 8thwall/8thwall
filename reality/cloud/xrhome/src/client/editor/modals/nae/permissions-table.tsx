import React from 'react'
import {useTranslation} from 'react-i18next'

import {
  validatePermissionUsageDescriptionUtil,
  NAE_PERMISSION_USAGE_DESCRIPTION_MAX_CHARACTERS,
} from '../../../../shared/nae/nae-utils'
import {hexColorWithAlpha} from '../../../../shared/colors'
import {
  IOS_AVAILABLE_PERMISSIONS,
  CAMERA_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
  LOCATION_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
  MICROPHONE_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
} from '../../../../shared/nae/nae-constants'
import type {
  HtmlShell,
  IosAvailablePermissions,
  Permissions,
} from '../../../../shared/nae/nae-types'
import {gray4} from '../../../static/styles/settings'
import {Icon, IconStroke} from '../../../ui/components/icon'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {StandardInlineToggleField} from '../../../ui/components/standard-inline-toggle-field'
import {StandardTextInput} from '../../../ui/components/standard-text-input'
import {createThemedStyles} from '../../../ui/theme'
import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import type {Steps} from './export-flow'
import {TooltipIcon} from '../../../widgets/tooltip-icon'
import {TextNotification} from '../../../ui/components/text-notification'
import {validatePermissionUsageDescription} from './validation'
import useCurrentApp from '../../../common/use-current-app'
import naeActions from '../../../studio/actions/nae-actions'
import {useAbandonableEffect} from '../../../hooks/abandonable-effect'
import useActions from '../../../common/use-actions'

const hover = '&:hover'
const placeholder = '&::placeholder'

const useStyles = createThemedStyles(theme => ({
  page: {
    display: 'flex',
    flexDirection: 'column',
    gap: '1.5rem',
  },
  table: {
    backgroundColor: theme.modalContainerBg,
    borderRadius: '0.375rem',
    overflow: 'hidden',
    width: '100%',
    border: `1px solid ${theme.modalContainerBg}`,
    borderCollapse: 'separate',
    borderSpacing: 0,
    tableLayout: 'fixed',
  },
  tableHeader: {
    backgroundColor: hexColorWithAlpha(theme.publishModalBg, 0.85),
  },
  headerRow: {
    height: '3rem',
  },
  permissionTypeHeader: {
    fontSize: '12px',
    fontWeight: 600,
    color: theme.fgMuted,
    padding: '0.5rem 0.75rem',
    textAlign: 'left',
    verticalAlign: 'middle',
    width: '35%',
  },
  descriptionHeader: {
    fontSize: '12px',
    fontWeight: 600,
    color: theme.fgMuted,
    padding: '0.5rem 0.75rem',
    textAlign: 'left',
    verticalAlign: 'middle',
    width: '50%',
  },
  resetAllButton: {
    fontSize: '12px',
    fontWeight: 600,
    color: theme.fgMain,
    background: 'none',
    border: 'none',
    cursor: 'pointer',
    padding: 0,
    float: 'right',
    [hover]: {
      textDecoration: 'underline',
    },
  },
  publishModalBg: {
    backgroundColor: theme.publishModalBg,
  },
  tableRow: {
    backgroundColor: theme.publishModalBg,
    borderTop: `1px solid ${theme.modalContainerBg}`,
  },
  permissionCell: {
    padding: '1rem 0.75rem',
    verticalAlign: 'middle',
    width: '35%',
  },
  permissionSection: {
    display: 'flex',
    alignItems: 'center',
    gap: '1.5rem',
  },
  permissionInfo: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    flex: 1,
  },
  permissionIcon: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    width: '14px',
    height: '14px',
    flexShrink: 0,
  },
  permissionName: {
    fontSize: '12px',
    fontWeight: 600,
    color: theme.fgMain,
    lineHeight: '16px',
    whiteSpace: 'nowrap',
    overflow: 'hidden',
    textOverflow: 'ellipsis',
  },
  inputCell: {
    padding: '1rem 0.75rem',
    verticalAlign: 'middle',
    width: '65%',
  },
  textInput: {
    'width': '100%',
    '& input': {
      backgroundColor: theme.inputFieldBg,
      border: 'none',
      borderRadius: '0.25rem',
      padding: '0.25rem 0.75rem',
      fontSize: '12px',
      color: theme.fgMain,
      lineHeight: '1rem',
      minHeight: '2rem',
      width: '100%',
      [placeholder]: {
        color: gray4,
        fontStyle: 'italic',
      },
    },
  },
}))

const DEFAULT_PERMISSIONS: Permissions = {
  camera: {requestStatus: 'NOT_REQUESTED', usageDescription: ''},
  location: {requestStatus: 'NOT_REQUESTED', usageDescription: ''},
  microphone: {requestStatus: 'NOT_REQUESTED', usageDescription: ''},
}

const PERMISSION_PLACEHOLDERS: Record<IosAvailablePermissions, string> = {
  camera: CAMERA_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
  location: LOCATION_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
  microphone: MICROPHONE_PERMISSION_USAGE_DESCRIPTION_DEFAULT,
} as const

const PERMISSION_ICONS: Record<IosAvailablePermissions, IconStroke> = {
  camera: 'camera',
  location: 'vpsLocation12',
  microphone: 'microphone',
} as const

const PERMISSION_NAMES: Record<IosAvailablePermissions, string> = {
  camera: 'editor_page.export_modal.permissions.camera',
  location: 'editor_page.export_modal.permissions.location',
  microphone: 'editor_page.export_modal.permissions.microphone',
} as const

const getAvailablePermissions = (platform: HtmlShell): IosAvailablePermissions[] => {
  if (platform === 'ios') {
    return [...IOS_AVAILABLE_PERMISSIONS]
  }
  return []
}

interface IPermissionsTable {
  platform: HtmlShell
  setCurrentStep: (step: Steps) => void
}

const PermissionsTable: React.FC<IPermissionsTable> = ({setCurrentStep, platform}) => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useStyles()
  const availablePermissions = getAvailablePermissions(platform)
  const [isSaving, setIsSaving] = React.useState(false)
  const [permissions, setPermissions] = React.useState<Permissions>(DEFAULT_PERMISSIONS)
  const [originalPermissions, setOriginalPermissions] =
  React.useState<Permissions>(DEFAULT_PERMISSIONS)
  const app = useCurrentApp()
  const {updateNaeInfo} = useActions(naeActions)
  const naeInfo = React.useMemo(
    // @ts-expect-error TODO(christoph): Clean up
    () => app.NaeInfos?.find(info => info.platform === platform.toUpperCase()),
    // @ts-expect-error TODO(christoph): Clean up
    [app.NaeInfos, platform]
  )

  useAbandonableEffect(() => {
    if (!naeInfo.permissions) {
      return
    }
    setPermissions(naeInfo.permissions)
    setOriginalPermissions(naeInfo.permissions)
  }, [naeInfo?.permissions])

  const handlePermissionToggle = (id: string, enabled: boolean) => {
    setPermissions(prev => ({
      ...prev,
      [id]: {
        requestStatus: enabled ? 'REQUESTED' : 'NOT_REQUESTED',
        ...(enabled && {usageDescription: prev[id].usageDescription}),
      },
    }))
  }

  const handleDescriptionChange = (id: string, usageDescription: string) => {
    setPermissions(prev => ({
      ...prev,
      [id]: {
        ...prev[id],
        usageDescription,
      },
    }))
  }

  const handleResetAll = () => {
    setPermissions(DEFAULT_PERMISSIONS)
  }

  const handleSave = async () => {
    if (isSaving) {
      return
    }
    setIsSaving(true)
    await updateNaeInfo({
      appUuid: app.uuid,
      platform,
      permissions: JSON.stringify(permissions),
    })
    setIsSaving(false)
  }

  const invalidPermission = availablePermissions
    .some(permissionName => validatePermissionUsageDescriptionUtil(
      permissions[permissionName].usageDescription
    ) !== 'success')

  const hasChanges = availablePermissions.some((permissionName) => {
    const current = permissions[permissionName]
    const original = originalPermissions[permissionName]
    return current.requestStatus !== original.requestStatus ||
      current.usageDescription?.trim() !== original.usageDescription
  })

  return (
    <PublishPageWrapper
      headline={t('editor_page.export_modal.permissions.configure_permissions_headline')}
      headlineType='build'
      onBack={() => setCurrentStep('start')}
      actionButton={(
        <PrimaryButton
          a8='click;cloud-editor-export-flow;save-permissions'
          color='purple'
          height='small'
          disabled={!hasChanges || invalidPermission}
          loading={isSaving}
          onClick={handleSave}
        >
          {t('button.save', {ns: 'common'})}
        </PrimaryButton>
      )}
    >
      <div className={classes.page}>
        <table className={classes.table}>
          <thead className={classes.tableHeader}>
            <tr className={classes.headerRow}>
              <th className={classes.permissionTypeHeader}>
                {t('editor_page.export_modal.permissions.permission')}
              </th>
              <th className={classes.descriptionHeader}>
                {t('editor_page.export_modal.permissions.usage_description')}
                <TooltipIcon
                  content={t('editor_page.export_modal.permissions.usage_description_tooltip')}
                />
                <button
                  className={classes.resetAllButton}
                  onClick={handleResetAll}
                  type='button'
                >
                  {t('editor_page.export_modal.permissions.reset_all')}
                </button>
              </th>
            </tr>
          </thead>

          <tbody className={classes.publishModalBg}>
            {availablePermissions.map((permission) => {
              const permissionStatus = permissions[permission].requestStatus
              const permissionUsageDescription = permissions[permission].usageDescription
              const permissionValidationResult = validatePermissionUsageDescription(
                permissionUsageDescription, t
              )
              return (
                <tr key={permission} className={classes.tableRow}>
                  <td className={classes.permissionCell}>
                    <div className={classes.permissionSection}>
                      <StandardInlineToggleField
                        id={`permission-${permission}`}
                        label=''
                        checked={permissionStatus === 'REQUESTED'}
                        onChange={checked => handlePermissionToggle(permission, checked)}
                      />
                      <div className={classes.permissionInfo}>
                        <div className={classes.permissionIcon}>
                          <Icon
                            stroke={PERMISSION_ICONS[permission]}
                            size={0.875}
                            color='muted'
                          />
                        </div>
                        <div className={classes.permissionName}>
                          {t(PERMISSION_NAMES[permission])}
                        </div>
                      </div>
                    </div>
                  </td>
                  <td className={classes.inputCell}>
                    <div className={classes.textInput}>
                      <StandardTextInput
                        id={`description-${permission}`}
                        value={permissionStatus === 'REQUESTED' ? permissionUsageDescription : ''}
                        onChange={e => handleDescriptionChange(permission, e.target.value)}
                        disabled={permissionStatus !== 'REQUESTED'}
                        placeholder={
                        PERMISSION_PLACEHOLDERS[permission]
                      }
                        height='small'
                        maxLength={NAE_PERMISSION_USAGE_DESCRIPTION_MAX_CHARACTERS}
                      />
                    </div>
                    {permissionValidationResult.uiResult &&
                      <TextNotification type={permissionValidationResult.uiResult.status}>
                        {permissionValidationResult.uiResult.message}
                      </TextNotification>
                    }
                  </td>
                </tr>
              )
            })}
          </tbody>
        </table>
      </div>
    </PublishPageWrapper>
  )
}

export {PermissionsTable}
