import React from 'react'

import {Tooltip} from '../../ui/components/tooltip'
import {Icon} from '../../ui/components/icon'

const TOOLTIP_OPEN_DELAY = 200
const TOOLTIP_CLOSE_DELAY = 200

interface IComponentTooltip {
  content: React.ReactNode
}

const ComponentTooltip: React.FC<IComponentTooltip> = ({content}) => (
  <Tooltip
    content={content}
    position='top-end'
    openDelay={TOOLTIP_OPEN_DELAY}
    closeDelay={TOOLTIP_CLOSE_DELAY}
  >
    <Icon stroke='questionMark12' color='muted' block />
  </Tooltip>
)

export {ComponentTooltip}
