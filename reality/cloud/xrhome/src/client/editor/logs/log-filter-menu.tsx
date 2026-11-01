import * as React from 'react'

import {Icon} from '../../ui/components/icon'
import {ConsoleMenu, ConsoleMenuButton} from './console-menu'

interface ILogFilterMenu {
  inverted: boolean
  filterError: boolean
  filterWarn: boolean
  filterInfo: boolean
  onToggleError: () => void
  onToggleWarn: () => void
  onToggleInfo: () => void
  errorCount: number
  warnCount: number
  infoCount: number
}

const LogFilterMenu: React.FunctionComponent<ILogFilterMenu> = props => (
  <ConsoleMenu variant='subFilter' inverted={props.inverted}>
    <ConsoleMenuButton
      active={props.filterError}
      aria-pressed={props.filterError}
      onClick={props.onToggleError}
    >
      <Icon stroke='danger' color='danger' size={0.75} />{props.errorCount}
    </ConsoleMenuButton>
    <ConsoleMenuButton
      active={props.filterWarn}
      aria-pressed={props.filterWarn}
      onClick={props.onToggleWarn}
    >
      <Icon stroke='warning' color='warning' size={0.75} />{props.warnCount}
    </ConsoleMenuButton>
    <ConsoleMenuButton
      active={props.filterInfo}
      aria-pressed={props.filterInfo}
      onClick={props.onToggleInfo}
    >
      <Icon stroke='info' size={0.75} />{props.infoCount}
    </ConsoleMenuButton>
  </ConsoleMenu>
)

export default LogFilterMenu
