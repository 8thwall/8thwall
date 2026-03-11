import React from 'react'
import {createUseStyles} from 'react-jss'

import {useSectionCollapsed} from '../hooks/collapsed-section'
import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  floatingCollapseContainer: {
    display: 'flex',
    flexDirection: 'column',
  },
  collapseHeader: {
    display: 'flex',
  },
  collapseButton: {
    padding: '0.5em 0 0.5em',
    display: 'flex',
    fontWeight: 700,
    cursor: 'pointer',
    justifyContent: 'space-between',
    userSelect: 'none',
    flex: 1,
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
    fontSize: '12px',
  },
})

interface ICollapsibleSection {
  children: React.ReactNode
  title: string
  defaultClosed?: boolean
  sectionId?: string
}

const CollapsibleSection: React.FC<ICollapsibleSection> = ({
  children, title, defaultClosed, sectionId = '',
}) => {
  const classes = useStyles()
  const [isCollapsed, setCollapsed] = useSectionCollapsed(sectionId, !defaultClosed)

  const onToggleCollapse = () => {
    setCollapsed(!isCollapsed)
  }

  return (
    <div
      className={classes.floatingCollapseContainer}
    >
      <div className={classes.collapseHeader}>
        <button
          type='button'
          className={combine(classes.collapseButton, 'style-reset')}
          onClick={onToggleCollapse}
          tabIndex={0}
        >
          <div className={classes.collapseTitle}>
            <span className={classes.titleText}>{title}</span>
          </div>
        </button>
      </div>
      <div className={classes.collapseContent}>
        {!isCollapsed && children}
      </div>
    </div>
  )
}

export {
  CollapsibleSection,
}
