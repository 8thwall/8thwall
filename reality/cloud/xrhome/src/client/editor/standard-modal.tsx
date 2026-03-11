import React from 'react'
import {Modal} from 'semantic-ui-react'

import '../static/styles/standard-modal.scss'
import {bodySanSerif, tinyViewOverride} from '../static/styles/settings'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  standardModal: {
    // TODO (tri) remove this important hack when removing semantic
    color: `${theme.modalFg} !important`,
    backgroundColor: `${theme.modalBg} !important`,
    fontFamily: bodySanSerif,
    borderRadius: '8px',
    [tinyViewOverride]: {
      width: 'calc(100vw - 2em) !important',
      margin: '1em auto',
    },
  },
}))

interface IStandardModal {
  onClose?: () => void
  closeOnDimmerClick?: boolean
  size?: 'mini' | 'tiny' | 'small' | 'large' | 'fullscreen'
  children?: React.ReactNode
}

const StandardModal: React.FC<IStandardModal> = ({
  children, onClose, size, closeOnDimmerClick = true,
}) => (
  <Modal
    open
    onClose={onClose}
    className={useStyles().standardModal}
    closeOnDimmerClick={closeOnDimmerClick}
    size={size}
  >
    {children}
  </Modal>
)

export {StandardModal}
