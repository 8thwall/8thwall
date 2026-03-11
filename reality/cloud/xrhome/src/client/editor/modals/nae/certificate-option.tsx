import React from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../../ui/theme'
import {Icon} from '../../../ui/components/icon'
import {IconButton} from '../../../ui/components/icon-button'
import {FloatingMenuButton} from '../../../ui/components/floating-menu-button'
import {TextButton} from '../../../ui/components/text-button'
import {useStyles as useRowStyles} from '../../../studio/configuration/row-styles'
import {DeleteCertificateModal} from './delete-certificate-modal'

const useStyles = createThemedStyles(() => ({
  certificateOption: {
    'position': 'relative',
    'width': '100%',
  },
  selectOption: {
    'paddingRight': '2.5rem !important',
  },
  actionButtons: {
    'position': 'absolute',
    'right': '0.75rem',
    'top': '50%',
    'transform': 'translateY(-50%)',
    'display': 'flex',
    'alignItems': 'center',
    'gap': '0.5rem',
  },
}))

interface ICertificateOption {
  certificateUuid: string
  certificateName: string
  isSelected: boolean
  onSelect: () => void
  onDeleteSuccess: () => void
  collapse: () => void
}

const CertificateOption: React.FC<ICertificateOption> = ({
  certificateUuid,
  certificateName,
  isSelected,
  onSelect,
  onDeleteSuccess,
  collapse,
}) => {
  const {t} = useTranslation(['cloud-editor-pages'])
  const classes = useStyles()
  const rowStyles = useRowStyles()
  const deleteModalTriggerRef = React.useRef<HTMLButtonElement>(null)

  const onDeleteClicked = (e: React.MouseEvent) => {
    e.preventDefault()
    e.stopPropagation()
    deleteModalTriggerRef.current?.click()
  }

  const deleteModalTrigger = (
    <TextButton
      type='button'
      ref={deleteModalTriggerRef}
      hidden
      aria-label={t('editor_page.export_modal.delete_certificate_modal.header')}
    />
  )

  return (
    <div className={classes.certificateOption}>
      <FloatingMenuButton onClick={onSelect}>
        <div className={`${rowStyles.selectOption} ${classes.selectOption}`}>
          <div className={rowStyles.selectText}>{certificateName}</div>
        </div>
      </FloatingMenuButton>
      <div className={classes.actionButtons}>
        {isSelected && (
          <div className={rowStyles.checkmark}>
            <Icon block color='highlight' stroke='checkmark' />
          </div>
        )}
        <IconButton
          text={t('editor_page.export_modal.delete_certificate_modal.header')}
          stroke='delete12'
          size={0.875}
          onClick={onDeleteClicked}
          color='danger'
        />
      </div>
      <DeleteCertificateModal
        trigger={deleteModalTrigger}
        certificateUuid={certificateUuid}
        certificateName={certificateName}
        onDeleteSuccess={() => {
          onDeleteSuccess()
          collapse()
        }}
      />
    </div>
  )
}

export {
  CertificateOption,
}
