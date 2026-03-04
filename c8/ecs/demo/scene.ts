import type {SceneGraph} from '../src/shared/scene-graph'

const demoSceneGraph = {
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
      components: {},
      name: 'Middle',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/assets/robot-hvtbti382g.glb',
        },
        animationClip: 'Walking',
        loop: true,
      },
      order: 4.155397920014744,
      shadow: {
        castShadow: true,
      },
      parentId: '0b886cf4-3d24-49c4-82db-38b7bd3989ab',
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
      order: 1.6035986133431632,
      parentId: 'a3a95419-a145-402d-aa50-14e9deb516b5',
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
      order: 2.6035986133431632,
      parentId: 'a3a95419-a145-402d-aa50-14e9deb516b5',
    },
    '9409c834-a0ed-452e-a33c-57eabe11e0ae': {
      id: '9409c834-a0ed-452e-a33c-57eabe11e0ae',
      position: [
        -5,
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
      components: {
        '7c146945-b73a-42ec-8ad9-9214aa619787': {
          id: '7c146945-b73a-42ec-8ad9-9214aa619787',
          name: 'position-animation',
          parameters: {
            autoFrom: true,
            toZ: 3,
            reverse: true,
            toX: -5,
            toY: 0.5,
          },
        },
      },
      name: 'Left',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/assets/robot-hvtbti382g.glb',
        },
        animationClip: 'Walking',
        loop: true,
      },
      parentId: '0b886cf4-3d24-49c4-82db-38b7bd3989ab',
      order: 3.6035986133431632,
      shadow: {
        castShadow: true,
      },
    },
    'd656690b-e369-4d5f-92ac-25bf67214db5': {
      id: 'd656690b-e369-4d5f-92ac-25bf67214db5',
      position: [
        5,
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
      components: {
        '7aa3f7c2-de3f-42a9-b0c8-83dcc40e972a': {
          id: '7aa3f7c2-de3f-42a9-b0c8-83dcc40e972a',
          name: 'position-animation',
          parameters: {
            autoFrom: true,
            toY: 3,
            reverse: true,
            toX: 5,
          },
        },
      },
      name: 'Right',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/assets/robot-hvtbti382g.glb',
        },
        animationClip: 'Walking',
        loop: true,
      },
      parentId: '0b886cf4-3d24-49c4-82db-38b7bd3989ab',
      order: 4.603598613343163,
      shadow: {
        castShadow: true,
      },
    },
    'faad07bd-650e-4fd4-824e-ff6f2532e138': {
      id: 'faad07bd-650e-4fd4-824e-ff6f2532e138',
      position: [
        0,
        0,
        0,
      ],
      rotation: [
        -0.7071067999999993,
        0,
        0,
        0.7071067623730954,
      ],
      scale: [
        25.30000000000001,
        25.300001346275796,
        25.300001346275796,
      ],
      geometry: {
        type: 'plane',
        width: 1,
        height: 1,
      },
      material: {
        type: 'basic',
        color: '#FFFFFF',
      },
      parentId: 'a3a95419-a145-402d-aa50-14e9deb516b5',
      components: {},
      name: 'Plane',
      shadow: {
        receiveShadow: true,
      },
      order: 5.603598613343163,
    },
    'aeae230b-cd30-4293-bfae-c08cf692ae56': {
      id: 'aeae230b-cd30-4293-bfae-c08cf692ae56',
      position: [
        0,
        3.6041696925575275,
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
      components: {},
      name: 'Up',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/assets/robot-hvtbti382g.glb',
        },
        animationClip: '',
        loop: true,
      },
      order: 4.155397920014744,
      shadow: {
        castShadow: true,
      },
      parentId: '0b886cf4-3d24-49c4-82db-38b7bd3989ab',
    },
    '0b886cf4-3d24-49c4-82db-38b7bd3989ab': {
      id: '0b886cf4-3d24-49c4-82db-38b7bd3989ab',
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
      name: 'Models',
    },
    'a3a95419-a145-402d-aa50-14e9deb516b5': {
      id: 'a3a95419-a145-402d-aa50-14e9deb516b5',
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
      name: 'Env',
    },
    '036456aa-1367-4c23-92d5-1cd64f0bc41d': {
      id: '036456aa-1367-4c23-92d5-1cd64f0bc41d',
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
      name: 'Boxes',
    },
    'aa9da475-d686-48a9-8869-3a5a43ba23bb': {
      id: 'aa9da475-d686-48a9-8869-3a5a43ba23bb',
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
        color: '#af4141',
      },
      components: {},
      name: 'Middle (1)',
      order: 4.155397920014744,
      shadow: {
        castShadow: true,
      },
      parentId: '036456aa-1367-4c23-92d5-1cd64f0bc41d',
    },
    'cda5c006-d32e-4e95-99a1-6e6e29a5df98': {
      id: 'cda5c006-d32e-4e95-99a1-6e6e29a5df98',
      position: [
        -5,
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
        color: '#af4141',
      },
      components: {
        '7c146945-b73a-42ec-8ad9-9214aa619787': {
          id: '7c146945-b73a-42ec-8ad9-9214aa619787',
          name: 'position-animation',
          parameters: {
            autoFrom: true,
            toZ: 3,
            reverse: true,
            toX: -5,
            toY: 0.5,
          },
        },
      },
      name: 'Left (1)',
      parentId: '036456aa-1367-4c23-92d5-1cd64f0bc41d',
      order: 3.6035986133431632,
      shadow: {
        castShadow: true,
      },
    },
    '00c60772-f349-4af1-9973-69de01401711': {
      id: '00c60772-f349-4af1-9973-69de01401711',
      position: [
        5,
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
        color: '#af4141',
      },
      components: {
        '7aa3f7c2-de3f-42a9-b0c8-83dcc40e972a': {
          id: '7aa3f7c2-de3f-42a9-b0c8-83dcc40e972a',
          name: 'position-animation',
          parameters: {
            autoFrom: true,
            toY: 3,
            reverse: true,
            toX: 5,
          },
        },
      },
      name: 'Right (1)',
      parentId: '036456aa-1367-4c23-92d5-1cd64f0bc41d',
      order: 4.603598613343163,
      shadow: {
        castShadow: true,
      },
    },
    'b9b9dfe3-e489-46fe-ba36-7076d52edf2d': {
      id: 'b9b9dfe3-e489-46fe-ba36-7076d52edf2d',
      position: [
        0,
        3.6041696925575275,
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
        color: '#af4141',
      },
      components: {},
      name: 'Up (1)',
      order: 4.155397920014744,
      shadow: {
        castShadow: true,
      },
      parentId: '036456aa-1367-4c23-92d5-1cd64f0bc41d',
    },
  },
} as any as SceneGraph

export {
  demoSceneGraph,
}
