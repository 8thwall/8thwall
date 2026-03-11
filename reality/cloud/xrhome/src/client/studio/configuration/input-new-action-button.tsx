import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {Icon} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'
import {LinkButton} from '../../ui/components/link-button'
import {StandardTextField} from '../../ui/components/standard-text-field'
import {SrOnly} from '../../ui/components/sr-only'

const useStyles = createThemedStyles(theme => ({
  newActionBtnRow: {
    display: 'flex',
    gap: '0.5em',
    padding: '0.5em 0',
    alignItems: 'center',
  },
  cancelButton: {
    'textAlign': 'center',
    'width': '100%',
    '& button': {
      fontWeight: 400,
      fontSize: '12px',
      color: theme.fgMain,
    },
  },
}))

interface IInputNewActionButton {
  onNewAction: (actionName: string) => void
}

const InputNewActionButton: React.FC<IInputNewActionButton> = ({onNewAction}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()

  const [creatingNewAction, setCreatingNewAction] = React.useState<boolean>(false)
  const [newActionName, setNewActionName] = React.useState<string>('')

  return (
    <div>
      {!creatingNewAction &&
        <FloatingPanelButton
          spacing='full'
          onClick={() => { setCreatingNewAction(true) }}
          height='tiny'
        >
          <Icon stroke='plus' inline />
          {t('input_manager.button.new_action')}
        </FloatingPanelButton>}
      {creatingNewAction &&
        <div>
          <form
            onSubmit={(e) => {
              e.preventDefault()
              onNewAction(newActionName)
              setNewActionName('')
              setCreatingNewAction(false)
            }}
          >
            <StandardTextField
              id='new-action-name'
              required
              label={<SrOnly>{t('input_manager.new_action_name.label')}</SrOnly>}
              onChange={e => setNewActionName(e.target.value)}
              height='tiny'
            />
            <div className={classes.newActionBtnRow}>
              <div className={classes.cancelButton}>
                <LinkButton
                  onClick={() => {
                    setCreatingNewAction(false)
                    setNewActionName('')
                  }}
                >
                  {t('button.cancel', {ns: 'common'})}
                </LinkButton>
              </div>
              <FloatingPanelButton
                type='submit'
                spacing='full'
              >
                {t('button.save', {ns: 'common'})}
              </FloatingPanelButton>
            </div>
          </form>
        </div>
        }
    </div>
  )
}

export {
  InputNewActionButton,
}
