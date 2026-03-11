import React from 'react'

import {FloatingPanelButton, IFloatingPanelButton} from '../../ui/components/floating-panel-button'
import {RowContent} from './row-content'

const RowFloatingPanelButton: React.FC<IFloatingPanelButton> = props => (
  <RowContent>
    <FloatingPanelButton {...props} />
  </RowContent>
)

export {RowFloatingPanelButton}
