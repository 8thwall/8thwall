import React, {useState} from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles, useUiTheme} from '../../../ui/theme'
import {FloatingTrayModal} from '../../../ui/components/floating-tray-modal'
import {Icon} from '../../../ui/components/icon'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {TextButton} from '../../../ui/components/text-button'
import {StaticBanner} from '../../../ui/components/banner'
import useActions from '../../../common/use-actions'
import naeActions from '../../../studio/actions/nae-actions'
import useCurrentApp from '../../../common/use-current-app'
import useCurrentAccount from '../../../common/use-current-account'

const useStyles = createThemedStyles(theme => ({
  modalOverlay: {
    zIndex: 4000,  // Higher than dropdown z-index of 3000
  },
  content: {
    background: theme.modalBg,
    color: theme.modalFg,
    borderRadius: '15px',
    whiteSpace: 'pre-wrap',
    display: 'flex',
    flexDirection: 'column',
    padding: '2em',
    justifyContent: 'center',
    alignItems: 'center',
    textAlign: 'center',
    gap: '1.5em',
  },
  header: {
    margin: 'auto',
  },
  buttonRow: {
    display: 'flex',
    gap: '1em',
    justifyContent: 'center',
  },
}))

interface IDeleteCertificateModal {
  trigger: React.ReactNode
  certificateUuid: string
  certificateName: string
  onDeleteSuccess: () => void
}

const DeleteCertificateModal: React.FC<IDeleteCertificateModal> = ({
  trigger, certificateUuid, certificateName, onDeleteSuccess,
}) => {
  const {t} = useTranslation(['cloud-editor-pages'])
  const theme = useUiTheme()
  const classes = useStyles()
  const app = useCurrentApp()
  const account = useCurrentAccount()
  const {deleteCertificate} = useActions(naeActions)

  const [isDeleting, setIsDeleting] = useState(false)
  const [unexpectedErrorMessage, setUnexpectedErrorMessage] = useState<string | null>(null)
  const [conflictingApps, setConflictingApps] = useState<string[]>([])

  const deleteCert = async (collapse: () => void) => {
    setIsDeleting(true)
    setUnexpectedErrorMessage(null)
    setConflictingApps([])

    try {
      const res = await deleteCertificate(account.uuid, app.uuid, certificateUuid)
      if (res.success) {
        onDeleteSuccess()
        collapse()
      } else if (res.conflictingAppTitles) {
        setConflictingApps(res.conflictingAppTitles)
      } else {
        setUnexpectedErrorMessage(
          t('editor_page.export_modal.delete_certificate_modal.unexpected_error')
        )
      }
    } catch (error) {
      setUnexpectedErrorMessage(
        t('editor_page.export_modal.delete_certificate_modal.network_error')
      )
    } finally {
      setIsDeleting(false)
    }
  }

  const renderContent = () => {
    if (conflictingApps.length > 0) {
      return (
        <div>
          <p>
            {t('editor_page.export_modal.delete_certificate_modal.conflict_message', {
              certificateName,
            })}
          </p>
          <StaticBanner type='danger'>
            {t('editor_page.export_modal.delete_certificate_modal.conflicting_apps', {
              appList: conflictingApps.join(', '),
            })}
          </StaticBanner>
        </div>
      )
    }

    const modalDefaultBody = (
      <p>
        {t('editor_page.export_modal.delete_certificate_modal.body', {
          certificateName,
        })}
      </p>
    )

    if (unexpectedErrorMessage) {
      return (
        <div>
          {modalDefaultBody}
          <StaticBanner type='danger'>
            {unexpectedErrorMessage}
          </StaticBanner>
        </div>
      )
    }

    return modalDefaultBody
  }

  return (
    <FloatingTrayModal trigger={trigger} overlayClass={classes.modalOverlay}>
      {collapse => (
        <div className={classes.content}>
          <Icon stroke='delete12' color={theme.modalFg} size={2.5} />
          <h1 className={classes.header}>
            {t('editor_page.export_modal.delete_certificate_modal.header')}
          </h1>

          {renderContent()}

          <div className={classes.buttonRow}>
            <TextButton
              type='button'
              onClick={collapse}
              disabled={isDeleting}
            >
              {t('editor_page.export_modal.delete_certificate_modal.cancel')}
            </TextButton>
            {conflictingApps.length === 0 && (
              <PrimaryButton
                type='button'
                loading={isDeleting}
                disabled={isDeleting}
                onClick={() => deleteCert(collapse)}
                height='small'
              >
                {t('editor_page.export_modal.delete_certificate_modal.confirm')}
              </PrimaryButton>
            )}
          </div>
        </div>
      )}
    </FloatingTrayModal>
  )
}

export {
  DeleteCertificateModal,
}
