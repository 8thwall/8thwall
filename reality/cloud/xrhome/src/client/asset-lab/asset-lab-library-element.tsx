import React from 'react'

import {createCustomUseStyles} from '../common/create-custom-use-styles'
import {AssetContextMenu, useAssetContextMenuOptions} from './widgets/asset-lab-asset-context-menu'
import {gray4} from '../static/styles/settings'
import {ContextMenu, useContextMenuState} from '../studio/ui/context-menu'
import {useAssetLabStateContext} from './asset-lab-context'
import {SquareAssetWithIcon} from './widgets/square-asset'
import {combine} from '../common/styles'
import type {UiTheme} from '../ui/theme'
import type {RefScroller} from '../hooks/use-ref-scroller'

const useLibraryElementStyles = createCustomUseStyles<{size: number}>()((theme: UiTheme) => ({
  assetContainer: {
    'position': 'relative',
    '&:hover $contextMenuButton': {
      opacity: 1,
      pointerEvents: 'auto',
    },
    '&:hover $squareAssetButton': {
      boxShadow: `2px 2px 0 ${gray4}`,
      borderRadius: '0.25rem',
    },
    'color': theme.fgMuted,
  },
  contextMenuButton: {
    position: 'absolute',
    top: ({size}) => (size > 100 ? '5px' : '3px'),
    right: ({size}) => (size > 100 ? '5px' : '3px'),
    opacity: 0,
    pointerEvents: 'none',
  },
  contextMenuButtonActive: {
    opacity: 1,
    pointerEvents: 'auto',
  },
  squareAssetButton: {
    width: '100%',
    cursor: 'pointer',
  },
  selectedSquareAssetButton: {
    boxShadow: `2px 2px 0 ${gray4}`,
    borderRadius: '0.25rem',
  },
}))

interface IAssetLabLibraryElement {
  id: string
  hoveredAssetId: string | null
  setHoveredAssetId: (id: string | null) => void
  contextMenuActive: boolean
  setContextMenuActive: (active: boolean) => void
  size?: number
  librarySquareRefScroller?: RefScroller<string>
  onClick?: () => void
}

const AssetLabLibraryElement: React.FC<IAssetLabLibraryElement> = ({
  id, hoveredAssetId, setHoveredAssetId, contextMenuActive, setContextMenuActive,
  size = 180, librarySquareRefScroller, onClick,
}) => {
  const assetLabCtx = useAssetLabStateContext()
  const classes = useLibraryElementStyles({size})
  const menuState = useContextMenuState()

  const menuOptions = useAssetContextMenuOptions(id)

  return (
    <div
      className={classes.assetContainer}
      key={id}
      ref={(el) => {
        librarySquareRefScroller?.setRef(id, el)
      }}
      onContextMenu={menuState.handleContextMenu}
      {...menuState.getReferenceProps()}
    >
      <button
        type='button'
        className={combine('style-reset', classes.squareAssetButton,
          hoveredAssetId === id && classes.selectedSquareAssetButton)}
        onClick={() => {
          onClick?.()
          assetLabCtx.setState({open: true, mode: 'detailView', assetGenerationUuid: id})
        }}
        disabled={contextMenuActive}
      >
        <SquareAssetWithIcon
          key={id}
          generationId={id}
          size={size}
        />
      </button>
      <div className={
        combine(classes.contextMenuButton,
          hoveredAssetId === id && contextMenuActive && classes.contextMenuButtonActive)}
      >
        <AssetContextMenu
          id={id}
          onOpenChange={(open) => {
            setHoveredAssetId(open ? id : null)
            setContextMenuActive(open)
          }}
        />
      </div>
      <ContextMenu
        menuState={menuState}
        options={menuOptions}
      />
    </div>
  )
}

export {
  AssetLabLibraryElement,
}
