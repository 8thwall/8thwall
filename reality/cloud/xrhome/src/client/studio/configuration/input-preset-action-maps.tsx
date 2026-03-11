import React from 'react'

import {ALL_PRESET_CATEGORIES} from './input-preset-map-strings'
import {SubMenuView} from '../ui/submenu'

interface IInputPresetActionMaps {
  setPresetsOpen: (isOpen: boolean) => void
  setCreatingNewMap: (isCreating: boolean) => void
  setNewActionMap: (name: string) => void
  setPresetMap: (name: string | null) => void
}

const InputPresetActionMaps: React.FC<IInputPresetActionMaps> = (
  {setPresetsOpen, setCreatingNewMap, setNewActionMap, setPresetMap}
) => {
  const handleChange = (newValue: string) => {
    if (!newValue) {
      setNewActionMap('')
    } else {
      setNewActionMap(newValue)
    }
    setPresetMap(newValue)
    setCreatingNewMap(true)
    setPresetsOpen(false)
  }

  return (
    <SubMenuView
      onOptionChange={handleChange}
      onCategoryChange={(newValue) => {
        if (newValue) {
          setPresetsOpen(true)
        } else {
          setPresetsOpen(false)
        }
      }}
      onCollapse={() => setPresetsOpen(false)}
      optionCategories={ALL_PRESET_CATEGORIES}
    />
  )
}

export {
  InputPresetActionMaps,
}
