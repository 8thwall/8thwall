import React from 'react'

import {createUseStyles} from 'react-jss'

import {useTranslation, Trans} from 'react-i18next'

import {PrimaryButton} from '../ui/components/primary-button'
import AutoHeading from '../widgets/auto-heading'
import {FloatingTrayModal} from '../ui/components/floating-tray-modal'
import {useCodeEditorSelection} from './preferences-modal'
import {useAbandonableEffect} from '../hooks/abandonable-effect'
import {BoldButton} from '../ui/components/bold-button'
import {StandardDropdownField} from '../ui/components/standard-dropdown-field'
import {StandardLink} from '../ui/components/standard-link'
import {SrOnly} from '../ui/components/sr-only'
import {StandardModalHeader} from '../editor/standard-modal-header'
import {SpaceBetween} from '../ui/layout/space-between'

const useStyles = createUseStyles({
  firstTimeSetupModal: {
    maxWidth: '80vw',
    minHeight: '30vh',
    width: '600px',
    padding: '1rem',
  },
})

interface IFirstTimeSetupModal {
  onComplete: () => void
}

const FirstTimeSetupModal: React.FC<IFirstTimeSetupModal> = ({onComplete}) => {
  const [currentValue, visibleOptions, handleSelectionChange] = useCodeEditorSelection()
  const {t} = useTranslation(['studio-desktop-pages', 'common'])
  const classes = useStyles()

  const finishSetup = () => {
    fetch('preferences:///current', {
      method: 'PATCH',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        firstTimeStatus: 'complete',
      }),
    })

    onComplete()
  }

  return (
    <FloatingTrayModal
      startOpen
      onOpenChange={finishSetup}
      trigger={undefined}
    >
      {() => (
        <div className={classes.firstTimeSetupModal}>
          <StandardModalHeader>
            <AutoHeading>{t('first_time_setup_page.title.select_editor')}</AutoHeading>
          </StandardModalHeader>

          <form onSubmit={(e) => {
            e.preventDefault()
            finishSetup()
          }}
          >
            <SpaceBetween direction='vertical'>
              <p>
                <Trans
                  ns='studio-desktop-pages'
                  i18nKey='first_time_setup_page.text.recommend_with_link'
                  components={{
                    1: <StandardLink newTab href='https://code.visualstudio.com/download' />,
                  }}
                />
              </p>
              <StandardDropdownField
                label={<SrOnly>{t('first_time_setup_page.label.code_editor')}</SrOnly>}
                value={currentValue}
                placeholder={t('first_time_setup_page.input.placeholder.select_editor')}
                options={visibleOptions}
                onChange={handleSelectionChange}
              />
              <SpaceBetween centered direction='vertical'>
                <PrimaryButton type='submit' spacing='wide' disabled={!currentValue}>
                  {t('button.continue', {ns: 'common'})}
                </PrimaryButton>
                <BoldButton
                  onClick={finishSetup}
                >
                  {t('first_time_setup_page.button.decide_later')}
                </BoldButton>
              </SpaceBetween>
            </SpaceBetween>
          </form>
        </div>
      )}
    </FloatingTrayModal>
  )
}

const FirstTimeSetupView: React.FC = () => {
  const [setupVisible, setSetupVisible] = React.useState(false)

  useAbandonableEffect(async (abandon) => {
    try {
      const res = await abandon(fetch('preferences:///current'))
      if (!res.ok) {
        return
      }
      const prefs = await abandon(res.json())
      setSetupVisible(prefs.firstTimeStatus !== 'complete')
    } catch (e) {
      // eslint-disable-next-line no-console
      console.error('Failed to load preferences:', e)
    }
  }, [])

  if (setupVisible) {
    return (
      <React.Suspense fallback={null}>
        <FirstTimeSetupModal
          onComplete={() => setSetupVisible(false)}
        />
      </React.Suspense>
    )
  }

  return null
}

export {
  FirstTimeSetupView,
}
