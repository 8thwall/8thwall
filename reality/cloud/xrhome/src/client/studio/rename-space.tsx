import React from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'
import type {Space} from '@ecs/shared/scene-graph'

import {StandardTextField} from '../ui/components/standard-text-field'
import {SrOnly} from '../ui/components/sr-only'
import {Keys} from './common/keys'

const useRenameSpaceState = (space: DeepReadonly<Space>) => {
  const [isRenaming, setIsRenaming] = React.useState(false)
  const [spaceName, setSpaceName] = React.useState('')
  const [justOpened, setJustOpened] = React.useState(false)

  const startRename = () => {
    if (!space) {
      return
    }

    setSpaceName(space.name)
    setIsRenaming(true)
  }

  return {
    isRenaming,
    spaceName,
    justOpened,
    startRename,
    setIsRenaming,
    setSpaceName,
    setJustOpened,
  }
}

interface IRenameSpaceForm {
  value: string
  renameState: ReturnType<typeof useRenameSpaceState>
  className?: string
  onRename: () => void
  onBlur: () => void
  onChange: (value: string) => void
  onEscapeKey: () => void
}

const RenameSpaceForm: React.FC<IRenameSpaceForm> = ({
  value, renameState, className, onRename, onBlur, onChange, onEscapeKey,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const onSubmit = (e: React.FormEvent) => {
    e.preventDefault()
    onRename()
  }

  return (
    <form onSubmit={onSubmit} className={className}>
      <StandardTextField
        id='rename-scene'
        label={<SrOnly>{t('tree_element_context_menu.button.rename_space')}</SrOnly>}
        height='tiny'
        value={value}
        onChange={(e) => {
          const name = e.target.value
          renameState.setSpaceName(name)
          onChange(name)
        }}
        onBlur={() => {
          if (!renameState.justOpened) {
            renameState.setIsRenaming(false)
          }
          renameState.setJustOpened(false)
          onBlur()
        }}
        ref={element => element && element.focus()}
        onKeyDown={(e) => {
          if (e.key === Keys.ESCAPE) {
            renameState.setIsRenaming(false)
            onEscapeKey()
          }
        }}
      />
    </form>
  )
}

export {
  useRenameSpaceState,
  RenameSpaceForm,
}
