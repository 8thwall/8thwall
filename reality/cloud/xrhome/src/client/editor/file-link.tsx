import React from 'react'
import {Link} from 'react-router-dom'
import {createUseStyles} from 'react-jss'
import {basename} from 'path'
import type {LocationDescriptor} from 'history'

import type {IAccount, IApp} from '../common/types/models'
import {useCurrentGit} from '../git/hooks/use-current-git'
import {useAppPathsContext} from '../common/app-container-context'

const useStyles = createUseStyles({
  fileLink: {
    'textDecoration': 'underline',
    'color': 'inherit',
    '&:hover': {
      color: 'inherit',
    },
  },
})

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
  const classes = useStyles()
  const appPaths = useAppPathsContext()

  const fileExists = useCurrentGit(git => !!git.filesByPath[file])

  let link: LocationDescriptor
  let linkText: string

  if (fileExists && account && app) {
    // TODO(christoph): Add line/column to link
    link = appPaths.getFileRoute(file)
    linkText = makeFileString(file, line, column)
  } else if (file) {
    linkText = basename(file)
  }

  if (!linkText) {
    return null
  }

  if (link) {
    return (
      <Link className={classes.fileLink} to={link}>
        {linkText}
      </Link>
    )
  } else {
    // eslint-disable-next-line react/jsx-no-useless-fragment
    return <>{linkText}</>
  }
}

export default FileLink
