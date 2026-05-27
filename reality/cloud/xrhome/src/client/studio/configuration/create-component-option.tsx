import React from 'react'
import {IgnoreKeys} from 'react-hotkeys'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import InlineTextInput from '../../common/inline-text-input'

import {combine} from '../../common/styles'
import useActions from '../../common/use-actions'
import coreGitActions from '../../git/core-git-actions'
import {useGitRepo} from '../../git/hooks/use-current-git'

import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {TextNotification} from '../../ui/components/text-notification'
import {SpaceBetween} from '../../ui/layout/space-between'
import {createNewComponentFileContent} from '../file-browser-new-file-item'
import {useTreeElementStyles} from '../ui/tree-element-styles'

const useStyles = createUseStyles({
  createComponentOption: {
    height: '23px',
    flex: '1 0 0',
  },
  input: {
    width: '100%',
    padding: '0 0.25rem',
  },
})

interface ICreateComponentOption {
  onCollapse: () => void
  onCreate: (name: string) => void
}

const CreateComponentOption: React.FC<ICreateComponentOption> = ({onCreate, onCollapse}) => {
  const [editing, setEditing] = React.useState(false)
  const [newName, setNewName] = React.useState('')
  const [hasConflict, setHasConflict] = React.useState(false)
  const treeElementClasses = useTreeElementStyles()
  const {mutateFile} = useActions(coreGitActions)
  const repo = useGitRepo()
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()

  if (!editing) {
    return (
      <FloatingMenuButton
        onClick={(e) => {
          e.stopPropagation()
          setEditing(true)
        }}
      >
        {t('create_component_option.button.create_new')}
      </FloatingMenuButton>
    )
  }

  return (
    <IgnoreKeys>
      <SpaceBetween narrow>
        <InlineTextInput
          value={newName}
          onChange={(e) => {
            setHasConflict(false)
            setNewName(e.target.value)
          }}
          onCancel={() => setEditing(false)}
          onSubmit={async () => {
            if (!newName) {
              setEditing(false)
              return
            }
            const filePath = `${newName}.ts`
            let collided = false
            await mutateFile(repo, {
              filePath,
              transform: (c) => {
                collided = true
                return c.content
              },
              generate: () => createNewComponentFileContent(newName),
            })
            if (collided) {
              setHasConflict(true)
              return
            }
            setEditing(false)
            onCollapse()
            onCreate(newName)
          }}
          formClassName={classes.createComponentOption}
          inputClassName={combine('style-reset', treeElementClasses.renaming, classes.input)}
          aria-label={t('create_component_option.label.component_name')}
        />
        {hasConflict &&
          <TextNotification type='danger'>
            {t('create_component_option.error.file_conflict')}
          </TextNotification>
        }
      </SpaceBetween>
    </IgnoreKeys>
  )
}

export {
  CreateComponentOption,
}
