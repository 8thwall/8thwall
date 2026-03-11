import * as React from 'react'

import type {IAccount, IApp} from '../../common/types/models'
import FileLink from '../file-link'
import StackDisplay from './stack-display'
import type {ILog} from './types'

interface ILogItem extends ILog {
  account?: IAccount
  app?: IApp
  preview?: boolean
  as?: React.ElementType<{className: string, children: React.ReactNode}>
}

const LogItem: React.FunctionComponent<ILogItem> = ({
  account,
  app,
  numRedundant,
  timestamp,
  text,
  type,
  sourceLocation,
  stack,
  preview = false,
  as = 'li',
}) => {
  const [expanded, setExpanded] = React.useState(false)

  const date = new Date(timestamp)
  const formattedDate = date.toLocaleTimeString()
  const LogItemTag = as

  let textContent
  if (!preview && stack && stack.length > 1) {
    textContent = (
      <>
        <button
          type='button'
          className='style-reset log-expander'
          onClick={() => setExpanded(!expanded)}
        >
          {expanded ? '▼' : '▶'} {text}
        </button>
        {expanded && <StackDisplay stack={stack} app={app} account={account} />}
      </>
    )
  } else {
    textContent = preview ? text.split('\n')[0] : text
  }
  return (
    <LogItemTag className={`log-item ${type}`}>
      {numRedundant > 1 && <span className='redundant'>{numRedundant}</span>}
      <span className='text'>{textContent}</span>
      {(sourceLocation?.file) &&
        <span style={{'marginRight': '2em'}}>
          <FileLink
            account={account}
            app={app}
            file={sourceLocation.file}
            line={sourceLocation.line}
            column={sourceLocation.column}
          />
        </span>}
      <span className='time'>{formattedDate}</span>
    </LogItemTag>
  )
}

export default LogItem
