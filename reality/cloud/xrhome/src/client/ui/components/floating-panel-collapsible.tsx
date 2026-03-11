import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import '../../static/styles/index.scss'
import {useSectionCollapsed} from '../../studio/hooks/collapsed-section'
import {ROW_PADDING} from '../../studio/configuration/row-styles'

const useStyles = createThemedStyles(theme => ({
  floatingCollapseContainer: {
    'border': theme.studioSectionBorder,
    'borderRadius': '0.25em',
    'display': 'flex',
    'flexDirection': 'column',
    'overflow': 'hidden',
    'marginTop': '8px',
    'marginBottom': '8px',
    'marginInline': ROW_PADDING,
    '&:first-child': {
      marginTop: 0,
    },
  },
  collapseHeader: {
    display: 'flex',
  },
  collapseButton: {
    'padding': '0.5em 0 0.5em 1em',
    'display': 'flex',
    'fontWeight': 700,
    'cursor': 'pointer',
    'justifyContent': 'space-between',
    'userSelect': 'none',
    'flex': 1,
    'background': theme.mainEditorPane,
    '&:hover': {
      background: theme.studioBtnHoverBg,
    },
  },
  collapseTitle: {
    display: 'flex',
    gap: '0.5em',
    alignItems: 'center',
  },
  titleText: {
    fontSize: '12px',
  },
  collapseContent: {
    padding: BuildIf.MIGRATE_PADDING_20250610 ? '0.75em 0em 0 0em' : '0.75em 0.75em 0 0.75em',
    fontSize: '12px',
    borderTop: theme.studioSectionBorder,
  },
  collapseOptions: {
    display: 'flex',
    alignItems: 'center',
  },
}))

interface IFloatingPanelCollapsible {
  topContent?: React.ReactNode
  children: React.ReactNode
  title: string
  defaultOpen?: boolean
  sectionId?: string
  a8?: string
}

const FloatingPanelCollapsible: React.FC<IFloatingPanelCollapsible> = ({
  a8, children, title, topContent, defaultOpen = true, sectionId = '',
}) => {
  const classes = useStyles()
  const [isCollapsed, setCollapsed] = useSectionCollapsed(sectionId, defaultOpen)

  const onToggleCollapse = () => {
    setCollapsed(!isCollapsed)
  }

  return (
    <div className={classes.floatingCollapseContainer}>
      <div className={classes.collapseHeader}>
        <button
          a8={a8}
          type='button'
          className={combine(classes.collapseButton, 'style-reset')}
          onClick={onToggleCollapse}
          tabIndex={0}
        >
          <div className={classes.collapseTitle}>
            <span className={classes.titleText}>{title}</span>
          </div>
        </button>
        <div className={classes.collapseOptions}>
          {topContent}
        </div>
      </div>
      {!isCollapsed && <div className={classes.collapseContent}>{children}</div>}
    </div>
  )
}

export {
  FloatingPanelCollapsible,
}
