import React from 'react'

import {Trans, useTranslation} from 'react-i18next'

import {useStudioStateContext} from './studio-state-context'
import {useSceneContext} from './scene-context'
import {combine} from '../common/styles'
import {TreeElement} from './tree-element'
import {DropTarget} from './ui/drop-target'
import {handleDrop} from './tree-drop-targets'
import {useTreeHierarchyStyles} from './ui/tree-hierarchy-styles'
import {IconButton} from '../ui/components/icon-button'
import {useDerivedScene} from './derived-scene-context'

// TODO: modify search results to work with isolated view
const IsolatedTreeHierarchy: React.FC = () => {
  const classes = useTreeHierarchyStyles()
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const {t} = useTranslation('cloud-studio-pages')
  const [hovering, setHovering] = React.useState(false)

  const selectedPrefab = derivedScene.getObject(stateCtx.state.selectedPrefab)

  return (
    <DropTarget
      as='div'
      className={combine(
        classes.treeHierarchy, hovering && classes.hovered, classes.paddingTop
      )}
      onHoverStart={() => setHovering(true)}
      onHoverStop={() => setHovering(false)}
      onDrop={(e) => {
        handleDrop(e, ctx, stateCtx, derivedScene, setHovering, stateCtx.state.selectedPrefab)
      }}
    >
      <div className={combine('style-reset', classes.paddingTop, classes.isolationHeader)}>
        <div className={classes.backButton}>
          <IconButton
            onClick={() => {
              stateCtx.update(p => ({...p, selectedPrefab: undefined}))
            }}
            stroke='chevronLeft'
            inline
            text={t('tree_hierarchy_modes.isolation.chevron_icon_exit.text')}
            size={0.6}
          />
        </div>
        <div className={classes.isolationHeaderText}>
          <Trans
            ns='cloud-studio-pages'
            i18nKey='tree_hierarchy_modes.isolation.header'
            className={classes.isolationHeaderText}
            components={{
              i: <i />,
              name: <span className={classes.prefabName} />,
            }}
            values={{name: selectedPrefab.name}}
          />
        </div>
      </div>
      <TreeElement id={stateCtx.state.selectedPrefab} />
    </DropTarget>
  )
}

export {
  IsolatedTreeHierarchy,
}
