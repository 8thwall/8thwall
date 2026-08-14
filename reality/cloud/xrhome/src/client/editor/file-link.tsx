import React from 'react'
import {basename} from 'path'

import type {IAccount, IApp} from '../common/types/models'
import {useCurrentGit} from '../git/hooks/use-current-git'
import {useFileActionsContext} from './files/file-actions-context'
import {BoldButton} from '../ui/components/bold-button'

interface IFileLink {
  account: IAccount
  app: IApp
  file?: string
  line?: number
  column?: number
}

const makeFileString = (file: string, line?: number, column?: number) => {
  const lineString = line ? `:${line}` : ''
  const columnString = (line && column) ? `:${column}` : ''
  return `${basename(file)}${lineString}${columnString}`
}

const FileLink: React.FC<IFileLink> = ({account, app, file, line, column}) => {
  const actionsContext = useFileActionsContext()

  const fileExists = useCurrentGit(git => !!git.filesByPath[file])

  let linkText: string

  if (fileExists && account && app) {
    linkText = makeFileString(file, line, column)
  } else if (file) {
    linkText = basename(file)
  }

  if (!linkText) {
    return null
  }

  if (fileExists) {
    return (
      <BoldButton onClick={() => actionsContext.onSelect(file)}>
        {linkText}
      </BoldButton>
    )
  } else {
    // eslint-disable-next-line react/jsx-no-useless-fragment
    return <>{linkText}</>
  }
}

export default FileLink
