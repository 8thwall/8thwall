import React from 'react'

import type {Fog} from '@ecs/shared/scene-graph'

interface ISceneFog {
  fog: Fog
}

const SceneFog: React.FC<ISceneFog> = ({fog}) => {
  if (!fog || fog.type === 'none') {
    return null
  }

  if (fog.type === 'linear') {
    return (
      <fog
        attach='fog'
        args={[fog.color, fog.near, fog.far]}
      />
    )
  }

  if (fog.type === 'exponential') {
    return (
      <fogExp2
        attach='fog'
        args={[fog.color, fog.density]}
      />
    )
  }

  return null
}

export {
  SceneFog,
}
