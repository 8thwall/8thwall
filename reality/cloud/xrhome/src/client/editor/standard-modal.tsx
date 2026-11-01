import React from 'react'

import {bodySanSerif, tinyViewOverride} from '../static/styles/settings'
import {StandardModal as BaseStandardModal} from '../ui/components/standard-modal'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  standardModal: {
    color: theme.modalFg,
    backgroundColor: theme.modalBg,
    fontFamily: bodySanSerif,
    borderRadius: '8px',
    width: '700px',
    [tinyViewOverride]: {
      width: 'calc(100vw - 2em)',
      margin: '1em auto',
    },
  },
}))

interface IStandardModal {
  onClose?: () => void
  closeOnDimmerClick?: boolean
  children?: React.ReactNode
}

const StandardModal: React.FC<IStandardModal> = ({
  children, onClose, closeOnDimmerClick = true,
}) => {
  const classes = useStyles()
  return (
    <BaseStandardModal
      trigger='render'
      onOpenChange={(open) => {
        if (!open) {
          onClose?.()
        }
      }}
      closable={closeOnDimmerClick}
      width='unset'
    >
      <div className={classes.standardModal}>
        {children}
      </div>
    </BaseStandardModal>
  )
}

export {StandardModal}
