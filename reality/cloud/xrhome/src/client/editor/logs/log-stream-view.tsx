import React from 'react'

import WebsocketPool from '../../websockets/websocket-pool'
import editorActions from '../editor-actions'
import type {ILog, ILogStream} from './types'
import LogItem from './log-item'
import {useSelector} from '../../hooks'
import {useTheme} from '../../user/use-theme'
import useCurrentAccount from '../../common/use-current-account'
import useActions from '../../common/use-actions'
import {GetSnapshotBeforeUpdate} from '../../widgets/snapshot-before-update'
import {useEnclosedApp} from '../../apps/enclosed-app-context'
import {Keys} from '../../studio/common/keys'

interface ILogStreamView {
  logStream: ILogStream
  logFilter?: (log: ILog) => boolean
}

const LogStreamView: React.FC<ILogStreamView> = ({logStream, logFilter}) => {
  const app = useEnclosedApp()
  const account = useCurrentAccount()
  const themeName = useTheme()
  const [command, setCommand] = React.useState('')
  const [pendingCommand, setPendingCommand] = React.useState('')
  const [commandHistoryIndex, setCommandHistoryIndex] = React.useState(-1)
  const consoleInput = useSelector(s => s.editor.consoleInput)
  const {addConsoleInput, loadConsoleInputHistory} = useActions(editorActions)

  const logs = logFilter ? logStream.logs.filter(logFilter) : logStream.logs

  const scrollBoxRef = React.useRef(null)

  const scrollToBottom = () => {
    if (scrollBoxRef.current) {
      scrollBoxRef.current.scrollTop = 100000000
    }
  }

  React.useEffect(() => {
    loadConsoleInputHistory()
    scrollToBottom()
  }, [])

  const handleCommandSubmit = (e) => {
    e.preventDefault()

    // TODO(christoph): Fix this up
    const specifier = null

    // Add submitted command to list of previously submitted commands.
    addConsoleInput(command)

    WebsocketPool.broadcastMessage(specifier, {
      action: 'EVAL',
      data: {
        cmd: command,
      },
      FilterExpression: 'deviceId=:deviceIdVal',
      FilterValues: {
        ':deviceIdVal': logStream.deviceId,
      },
    })

    setCommand('')
    setPendingCommand('')
    setCommandHistoryIndex(-1)
  }

  const cycleInputHistory = async (e) => {
    switch (e.key) {
      case 'ArrowUp':
        if (commandHistoryIndex < consoleInput.commandHistory.length - 1) {
          e.preventDefault()
          const newCommand = consoleInput.commandHistory[commandHistoryIndex + 1]
          setCommand(newCommand)
          setPendingCommand(commandHistoryIndex === -1 ? command : pendingCommand)
          setCommandHistoryIndex(commandHistoryIndex + 1)
          e.target.setSelectionRange(newCommand.length, newCommand.length)
        }
        break
      case 'ArrowDown':
        if (commandHistoryIndex > -1) {
          e.preventDefault()
          const newCommand = commandHistoryIndex === 0
            ? pendingCommand
            : consoleInput.commandHistory[commandHistoryIndex - 1]
          setCommand(newCommand)
          setCommandHistoryIndex(commandHistoryIndex - 1)
          e.target.setSelectionRange(newCommand.length, newCommand.length)
        }
        break
      default:
    }
  }

  const handleInputKeyDown = (e) => {
    if (e.key === Keys.ENTER && !e.shiftKey) {
      handleCommandSubmit(e)
    } else {
      cycleInputHistory(e)
    }
  }

  return (
    <div
      ref={scrollBoxRef}
      className='log-list-container'
      a8='scroll;cloud-editor-console;console-scroll'
    >
      <ol className={`log-list ${themeName}`}>
        {logs.map(log => (
          <LogItem
            key={log.timestamp}
            account={account}
            app={app}
            {...log}
          />
        ))}
      </ol>

      {logStream.deviceId &&
        <form
          autoComplete='off'
          onSubmit={handleCommandSubmit}
          className={`command-form ${themeName}`}
        >
          <textarea
            name='command'
            className='style-reset'
            value={command}
            onChange={e => setCommand(e.target.value)}
            tabIndex={0}
            onKeyDown={handleInputKeyDown}
            rows={1}
          />
        </form>
      }
      <GetSnapshotBeforeUpdate
        getSnapshotBeforeUpdateCb={() => {
          const box = scrollBoxRef.current
          if (!box) {
            return true
          }
          const currentScrollBottom = box.scrollHeight - box.scrollTop - box.clientHeight
          return currentScrollBottom === 0
        }}
        componentDidUpdateCb={(prevProps, prevState, shouldScroll) => {
          if (shouldScroll) {
            scrollToBottom()
          }
        }}
      />
    </div>
  )
}

export {LogStreamView}
