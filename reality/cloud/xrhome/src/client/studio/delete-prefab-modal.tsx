import * as React from 'react'

import {useTranslation} from 'react-i18next'

import {StandardModal} from '../editor/standard-modal'
import {StandardModalActions} from '../editor/standard-modal-actions'
import {StandardModalHeader} from '../editor/standard-modal-header'
import {BasicModalContent} from '../editor/basic-modal-content'
import {Icon} from '../ui/components/icon'
import {SecondaryButton} from '../ui/components/secondary-button'
import {DangerButton} from '../ui/components/danger-button'

interface IPrefabDeleteModal {
  onClose: () => void
  onDelete: () => void
  onUnlink: () => void
}

const PrefabDeleteModal: React.FC<IPrefabDeleteModal> = ({
  onClose, onDelete, onUnlink,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  return (
    <StandardModal
      onClose={onClose}
    >
      <StandardModalHeader>
        <Icon stroke='warning' size={4} />
        <h2>{t('prefab_delete_modal.header')}</h2>
      </StandardModalHeader>
      <BasicModalContent>
        <p>
          {t('prefab_delete_modal.body')}
        </p>
      </BasicModalContent>
      <StandardModalActions>
        <SecondaryButton
          autoFocus
          onClick={onClose}
        >
          {t('prefab_delete_modal.option.cancel')}
        </SecondaryButton>
        <DangerButton
          onClick={onUnlink}
        >
          {t('prefab_delete_modal.option.unlink')}
        </DangerButton>
        <DangerButton
          onClick={onDelete}
        >
          {t('prefab_delete_modal.option.delete')}
        </DangerButton>
      </StandardModalActions>
    </StandardModal>
  )
}

export {
  PrefabDeleteModal,
}
