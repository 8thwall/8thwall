import React from 'react'
import type {UiRootType} from '@ecs/shared/scene-graph'

import {UiEntity} from './ui-entity'
import {OverlayUiContainer} from './ui-overlay-container'

const VIEWPORT_UI_SCALE = 0.01

interface IUiRoot {
  id: string
  outlined?: boolean
  type: UiRootType
}

const UiRoot: React.FC<IUiRoot> = ({id, outlined, type}) => (
  <mesh scale={[VIEWPORT_UI_SCALE, VIEWPORT_UI_SCALE, 1]}>
    {type === '3d'
      ? <UiEntity id={id} outlined={outlined} />
      : <OverlayUiContainer id={id} outlined={outlined} />
    }
  </mesh>
)

export {
  UiRoot,
  VIEWPORT_UI_SCALE,
}
