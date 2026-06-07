import {useQueryClient} from '@tanstack/react-query'
import React from 'react'
import {useHistory} from 'react-router-dom'
import {Trans, useTranslation} from 'react-i18next'

import {openDiskLocation} from '../studio/local-sync-api'
import {getLocalStudioPath} from './desktop-paths'
import {BoldButton} from '../ui/components/bold-button'
import {PrimaryButton} from '../ui/components/primary-button'
import {StandardModal} from '../ui/components/standard-modal'
import {StandardModalActions} from '../ui/components/standard-modal-actions'
import {StandardCheckboxField} from '../ui/components/standard-checkbox-field'
import {StandardModalContent} from '../ui/components/standard-modal-content'
import {StandardLink} from '../ui/components/standard-link'
import {StandardModalHeader} from '../editor/standard-modal-header'
import {useTypography} from '../ui/typography'
import AutoHeading from '../widgets/auto-heading'
import AutoHeadingScope from '../widgets/auto-heading-scope'

const getAlreadyConfirmedNonStudio = () => localStorage.getItem('confirmed-non-studio') === 'true'
const confirmNonStudio = () => localStorage.setItem('confirmed-non-studio', 'true')

interface IConfirmNonStudioModal {
  location: string
  onClose: () => void
}

const ConfirmNonStudioModal: React.FC<IConfirmNonStudioModal> = ({location, onClose}) => {
  const queryClient = useQueryClient()
  const history = useHistory()
  const typography = useTypography()
  const {t} = useTranslation(['studio-desktop-pages', 'common'])

  const [neverShowAgain, setNeverShowAgain] = React.useState(false)
  const [loading, setLoading] = React.useState(false)

  const handleContinue = async () => {
    try {
      setLoading(true)
      if (neverShowAgain) {
        confirmNonStudio()
      }
      const {appKey, initialization, canceled} = await openDiskLocation({
        location,
        acceptNonStudio: true,
      })
      if (canceled) {
        return
      } else {
        queryClient.invalidateQueries({queryKey: ['listProjects']})
        if (initialization === 'v2') {
          history.push(getLocalStudioPath(appKey))
        }
      }
      onClose()
    } finally {
      setLoading(false)
    }
  }

  return (
    <AutoHeadingScope level={2}>
      <StandardModal
        width='narrow'
        trigger='render'
        onOpenChange={onClose}
      >
        <StandardModalHeader>
          <AutoHeading className={typography.heading4}>
            {t('confirm_non_studio_modal.heading')}
          </AutoHeading>

        </StandardModalHeader>
        <StandardModalContent>
          <p>
            <Trans
              ns='studio-desktop-pages'
              i18nKey='confirm_non_studio_modal.explanation'
              components={{
                1: <StandardLink newTab href='https://8th.io/non-studio-in-desktop' />,
              }}
            />

          </p>

          <StandardCheckboxField
            label={t('confirm_non_studio_modal.label.do_not_show_again')}
            checked={neverShowAgain}
            onChange={e => setNeverShowAgain(e.target.checked)}
          />

        </StandardModalContent>
        <StandardModalActions>
          <BoldButton onClick={onClose}>
            {t('button.cancel', {ns: 'common'})}
          </BoldButton>
          <PrimaryButton loading={loading} onClick={handleContinue}>
            {t('button.continue', {ns: 'common'})}
          </PrimaryButton>
        </StandardModalActions>
      </StandardModal>
    </AutoHeadingScope>
  )
}

export {
  getAlreadyConfirmedNonStudio,
  ConfirmNonStudioModal,
}
