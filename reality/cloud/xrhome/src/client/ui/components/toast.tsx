import React from 'react'

import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(() => ({
  toastContainer: {
    position: 'absolute',
    width: '100%',
    height: 0,
    top: '50%',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    zIndex: 1,
  },
  toast: {
    display: 'inline-flex',
    padding: '0.75rem',
    justifyContent: 'center',
    alignItems: 'center',
    borderRadius: 73,
    backgroundColor: 'rgba(0, 0, 0, 0.40)',
    maxWidth: '70%',
  },
  text: {
    color: '#FFFFFF',
    textAlign: 'center',
    fontSize: 18,
    fontWeight: 700,
    lineHeight: '24px',
    letterSpacing: 0.3,
  },
}))

type Props = {
  text: string
};

const ToastContainer = (props: { children: React.ReactNode }) => {
  const classes = useStyles()

  return (
    <div className={classes.toastContainer}>
      {props.children}
    </div>
  )
}

const ToastUI: React.FC<Props> = (props: Props) => {
  const classes = useStyles()

  return (
    <div className={classes.toast}>
      <span className={classes.text}>
        {props.text}
      </span>
    </div>
  )
}

const Toast: React.FC<Props> = (props: Props) => (
  <ToastContainer>
    <ToastUI {...props} />
  </ToastContainer>
)

export {
  Toast,
}
