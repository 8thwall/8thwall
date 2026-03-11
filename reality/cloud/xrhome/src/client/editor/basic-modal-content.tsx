import React from 'react'

import {createThemedStyles} from '../ui/theme'
import {combine} from '../common/styles'

const useStyles = createThemedStyles(theme => ({
  basicModalContent: {
    color: theme.modalFg,
    display: 'flex',
    flexDirection: 'column',
    gap: '1rem',
    padding: '1.5rem',
    alignItems: 'center',
  },
}))

interface IBasicModalContent {
  children: React.ReactNode
  className?: string
}
const BasicModalContent: React.FC<IBasicModalContent> = ({children, className}) => {
  const classes = useStyles()
  return (
    <div className={combine(classes.basicModalContent, className)}>
      {children}
    </div>
  )
}

export {BasicModalContent}
