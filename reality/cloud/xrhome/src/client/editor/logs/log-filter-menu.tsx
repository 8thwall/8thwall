import * as React from 'react'
import {Menu, Icon} from 'semantic-ui-react'

interface ILogFilterMenu {
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
  <Menu className='sub-filter-menu'>
    <Menu.Item
      active={props.filterError}
      onClick={props.onToggleError}
    >
      <Icon name='times circle' color='red' />{props.errorCount}
    </Menu.Item>
    <Menu.Item
      active={props.filterWarn}
      onClick={props.onToggleWarn}
    >
      <Icon name='exclamation triangle' color='yellow' />{props.warnCount}
    </Menu.Item>
    <Menu.Item
      active={props.filterInfo}
      onClick={props.onToggleInfo}
    >
      <Icon name='info circle' />{props.infoCount}
    </Menu.Item>
  </Menu>
)

export default LogFilterMenu
