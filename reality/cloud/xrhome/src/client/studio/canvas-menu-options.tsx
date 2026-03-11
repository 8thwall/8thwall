import React from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../ui/theme'
import {useStudioStateContext} from './studio-state-context'
import {MenuOptions} from './ui/option-menu'

const useStyles = createThemedStyles(theme => ({
  shortcutLabel: {
    flexGrow: 1,
    whiteSpace: 'nowrap',
  },
  shortcutKey: {
    color: theme.fgMuted,
    whiteSpace: 'nowrap',
    marginLeft: '1em',
  },
}))

const CanvasMenuOptions: React.FC<{collapse: () => void}> = ({collapse}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const stateCtx = useStudioStateContext()
  const {state: {showUILayer, showGrid}} = stateCtx
  /* eslint-disable-next-line local-rules/hardcoded-copy */
  const isMac = navigator.userAgent.includes('Mac')

  const options = [
    {
      content: (
        <>
          <div className={classes.shortcutLabel}>
            {showUILayer
              ? t('canvas_context_menu.button.hide_ui')
              : t('canvas_context_menu.button.show_ui')}
          </div>
          {/* eslint-disable-next-line local-rules/hardcoded-copy */}
          <div className={classes.shortcutKey}>{isMac ? '⌘-\\' : 'Ctrl-\\'}</div>
        </>
      ),
      onClick: () => {
        stateCtx.update(p => ({...p, showUILayer: !p.showUILayer}))
      },
    },
    {
      content: showGrid
        ? t('canvas_context_menu.button.hide_grid')
        : t('canvas_context_menu.button.show_grid'),
      onClick: () => {
        stateCtx.update(p => ({...p, showGrid: !p.showGrid}))
      },
    },
  ]
  return <MenuOptions options={options} collapse={collapse} />
}

export {
  CanvasMenuOptions,
}
