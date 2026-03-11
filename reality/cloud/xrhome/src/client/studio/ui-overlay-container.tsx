import React from 'react'
import type {UiGraphSettings} from '@ecs/shared/scene-graph'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'
import {Box3, Box3Helper, Color, Vector3} from 'three'

import {useYogaParentContext} from './yoga-parent-context'
import {useUiFont} from './hooks/use-ui-font'
import {useUiBlock} from './use-ui'
import {mango} from '../static/styles/settings'
import {UiEntity} from './ui-entity'
import {useTargetResolution} from './use-target-resolution'
import {calculateUiContainerOffset} from './calculate-ui-container-offset'

interface IOverlayUiRoot {
  id: string
  outlined?: boolean
}

const OverlayUiContainer: React.FC<IOverlayUiRoot> = ({id, outlined = false}) => {
  const {layoutMetrics, overlayContainers} = useYogaParentContext()
  const containerId = overlayContainers.get(id)
  const containerLayoutMetric = layoutMetrics.get(containerId)
  const layoutMetric = layoutMetrics.get(id)

  const {width, height} = useTargetResolution()

  const rootSettings = React.useMemo<UiGraphSettings>(() => ({
    width,
    height,
    backgroundOpacity: 0,
    ignoreRaycast: true,
  }), [width, height])

  const font = useUiFont(UI_DEFAULTS.font)

  const uiBlock = useUiBlock(
    rootSettings,
    containerLayoutMetric,
    font.json,
    font.png,
    undefined,
    false,
    id,
    -1,
    0,
    false
  )

  const box = React.useMemo(() => new Box3(), [])
  const helper = React.useMemo(() => {
    const obj = new Box3Helper(box, new Color(mango))
    obj.raycast = () => {}
    return obj
  }, [box])

  React.useLayoutEffect(() => {
    box.setFromCenterAndSize(
      new Vector3(0, 0, 0),
      new Vector3(width, height, 0)
    )
  }, [box, width, height])

  const offset = React.useMemo(() => calculateUiContainerOffset(layoutMetric), [layoutMetric])

  return uiBlock && (
    <group position={offset}>
      <primitive object={helper} />
      <primitive object={uiBlock} />
      <UiEntity
        id={id}
        outlined={outlined}
        parentBlock={uiBlock}
      />
    </group>
  )
}

export {
  OverlayUiContainer,
}
