import * as React from 'react'
import {Button, Menu, Checkbox} from 'semantic-ui-react'
import {useTranslation} from 'react-i18next'

import WebsocketPool, {SocketSpecifier} from '../websockets/websocket-pool'
import editorActions from './editor-actions'
import RecencyIndicator from './recency-indicator'
import {SYSTEM_STREAM_NAME} from './logs/log-constants'
import LogFilterMenu from './logs/log-filter-menu'
import SystemFilterMenu from './logs/system-filter-menu'
import type {ILogStream} from './logs/types'
import LogSearchBox from './logs/log-search-box'
import {LogStreamView} from './logs/log-stream-view'
import {getLastLog, makeLogFilter, makeSystemFilter} from './logs/log-filter'
import {
  getAvailableStreams, useLogStreams, getMainLogStream,
} from './logs/use-log-streams'
import {useTheme} from '../user/use-theme'
import useActions from '../common/use-actions'
import {countType} from './logs/count-reducer'
import {useChangeEffect} from '../hooks/use-change-effect'
import {getSessionDisplayTitle} from './debug-session-info'
import {combine} from '../common/styles'

interface ILogContainer {
  logKey: string
  expanded: boolean
  clientSpecifier: SocketSpecifier
  toggleExpanded: () => void
  autoExpand?: boolean  // allows the console to auto-expand when a new log stream is added
  extraTabContent?: React.ReactNode
}

interface FilterOptions {
  showPane?: string
  filterError?: boolean
  filterWarn?: boolean
  filterInfo?: boolean
  filterBuild?: boolean
  filterRepo?: boolean
  searchString?: string
}

