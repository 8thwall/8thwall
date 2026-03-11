import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from '@ecs/shared/scene-graph'

/* eslint quote-props: ["error", "as-needed"] */

const defaultScene: DeepReadonly<SceneGraph> = {
  objects: {
    '7e98572b-0ccc-4f7d-911d-5782b9e8c988': {
      id: '7e98572b-0ccc-4f7d-911d-5782b9e8c988',
      position: [
        0,
        0.5,
        0,
      ],
      rotation: [
        0,
        0,
        0,
        1,
      ],
      scale: [
        1,
        1,
        1,
      ],
      geometry: {
        type: 'box',
        depth: 1,
        height: 1,
        width: 1,
      },
      material: {
        type: 'basic',
        color: '#7611b6',
      },
      components: {},
      name: 'Box',
    },
    'ac1989e3-3b71-49e2-a05f-e682aeb18c36': {
      id: 'ac1989e3-3b71-49e2-a05f-e682aeb18c36',
      position: [
        20,
        20,
        10,
      ],
      rotation: [
        0,
        0,
        0,
        1,
      ],
      scale: [
        1,
        1,
        1,
      ],
      geometry: null,
      material: null,
      components: {},
      name: 'Directional Light',
      light: {
        type: 'directional',
        intensity: 1,
      },
    },
    '9c311ef2-d1e4-41df-aadd-f0b9a451fe5c': {
      id: '9c311ef2-d1e4-41df-aadd-f0b9a451fe5c',
      position: [
        0,
        0,
        0,
      ],
      rotation: [
        0,
        0,
        0,
        1,
      ],
      scale: [
        1,
        1,
        1,
      ],
      geometry: null,
      material: null,
      components: {},
      name: 'Ambient Light',
      light: {
        type: 'ambient',
      },
    },
  },
}

export {
  defaultScene,
}
