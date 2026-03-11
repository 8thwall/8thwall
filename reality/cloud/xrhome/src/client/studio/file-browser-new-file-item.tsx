import React from 'react'

import icons from '../apps/icons'
import InlineTextInput from '../common/inline-text-input'
import FileListIcon from '../editor/file-list-icon'
import type {ItemType} from './file-browser-list'
import {useTreeElementStyles, IndentFileBrowserCssProperties} from './ui/tree-element-styles'
import {combine} from '../common/styles'

interface INewFileItem {
  itemType: ItemType
  onSubmit: Function
  onCancel: () => void
  level?: number
}

const createNewComponentFileContent = (name: string) => (
  // eslint-disable-next-line local-rules/hardcoded-copy
  `// This is a component file. You can use this file to define a custom component for your project.
// This component will appear as a custom component in the editor.

import * as ecs from '@8thwall/ecs'  // This is how you access the ecs library.

ecs.registerComponent({
  name: '${name}',
  // schema: {
  // },
  // schemaDefaults: {
  // },
  // data: {
  // },
  // add: (world, component) => {
  // },
  // tick: (world, component) => {
  // },
  // remove: (world, component) => {
  // },
  // stateMachine: ({world, eid, schemaAttribute, dataAttribute}) => {
  //   ecs.defineState('default').initial()
  // },
})
`
)

const NewFileItem: React.FC<INewFileItem> = ({itemType, onSubmit, onCancel, level = 0}) => {
  const classes = useTreeElementStyles()
  const [name, setName] = React.useState('')

  const handleSubmit = () => {
    if (itemType === 'folder') {
      onSubmit({name, isFile: false})
    } else if (itemType === 'file') {
      onSubmit({name, isFile: true})
    } else {
      const fileName = name.split('.')[0]
      onSubmit({
        name: `${fileName}.ts`,
        isFile: true,
        fileContent: createNewComponentFileContent(fileName),
      })
    }
  }

  return (
    <div className={classes.treeElementButton}>
      <div
        style={{'--file-level': level} as IndentFileBrowserCssProperties}
        className={classes.fileName}
      >
        <FileListIcon icon={itemType === 'folder' && icons.folder_empty} filename={name} />
        <InlineTextInput
          value={name}
          onChange={e => setName(e.target.value)}
          onCancel={onCancel}
          onSubmit={handleSubmit}
          inputClassName={combine('style-reset', classes.renaming)}
        />
      </div>
    </div>
  )
}

export {NewFileItem}

export type {INewFileItem}
