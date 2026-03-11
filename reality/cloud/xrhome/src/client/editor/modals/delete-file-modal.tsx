import * as React from 'react'

import {useTranslation} from 'react-i18next'

import {StandardModal} from '../standard-modal'
import {StandardModalActions} from '../standard-modal-actions'
import {StandardModalHeader} from '../standard-modal-header'
import {BasicModalContent} from '../basic-modal-content'
import {LinkButton} from '../../ui/components/link-button'
import {TertiaryButton} from '../../ui/components/tertiary-button'
import {useDismissibleModal} from '../dismissible-modal-context'
import {Icon} from '../../ui/components/icon'

const DeleteFileModal = ({confirmSwitch, rejectSwitch, file}) => {
  useDismissibleModal(rejectSwitch)
  const {t} = useTranslation(['common'])

  return (
    <StandardModal onClose={rejectSwitch}>
      <StandardModalHeader>
        <Icon stroke='warning' size={2} />
        <h2>Delete?</h2>
      </StandardModalHeader>
      <BasicModalContent>
        <p>This will delete "{file}". Continue?</p>
      </BasicModalContent>
      <StandardModalActions>
        <LinkButton onClick={rejectSwitch}>Cancel</LinkButton>
        <TertiaryButton autoFocus onClick={confirmSwitch}>{t('button.delete')}</TertiaryButton>
      </StandardModalActions>
    </StandardModal>
  )
}

export default DeleteFileModal
