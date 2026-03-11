import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import '../../static/styles/index.scss'

const useStyles = createThemedStyles(theme => ({
  border: {
    borderBottom: theme.studioSectionBorder,
  },
  floatingSectionContainer: {
    display: 'flex',
    flexDirection: 'column',
    paddingTop: '0.5em',
  },
  header: {
    display: 'flex',
  },
  title: {
    display: 'flex',
    alignItems: 'center',
    padding: '0 0 0.5em 0.75em',
    fontWeight: 700,
    justifyContent: 'space-between',
    flex: 1,
    userSelect: 'none',
  },
  titleText: {
    fontSize: '12px',
  },
  content: {
    padding: BuildIf.MIGRATE_PADDING_20250610 ? '0em 0em 1em' : '0em 1em 1em',
    fontSize: '12px',
  },
  collapseOptions: {
    display: 'flex',
    alignItems: 'center',
  },
  noBottomPadding: {
    paddingBottom: '0 !important',
  },
}))

interface IFloatingTraySection {
  topContent?: React.ReactNode
  children: React.ReactNode
  title?: string
  borderBottom?: boolean
}

const FloatingTraySection: React.FC<IFloatingTraySection> = ({
  children, title, topContent, borderBottom = true,
}) => {
  const classes = useStyles()

  return (
    <div className={combine(classes.floatingSectionContainer, borderBottom && classes.border)}>
      {title &&
        <div className={classes.header}>
          <div className={classes.title}>
            <span className={classes.titleText}>{title}</span>
          </div>
          <div className={classes.collapseOptions}>
            {topContent}
          </div>
        </div>
      }
      <div className={combine(classes.content, !borderBottom && classes.noBottomPadding)}>
        {children}
      </div>
    </div>
  )
}

export {
  FloatingTraySection,
}
