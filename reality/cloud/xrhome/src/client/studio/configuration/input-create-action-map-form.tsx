import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {StandardTextField} from '../../ui/components/standard-text-field'
import {createThemedStyles} from '../../ui/theme'
import {LinkButton} from '../../ui/components/link-button'
import {SrOnly} from '../../ui/components/sr-only'

const useStyles = createThemedStyles(theme => ({
  saveContainer: {
    display: 'flex',
    justifyContent: 'space-between',
    marginTop: '0.5em',
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

interface IInputCreateActionMapForm {
  onAddMap: (name: string, sourceMap: string | null, isPreset: boolean) => void
  setCreatingNewMap: (isCreating: boolean) => void
  newActionMap: string
  setNewActionMap: (name: string) => void
  presetMap: string | null
  collapse: () => void
}

const InputCreateActionMapForm: React.FC<IInputCreateActionMapForm> = ({
  onAddMap, setCreatingNewMap, newActionMap, setNewActionMap, presetMap, collapse,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()

  return (
    <form
      onSubmit={(e) => {
        e.preventDefault()
        onAddMap(newActionMap, presetMap, true)
        setCreatingNewMap(false)
        setNewActionMap('')
        collapse()
      }}
    >
      <StandardTextField
        id='action-map-name'
        required
        label={<SrOnly>{t('input_user_action_maps.action_map_name.label')}</SrOnly>}
        value={newActionMap}
        onChange={e => setNewActionMap(e.target.value)}
        height='tiny'
      />
      <div className={classes.saveContainer}>
        <div className={classes.cancelButton}>
          <LinkButton onClick={() => {
            setCreatingNewMap(false)
          }}
          >
            {t('button.cancel', {ns: 'common'})}
          </LinkButton>
        </div>
        <FloatingPanelButton
          spacing='full'
          type='submit'
        >
          {t('button.save', {ns: 'common'})}
        </FloatingPanelButton>
      </div>
    </form>
  )
}

export {
  InputCreateActionMapForm,
}
