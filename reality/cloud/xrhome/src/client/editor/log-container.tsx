import * as React from 'react'
import {useTranslation} from 'react-i18next'

import editorActions from './editor-actions'
import RecencyIndicator from './recency-indicator'
import {SYSTEM_STREAM_NAME} from './logs/log-constants'
import LogFilterMenu from './logs/log-filter-menu'
import type {ILogStream} from './logs/types'
import LogSearchBox from './logs/log-search-box'
import {LogStreamView} from './logs/log-stream-view'
import {getLastLog, makeLogFilter, makeSystemFilter} from './logs/log-filter'
import {
  getAvailableStreams, useLogStreams, getMainLogStream,
} from './logs/use-log-streams'
import useActions from '../common/use-actions'
import {countType} from './logs/count-reducer'
import {useChangeEffect} from '../hooks/use-change-effect'
import {getSessionDisplayTitle} from './debug-session-info'
import {combine} from '../common/styles'
import {useDeviceBroadcast} from './hooks/use-device-broadcast'
import {SystemLogsMenu} from './logs/system-logs-menu'
import {
  ConsoleMenu, ConsoleMenuButton, ConsoleMenuCloseTab, ConsoleMenuItem, ConsoleMenuTabButton,
} from './logs/console-menu'
import {IconButton} from '../ui/components/icon-button'
import {StandardCheckboxField} from '../ui/components/standard-checkbox-field'
import {useTheme} from '../user/use-theme'

interface ILogContainer {
  logKey: string
  expanded: boolean
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
  autoExpand = true,
  extraTabContent,
}) => {
  const inverted = useTheme() === 'dark'
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
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

  const broadcast = useDeviceBroadcast()

  const handleToggleDebug = (stream: ILogStream) => {
    setLogStreamDebugHudStatus(key, stream.name, !stream.isDebugHudActive)
    broadcast(stream.deviceId, {
      action: 'DEBUG_HUD',
      enable: !stream.isDebugHudActive,
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
      <ConsoleMenu variant='device' inverted={inverted}>
        {availableStreams.map((ls) => {
          const active = expanded && currentStream.name === ls.name
          const lastLog = getLastLog(ls)
          return (
            <ConsoleMenuItem
              key={ls.name}
              active={active}
            >
              <ConsoleMenuTabButton
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
              </ConsoleMenuTabButton>
              {ls.name !== SYSTEM_STREAM_NAME &&
                <ConsoleMenuCloseTab>
                  <IconButton
                    size={0.75}
                    stroke='close'
                    text={t('button.close', {ns: 'common'})}
                    onClick={(e) => {
                      e.stopPropagation(); closeLogPane(ls.name)
                    }}
                  />
                </ConsoleMenuCloseTab>
              }
            </ConsoleMenuItem>
          )
        })}
        {extraTabContent}
      </ConsoleMenu>
      {expanded &&
        <>
          {(currentStreamIsSystem || currentIsDeviceStream) &&
            <ConsoleMenu inverted={inverted}>
              {currentStreamIsSystem
                ? <SystemLogsMenu />
                : <LogFilterMenu
                    inverted={inverted}
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
              <ConsoleMenuButton
                onClick={() => clearEditorLogStream(key, currentStream.name)}
              >
                {t('editor_page.log_container.button.clear')}
              </ConsoleMenuButton>
              {currentIsDeviceStream &&
                <>
                  <ConsoleMenuItem>
                    <StandardCheckboxField
                      label={t('editor_page.log_container.label.clear_on_run')}
                      checked={currentStream.isClearOnRunActive}
                      onChange={() => toggleIsClearOnRunActive(key, filterOptions.showPane)}
                    />
                  </ConsoleMenuItem>
                  <ConsoleMenuItem>
                    <LogSearchBox
                      value={filterOptions.searchString}
                      onChange={s => updateFilterOptions({searchString: s})}
                    />
                  </ConsoleMenuItem>
                  <ConsoleMenuItem>
                    <StandardCheckboxField
                      label={t('editor_page.log_container.label.debug_mode')}
                      checked={currentStream.isDebugHudActive}
                      onChange={() => handleToggleDebug(currentStream)}
                    />
                  </ConsoleMenuItem>
                </>
              }
            </ConsoleMenu>
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
