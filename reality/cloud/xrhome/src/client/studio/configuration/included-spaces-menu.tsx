import React from 'react'

import {useTranslation} from 'react-i18next'

import {useActiveSpace} from '../hooks/active-space'
import {useSceneContext} from '../scene-context'
import {RowMultiSelect} from './row-fields'
import {updateIncludedSpaces} from './included-spaces'

const IncludedSpacesMenu: React.FC = () => {
  const ctx = useSceneContext()

  const activeSpace = useActiveSpace()

  const {spaces} = ctx.scene
  const includedSpaces = activeSpace?.id ? spaces[activeSpace.id]?.includedSpaces : []
  const spacesToInclude = Object.values(spaces).filter(s => s.id !== activeSpace?.id).map(s => ({
    id: s.id,
    label: s.name,
  }))
  const selectedIds = [...(includedSpaces ?? [])]

  const {t} = useTranslation(['cloud-studio-pages'])

  const getDisplayText = (selected: string[]) => {
    if (selected.length === 0) {
      return t('space_configurator.include_spaces.placeholder')
    }
    return selected.map(id => spaces[id].name).join(', ')
  }

  return (
    <RowMultiSelect
      label={t('space_configurator.include_spaces.title')}
      options={spacesToInclude}
      selectedIds={selectedIds}
      onChange={spaceId => updateIncludedSpaces(spaceId, ctx, includedSpaces, activeSpace)}
      formIdLabel='included-spaces-menu'
      displayText={getDisplayText(selectedIds)}
    />
  )
}

export {
  IncludedSpacesMenu,
}
