import React from 'react'
import {createUseStyles} from 'react-jss'
import {Popup} from 'semantic-ui-react'

import {gray3} from '../static/styles/settings'
import {useUiTheme} from '../ui/theme'

const MOUSE_ENTER_DELAY = 200
const MOUSE_LEAVE_DELAY = 300

interface ITooltipIcon {
  content: React.ReactNode
  wide?: boolean
  position?:
    | 'top left'
    | 'top right'
    | 'bottom right'
    | 'bottom left'
    | 'right center'
    | 'left center'
    | 'top center'
    | 'bottom center'
  hoverable?: boolean
}

const useStyles = createUseStyles({
  icon: {
    marginLeft: '4px',
    display: 'inline',
    border: 'none',
    verticalAlign: 'middle',
    textAlign: 'center',
    background: 'none',
    padding: '0',
  },
})

const TooltipIcon: React.FC<ITooltipIcon> = ({content, wide, position = 'top left', hoverable}) => {
  const theme = useUiTheme()
  const classes = useStyles()

  /* eslint-disable max-len */
  const trigger = (
    <button type='button' className={classes.icon}>
      <svg width='12' height='12' viewBox='0 0 12 12' fill='none' xmlns='http://www.w3.org/2000/svg'>
        <path fill={gray3} fillRule='evenodd' clipRule='evenodd' d='M12 6C12 9.31444 9.31348 12 6 12C2.68652 12 0 9.31444 0 6C0 2.68749 2.68652 0 6 0C9.31348 0 12 2.68749 12 6ZM6.16101 1.98387C4.84253 1.98387 4.00161 2.53928 3.34127 3.5264C3.25573 3.65429 3.28435 3.82677 3.40696 3.91974L4.24645 4.55627C4.37238 4.65177 4.5518 4.62905 4.64964 4.50494C5.08183 3.95676 5.37818 3.63888 6.036 3.63888C6.53025 3.63888 7.1416 3.95698 7.1416 4.43625C7.1416 4.79857 6.84249 4.98465 6.35448 5.25825L6.31342 5.28124C5.74983 5.59659 5.03226 5.99809 5.03226 6.96774V7.06452C5.03226 7.22485 5.16225 7.35484 5.32258 7.35484H6.67742C6.83775 7.35484 6.96774 7.22485 6.96774 7.06452V7.03227C6.96774 6.77193 7.25541 6.60592 7.61323 6.39943C8.20185 6.05974 8.98031 5.61051 8.98031 4.45161C8.98031 3.04834 7.5247 1.98387 6.16101 1.98387ZM4.8871 9.09678C4.8871 8.48311 5.38633 7.98387 6 7.98387C6.61367 7.98387 7.1129 8.48311 7.1129 9.09678C7.1129 9.71042 6.61367 10.2097 6 10.2097C5.38633 10.2097 4.8871 9.71042 4.8871 9.09678Z' />
      </svg>
    </button>
  )
  /* eslint-enable max-len */
  return (
    <Popup
      wide={wide}
      position={position}
      trigger={trigger}
      on={['hover', 'click', 'focus']}
      hoverable={hoverable}
      mouseLeaveDelay={MOUSE_LEAVE_DELAY}
      mouseEnterDelay={MOUSE_ENTER_DELAY}
      inverted={theme.semanticTooltipInverted}
    >
      <Popup.Content>
        {content}
      </Popup.Content>
    </Popup>
  )
}

export {TooltipIcon}
