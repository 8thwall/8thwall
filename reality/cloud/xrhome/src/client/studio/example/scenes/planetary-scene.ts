import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from '@ecs/shared/scene-graph'

/* eslint quote-props: ["error", "as-needed"] */

const planetaryScene: DeepReadonly<SceneGraph> = {
  objects: {
    '4236a08e-a0b0-4661-9d7d-240e7a1ebf1d': {
      id: '4236a08e-a0b0-4661-9d7d-240e7a1ebf1d',
      position: [
        0,
        2,
        0,
      ],
      rotation: [
        0,
        0,
        0,
        1,
      ],
      scale: [
        0.13,
        0.13,
        0.13,
      ],
      geometry: null,
      material: null,
      components: {},
      name: 'sun',
    },
    'd7978445-1cf3-4b52-9840-c2e9af2e1ae3': {
      id: 'd7978445-1cf3-4b52-9840-c2e9af2e1ae3',
      position: [
        25,
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
      components: {
        xyz: {
          id: 'xyz',
          name: 'particle-spawner',
          parameters: {
            interval: 250,
            size: 0.3,
            radius: 0.02,
          },
        },
        spin: {
          id: 'spin',
          name: 'spin',
          parameters: {
            x: 0,
            y: 1,
            z: 0,
            speed: 0.25,
          },
        },
        orbit: {
          id: 'orbit',
          name: 'orbit',
          parameters: {
            radius: 25,
            duration: 40000,
            type: 'elliptical',
          },
        },
      },
      name: 'earth',
      parentId: '4236a08e-a0b0-4661-9d7d-240e7a1ebf1d',
    },
    'f3255dda-48c1-449b-9916-4401a38735ee': {
      id: 'f3255dda-48c1-449b-9916-4401a38735ee',
      position: [
        10,
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
        2,
        2,
        2,
      ],
      geometry: null,
      material: null,
      components: {
        xyz: {
          id: 'xyz',
          name: 'particle-spawner',
          parameters: {
            interval: 150,
            radius: 0.02,
          },
        },
        orbit: {
          id: 'orbit',
          name: 'orbit',
          parameters: {
            radius: 10,
            duration: 5000,
            type: 'circular',
          },
        },
      },
      name: 'moon',
      parentId: 'd7978445-1cf3-4b52-9840-c2e9af2e1ae3',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/assets/Moon-lowpoly-v3-yg6nmdc1uj.glb',
        },
        loop: false,
      },
    },
    'f9d7201f-5159-4c66-a243-cfe886fa564f': {
      id: 'f9d7201f-5159-4c66-a243-cfe886fa564f',
      position: [
        0,
        8,
        0,
      ],
      rotation: [
        0.47354985570022246,
        0.30313120355450945,
        0.773145020399135,
        0.29344298429152144,
      ],
      scale: [
        0.1,
        0.1,
        0.1,
      ],
      geometry: null,
      material: null,
      components: {
        xyz: {
          id: 'xyz',
          name: 'particle-spawner',
          parameters: {
            interval: 75,
            radius: 0.015,
          },
        },
        orbit: {
          id: 'orbit',
          name: 'orbit',
          parameters: {
            radius: 8,
            duration: 6000,
            type: 'vertical',
          },
        },
      },
      parentId: 'd7978445-1cf3-4b52-9840-c2e9af2e1ae3',
      name: 'satellite',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/assets/Satellite-v4-vgpljdc7ni.glb',
        },
        loop: false,
      },
    },
    '67aedee3-9b4b-4cc2-8c23-327067f91c24': {
      id: '67aedee3-9b4b-4cc2-8c23-327067f91c24',
      position: [
        0,
        0,
        0,
      ],
      rotation: [
        0,
        0,
        0,
        0,
      ],
      scale: [
        10,
        10,
        10,
      ],
      geometry: null,
      material: null,
      components: {},
      parentId: '4236a08e-a0b0-4661-9d7d-240e7a1ebf1d',
      name: 'sunmodel',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/download/assets/Sun-apd6yku3z5.glb?name=Sun.glb',
        },
      },
    },
    '73a3766b-3e41-478b-8692-b24e2155a1a0': {
      id: '73a3766b-3e41-478b-8692-b24e2155a1a0',
      position: [
        0,
        0,
        0,
      ],
      rotation: [
        0,
        0,
        0,
        0,
      ],
      scale: [
        0.3,
        0.3,
        0.3,
      ],
      geometry: null,
      material: null,
      components: {},
      parentId: 'd7978445-1cf3-4b52-9840-c2e9af2e1ae3',
      name: 'earthmodel',
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/download/assets/Earth-59l3h7i6f2.glb?name=Earth.glb',
        },
      },
    },
    'bcf50d1d-a791-4567-b1ba-6ad307b8420b': {
      id: 'bcf50d1d-a791-4567-b1ba-6ad307b8420b',
      position: [
        20,
        15,
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
      name: 'directional-light',
      light: {
        type: 'directional',
        intensity: 1,
        color: '#dbdeff',
      },
    },
    'a4e63194-1ca5-4d02-8e11-8c168184ded2': {
      id: 'a4e63194-1ca5-4d02-8e11-8c168184ded2',
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
      name: 'ambient-light',
      light: {
        type: 'ambient',
        intensity: 0.1,
      },
    },
  },
}

export {
  planetaryScene,
}
