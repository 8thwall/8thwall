import React from 'react'

import {useSceneContext} from './scene-context'
import {useStudioStateContext} from './studio-state-context'
import {DropTarget} from './ui/drop-target'
import {createThemedStyles} from '../ui/theme'
import {handleDrop} from './tree-drop-targets'
import {useDerivedScene} from './derived-scene-context'

interface LevelTreeElementCssProperties extends React.CSSProperties {
  '--level'?: number
}

const useStyles = createThemedStyles(theme => ({
  dropElementDivider: {
    height: '6px',  // Height of drop target
    background: 'transparent',
    position: 'relative',
    zIndex: 'calc(var(--level) + 1)',  // +1 z-index for each level down to ensure hover detection
    width: 'calc(100% - (1.25em * var(--level) + 1.5em))',  // width is reduced for each level down
    marginLeft: 'auto',
    marginTop: '-3px',
    marginBottom: '-3px',
  },
  hovering: {
    height: '2px',  // Height of hover indicator
    backgroundColor: theme.fgMain,
  },
}))

const TreeHierarchyDivider: React.FC<{ parentId: string, siblingId: string, level: number }> = ({
  parentId, siblingId, level,
}) => {
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const classes = useStyles()
  const [hovering, setHovering] = React.useState(false)

  return (
    <div style={{'--level': level} as LevelTreeElementCssProperties}>
      <DropTarget
        as='div'
        className={classes.dropElementDivider}
        onHoverStart={() => setHovering(true)}
        onHoverStop={() => setHovering(false)}
        onDrop={e => handleDrop(e, ctx, stateCtx, derivedScene, setHovering, parentId, siblingId)}
      >
        <div className={hovering ? classes.hovering : ''} />
      </DropTarget>
    </div>
  )
}

export {
  TreeHierarchyDivider,
}
