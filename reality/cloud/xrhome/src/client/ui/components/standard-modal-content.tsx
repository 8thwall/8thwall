import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'

const useStyles = createThemedStyles(theme => ({
  standardModalContent: {
    margin: '0 1.2rem',
  },
  fixed: {
    padding: '1.2rem 0',
    overflowY: 'auto',
    borderTop: theme.modalContentBorder,
    borderBottom: theme.modalContentBorder,
  },
  scroll: {
    'maxHeight': '50vh',
    'overflowY': 'auto',
    'backgroundColor': theme.modalContentBg,
    'padding': '1.2rem',
    'borderRadius': '6px',
    'border': theme.modalContentBorder,
    '&::-webkit-scrollbar, & *::-webkit-scrollbar': {
      width: '6px',
    },
    '&::-webkit-scrollbar-track, & *::-webkit-scrollbar-track': {
      backgroundColor: theme.scrollbarTrackBackground,
      borderRadius: '6px',
    },
    '&::-webkit-scrollbar-thumb, & *::-webkit-scrollbar-thumb': {
      'backgroundColor': theme.scrollbarThumbColor,
      '&:hover': {
        backgroundColor: theme.scrollbarThumbHoverColor,
      },
    },
  },
}))

interface IStandardModalContent {
  scroll?: boolean
  children: React.ReactNode
}

const StandardModalContent: React.FC<IStandardModalContent> = ({
  scroll, children,
}) => {
  const classes = useStyles()
  return (
    <div
      className={combine(
        classes.standardModalContent,
        scroll ? classes.scroll : classes.fixed
      )}
    >
      {children}
    </div>
  )
}

export {StandardModalContent}
