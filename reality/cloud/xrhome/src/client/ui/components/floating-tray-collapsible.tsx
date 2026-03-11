import React from 'react'

import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import '../../static/styles/index.scss'
import {useSectionCollapsed} from '../../studio/hooks/collapsed-section'
import {ContextMenu, useContextMenuState} from '../../studio/ui/context-menu'
import type {MenuOption} from '../../studio/ui/option-menu'
import {blueberry, persimmon} from '../../static/styles/settings'
import {Popup} from './popup'
import {Icon} from './icon'
import {
  renderDiffNoBanner,
} from '../../studio/configuration/diff-chip-default-renderers'
import type {ExpanseField} from '../../studio/configuration/expanse-field-types'
import type {DefaultableConfigDiffInfo} from '../../studio/configuration/diff-chip-types'
import {ExpanseFieldDiffChip} from '../../studio/configuration/expanse-field-diff-chip'

const useStyles = createThemedStyles(theme => ({
  floatingCollapseContainer: {
    borderBottom: theme.studioSectionBorder,
    display: 'flex',
    flexDirection: 'column',
  },
  collapseHeader: {
    display: 'flex',
  },
  collapseButton: {
    padding: '0.125em 0 0.125em 0.75em',
    display: 'flex',
    fontWeight: 700,
    cursor: 'pointer',
    justifyContent: 'space-between',
    userSelect: 'none',
    flex: 1,
    position: 'relative',
  },
  collapseTitle: {
    display: 'flex',
    alignItems: 'center',
  },
  titleText: {
    fontSize: '12px',
  },
  collapseContent: {
    padding: BuildIf.MIGRATE_PADDING_20250610 ? '0em 0em 1em' : '0em 1em 1em',
    fontSize: '12px',
  },
  collapsed: {
    display: 'none',
  },
  collapseOptions: {
    display: 'flex',
    alignItems: 'center',
  },
  overriddenTitle: {
    color: persimmon,
  },
  overriddenIcon: {
    color: blueberry,
    position: 'relative',
    left: '-4px',
    top: '1px',
  },
}))

interface IFloatingTrayCollapsible {
  topContent?: React.ReactNode
  children: React.ReactNode
  title: string
  defaultOpen?: boolean
  sectionId?: string
  contextMenuOptions?: MenuOption[]
  a8?: string
  overridden?: boolean
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<unknown, string[][]>>
}

const FloatingTrayCollapsible: React.FC<IFloatingTrayCollapsible> = ({
  children, title, topContent, defaultOpen = true, sectionId = '', contextMenuOptions, a8,
  overridden, expanseField,
}) => {
  const classes = useStyles()
  const {t} = useTranslation('cloud-studio-pages')
  const [isCollapsed, setCollapsed] = useSectionCollapsed(sectionId, defaultOpen)

  const menuState = useContextMenuState()

  const collapseContextMenu = () => {
    menuState.setContextMenuOpen(false)
  }

  const onToggleCollapse = () => {
    setCollapsed(!isCollapsed)
    collapseContextMenu()
  }

  return (
    <div
      className={classes.floatingCollapseContainer}
      ref={menuState.refs.setReference}
      {...menuState.getReferenceProps()}
    >
      <div className={classes.collapseHeader}>
        <button
          a8={a8}
          type='button'
          className={combine(classes.collapseButton, 'style-reset')}
          onClick={onToggleCollapse}
          onContextMenu={contextMenuOptions ? menuState.handleContextMenu : undefined}
          tabIndex={0}
        >
          <div className={classes.collapseTitle}>
            {expanseField && <ExpanseFieldDiffChip
              field={expanseField}
              defaultRenderers={{defaultRenderDiff: renderDiffNoBanner}}
            />}
            <span className={combine(classes.titleText, overridden && classes.overriddenTitle)}>
              {title}
            </span>
            {overridden &&
              <div className={classes.overriddenIcon}>
                <Popup
                  content={t('tree_element.icon.instance_overrides')}
                  position='top'
                  alignment='left'
                  size='tiny'
                  delay={250}
                >
                  <Icon stroke='filledCircle6' inline />
                </Popup>
              </div>
            }
          </div>
        </button>
        <div className={classes.collapseOptions}>
          {topContent}
        </div>
      </div>
      <div className={combine(classes.collapseContent, isCollapsed && classes.collapsed)}>
        {children}
      </div>
      {contextMenuOptions && <ContextMenu
        menuState={menuState}
        options={contextMenuOptions}
      />}
    </div>
  )
}

export {
  FloatingTrayCollapsible,
}
