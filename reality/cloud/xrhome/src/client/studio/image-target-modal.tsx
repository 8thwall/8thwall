import React from 'react'

import {createThemedStyles, useUiTheme} from '../ui/theme'
import {FloatingTrayModal} from '../ui/components/floating-tray-modal'
import {Icon} from '../ui/components/icon'
import {PrimaryButton} from '../ui/components/primary-button'

const useStyles = createThemedStyles(theme => ({
  content: {
    background: theme.modalBg,
    color: theme.modalFg,
    borderRadius: '15px',
    whiteSpace: 'pre-wrap',
    display: 'flex',
    flexDirection: 'column',
    padding: '2em',
    justifyContent: 'center',
    alignItems: 'center',
    textAlign: 'center',
    gap: '1.5em',
  },
  header: {
    margin: 'auto',
  },
  buttonRow: {
    display: 'flex',
    gap: '1em',
    justifyContent: 'center',
  },
  cancelButton: {
    background: 'none',
    color: theme.modalFg,
    border: 'none',
    cursor: 'pointer',
  },
}))

interface IImageTargetModal {
  trigger: React.ReactNode
  onSubmit: (() => void) | (() => Promise<void>)
  onCancel?: () => void
  heading: string
  body: string
  submitLabel: string
  cancelLabel: string
  submitDisabled?: boolean
  submitLoading?: boolean
}

const ImageTargetModal: React.FC<IImageTargetModal> = ({
  trigger, onSubmit, onCancel, heading, body, submitLabel, cancelLabel, submitDisabled,
  submitLoading,
}) => {
  const classes = useStyles()
  const theme = useUiTheme()

  const submitThenCollapse = async (collapse) => {
    const result = onSubmit()
    if (result instanceof Promise) {
      await result
    }
    collapse()
  }

  return (
    <FloatingTrayModal trigger={trigger}>
      {collapse => (
        <div className={classes.content}>
          <Icon stroke='warning' color={theme.modalFg} size={2.5} />
          <h1 className={classes.header}>{heading}</h1>
          <p>{body}</p>
          <div className={classes.buttonRow}>
            <button
              type='button'
              className={classes.cancelButton}
              onClick={() => {
                onCancel?.()
                collapse()
              }}
            >
              {cancelLabel}
            </button>
            <PrimaryButton
              type='button'
              onClick={() => submitThenCollapse(collapse)}
              disabled={submitDisabled}
              loading={submitLoading}
            >
              {submitLabel}
            </PrimaryButton>
          </div>
        </div>
      )}
    </FloatingTrayModal>
  )
}

export {
  ImageTargetModal,
}
