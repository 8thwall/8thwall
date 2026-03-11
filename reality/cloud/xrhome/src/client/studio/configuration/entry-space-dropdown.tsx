import React from 'react'
import {useTranslation} from 'react-i18next'

import {useSceneContext} from '../scene-context'
import {RowSelectField, useStyles as useRowStyles} from './row-fields'

const EntrySpaceDropdown: React.FC = () => {
  const {t} = useTranslation('cloud-studio-pages')
  const ctx = useSceneContext()
  const rowClasses = useRowStyles()

  if (!ctx.scene.spaces) {
    return null
  }

  return (
    <div className={rowClasses.flexItem}>
      <RowSelectField
        id='entry-space-dropdown'
        label={t('entry_space_dropdown.label')}
        options={
          Object.values(ctx.scene.spaces).map(space => ({value: space.id, content: space.name}))
        }
        value={ctx.scene.entrySpaceId ?? ''}
        onChange={value => ctx.updateScene(s => ({...s, entrySpaceId: value}))}
      />
    </div>
  )
}

export {
  EntrySpaceDropdown,
}
