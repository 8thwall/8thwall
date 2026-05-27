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
import {createNewComponentFileContent} from '../file-browser-new-file-item'
import {useTreeElementStyles} from '../ui/tree-element-styles'

const useStyles = createUseStyles({
  createComponentOption: {
    height: '23px',
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

    <IgnoreKeys className={classes.createComponentOption}>
      <InlineTextInput
        value={newName}
        onChange={e => setNewName(e.target.value)}
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
            return
          }
          setEditing(false)
          onCollapse()
          onCreate(newName)
        }}
        inputClassName={combine('style-reset', treeElementClasses.renaming, classes.input)}
        aria-label={t('create_component_option.label.component_name')}
      />
    </IgnoreKeys>
  )
}

export {
  CreateComponentOption,
}