const LogContainer: React.FC<ILogContainer> = ({
  logKey: key,
  expanded,
  toggleExpanded,
  clientSpecifier,
  autoExpand = true,
  extraTabContent,
}) => {
  const themeName = useTheme() || 'dark'
  const {t} = useTranslation(['cloud-editor-pages'])
  const [filterOptions, setFilterOptions] = React.useState<FilterOptions>({
    showPane: null,
    filterError: true,
    filterWarn: true,
    filterInfo: true,
    filterBuild: true,
    filterRepo: true,
    searchString: '',
  })
  const logStreams = useLogStreams(key)
  const availableStreams = getAvailableStreams(logStreams)
  const mainLogStream = getMainLogStream(logStreams)
  const currentStream = (
    filterOptions.showPane && logStreams.find(ls => ls.name === filterOptions.showPane)
  ) || mainLogStream
  const {
    clearEditorLogStream, deleteEditorLogStream, setLogStreamDebugHudStatus,
    toggleIsClearOnRunActive,
  } = useActions(editorActions)

  const updateFilterOptions = (newState: FilterOptions) => (
    setFilterOptions(current => ({...current, ...newState}))
  )

  useChangeEffect(([previousLogStreams]) => {
    const recentStreamName = logStreams.slice(-1)?.[0]?.name || ''
    if (recentStreamName && previousLogStreams) {
      const isNewLogStream = !previousLogStreams.find(({name}) => name === recentStreamName)
      if (isNewLogStream) {
        updateFilterOptions({showPane: recentStreamName})
        if (autoExpand && !expanded) {
          toggleExpanded()
        }
      }
    }
  }, [logStreams])

  const handleToggleDebug = (stream: ILogStream, e: React.MouseEvent) => {
    e.preventDefault()

    setLogStreamDebugHudStatus(key, stream.name, !stream.isDebugHudActive)
    WebsocketPool.broadcastMessage(clientSpecifier, {
      action: 'DEBUG_HUD',
      data: {
        enable: !stream.isDebugHudActive,
      },
      FilterExpression: 'deviceId=:deviceIdVal',
      FilterValues: {
        ':deviceIdVal': stream.deviceId,
      },
    })
  }

  const closeLogPane = (logStreamName: string) => {
    deleteEditorLogStream(key, logStreamName)
  }

  let logFilter = null
  const currentStreamIsSystem = currentStream?.name === SYSTEM_STREAM_NAME
  const currentIsDeviceStream = !!currentStream?.deviceId
  if (currentStreamIsSystem) {
    logFilter = makeSystemFilter(filterOptions.filterBuild, filterOptions.filterRepo)
  } else if (currentIsDeviceStream) {
    logFilter = makeLogFilter(
      filterOptions.filterError,
      filterOptions.filterWarn,
      filterOptions.filterInfo,
      filterOptions.searchString
    )
  }

  return (
    <div
      className={combine('log-container', expanded && 'expanded')}
      a8='click;cloud-editor-console;console-click'
    >
      <Menu className='device-menu'>
        {availableStreams.map((ls) => {
          const active = expanded && currentStream.name === ls.name
          const lastLog = getLastLog(ls)
          return (
            <Menu.Item
              key={ls.name}
              className={active ? 'active' : ''}
              onClick={() => {
                if (active && expanded) {
                  toggleExpanded()
                } else {
                  updateFilterOptions({showPane: ls.name})
                  if (!expanded) {
                    toggleExpanded()
                  }
                }
              }}
            >
              <RecencyIndicator
                lastLogTime={lastLog?.timestamp}
                color={lastLog?.type === 'error' ? 'mango' : 'green'}
              />
              {getSessionDisplayTitle(ls.title, ls.name, availableStreams)}
              {ls.name !== SYSTEM_STREAM_NAME &&
                <Button
                  className='close-tab'
                  size='mini'
                  basic
                  compact
                  icon='close'
                  onClick={(e) => { e.stopPropagation(); closeLogPane(ls.name) }}
                />
                    }
            </Menu.Item>
          )
        })}
        {extraTabContent}
      </Menu>
      {expanded &&
        <>
          {(currentStreamIsSystem || currentIsDeviceStream) &&
            <Menu className='filter-menu'>
              {currentStreamIsSystem
                ? <SystemFilterMenu
                    filterBuild={filterOptions.filterBuild}
                    filterRepo={filterOptions.filterRepo}
                    onToggleBuild={
                        () => updateFilterOptions({filterBuild: !filterOptions.filterBuild})}
                    onToggleRepo={
                        () => updateFilterOptions({filterRepo: !filterOptions.filterRepo})}
                />
                : <LogFilterMenu
                    filterError={filterOptions.filterError}
                    filterWarn={filterOptions.filterWarn}
                    filterInfo={filterOptions.filterInfo}
                    onToggleError={
                        () => updateFilterOptions({filterError: !filterOptions.filterError})}
                    onToggleWarn={
                        () => updateFilterOptions({filterWarn: !filterOptions.filterWarn})}
                    onToggleInfo={
                        () => updateFilterOptions({filterInfo: !filterOptions.filterInfo})}
                    errorCount={countType(currentStream?.logs, 'error')}
                    warnCount={countType(currentStream?.logs, 'warn')}
                    infoCount={countType(currentStream?.logs, 'log')}
                />
                    }
              <Menu.Item onClick={() => clearEditorLogStream(key, currentStream.name)}>
                {t('editor_page.log_container.button.clear')}
              </Menu.Item>
              {currentIsDeviceStream &&
                <>
                  <Menu.Item>
                    <Checkbox
                      className={themeName}
                      label={t('editor_page.log_container.label.clear_on_run')}
                      checked={currentStream.isClearOnRunActive}
                      onClick={() => toggleIsClearOnRunActive(key, filterOptions.showPane)}
                    />
                  </Menu.Item>
                  <Menu.Item>
                    <LogSearchBox
                      value={filterOptions.searchString}
                      onChange={s => updateFilterOptions({searchString: s})}
                    />
                  </Menu.Item>
                  <Menu.Item>
                    <Checkbox
                      className={themeName}
                      label={t('editor_page.log_container.label.debug_mode')}
                      checked={currentStream.isDebugHudActive}
                      onClick={e => handleToggleDebug(currentStream, e)}
                    />
                  </Menu.Item>
                </>
              }
            </Menu>
          }
          <LogStreamView
            logStream={currentStream}
            logFilter={logFilter}
          />
        </>
      }
    </div>
  )
}

export {LogContainer}
