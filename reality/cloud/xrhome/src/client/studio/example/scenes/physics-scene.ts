import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from '@ecs/shared/scene-graph'

/* eslint quote-props: ["error", "as-needed"] */

const physicsScene: DeepReadonly<SceneGraph> = {
  objects: {
    'd7978445-1cf3-4b52-9840-c2e9af2e1ae3': {
      id: 'd7978445-1cf3-4b52-9840-c2e9af2e1ae3',
      position: [
        -1.7541523789077474e-16,
        3.4000000000000004,
        -0.79,
      ],
      rotation: [
        0,
        0,
        1,
        6.123233995736766e-17,
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
            interval: 250,
            size: 0.3,
            physics: true,
            radius: 0.05,
          },
        },
      },
      name: 'bucket',
      hidden: false,
      collider: null,
      gltfModel: {
        src: {
          type: 'url',
          url: 'https://static.8thwall.app/assets/Bucket-lqhello57c.glb',
        },
        loop: false,
      },
    },
    '80dc8028-f860-45bc-bf49-709fd93f24ad': {
      id: '80dc8028-f860-45bc-bf49-709fd93f24ad',
      position: [
        0,
        0,
        6.000000000000001,
      ],
      rotation: [
        0,
        0,
        1.1102230246251568e-16,
        1,
      ],
      scale: [
        0.9999999999999999,
        0.9999999999999999,
        1,
      ],
      geometry: {
        type: 'box',
        depth: 1,
        height: 10,
        width: 100,
      },
      material: {
        type: 'basic',
        color: '#248f79',
      },
      components: {},
      gltfModel: null,
      name: 'rightwall',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
      },
      parentId: 'ad3d69ab-7d7f-4349-a7da-10dc2ab9fe43',
    },
    '72900c5c-f326-4c34-8ef1-5fc042a058ad': {
      id: '72900c5c-f326-4c34-8ef1-5fc042a058ad',
      position: [
        -1.0436096431476472e-16,
        2.5,
        -0.47000000000000003,
      ],
      rotation: [
        -0.16065614185727,
        -0.6886142636364255,
        0.1606561418572701,
        0.6886142636364257,
      ],
      scale: [
        0.1,
        0.1,
        0.1,
      ],
      geometry: {
        type: 'box',
        depth: 4.5,
        height: 0.8,
        width: 5.7,
      },
      material: {
        type: 'basic',
        color: '#aa2c6d',
      },
      components: {},
      gltfModel: null,
      name: 'platform',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
        mass: 0,
      },
      parentId: '62aa1279-8f6f-488c-b78b-3f9f6156b6f7',
    },
    '7dcfb290-96ce-45b9-b701-95e7b7bfc480': {
      id: '7dcfb290-96ce-45b9-b701-95e7b7bfc480',
      position: [
        0,
        1,
        0.4,
      ],
      rotation: [
        0,
        0,
        0,
        1,
      ],
      scale: [
        0.1,
        0.1,
        0.1,
      ],
      geometry: null,
      material: {
        type: 'basic',
        color: '#0f4d2d',
      },
      components: {
        '98df41eb-471b-4837-89fd-bbb2ef0bfcf4': {
          id: '98df41eb-471b-4837-89fd-bbb2ef0bfcf4',
          name: 'spin',
          parameters: {
            speed: 0.2,
            z: 0,
            x: 1,
          },
        },
      },
      gltfModel: null,
      name: 'pusher',
      hidden: false,
      collider: null,
    },
    '867d747b-8fcc-40d2-975b-57cbaf73cdf4': {
      id: '867d747b-8fcc-40d2-975b-57cbaf73cdf4',
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
      geometry: {
        type: 'box',
        depth: 11,
        height: 1,
        width: 11,
      },
      material: {
        type: 'basic',
        color: '#59b757',
      },
      components: {},
      gltfModel: null,
      name: 'vane1',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
      },
      parentId: '7dcfb290-96ce-45b9-b701-95e7b7bfc480',
    },
    '8ea5f8d1-d660-42f9-9b91-d5d0667a6388': {
      id: '8ea5f8d1-d660-42f9-9b91-d5d0667a6388',
      position: [
        -1.8651746813702632e-16,
        2.8,
        -0.8400000000000001,
      ],
      rotation: [
        0.10272872340162417,
        -0.699604752262499,
        -0.10272872340162419,
        0.6996047522624991,
      ],
      scale: [
        0.1,
        0.1,
        0.1,
      ],
      geometry: {
        type: 'box',
        depth: 4.5,
        height: 0.8,
        width: 5.7,
      },
      material: {
        type: 'basic',
        color: '#aa2c6d',
      },
      components: {},
      gltfModel: null,
      name: 'platform',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
        mass: 0,
      },
      parentId: '62aa1279-8f6f-488c-b78b-3f9f6156b6f7',
    },
    'b04f57ef-a518-46db-8a5b-41c304489164': {
      id: 'b04f57ef-a518-46db-8a5b-41c304489164',
      position: [
        -2.020605904817785e-16,
        2.2,
        -0.91,
      ],
      rotation: [
        0.24962241249260292,
        -0.6615804192850425,
        -0.249622412492603,
        0.6615804192850427,
      ],
      scale: [
        0.1,
        0.1,
        0.1,
      ],
      geometry: {
        type: 'box',
        depth: 4.5,
        height: 0.8,
        width: 5.7,
      },
      material: {
        type: 'basic',
        color: '#aa2c6d',
      },
      components: {},
      gltfModel: null,
      name: 'platform',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
        mass: 0,
      },
      parentId: '62aa1279-8f6f-488c-b78b-3f9f6156b6f7',
    },
    'ad3d69ab-7d7f-4349-a7da-10dc2ab9fe43': {
      id: 'ad3d69ab-7d7f-4349-a7da-10dc2ab9fe43',
      position: [
        -1.1990408665951691e-16,
        0.9999999999999996,
        0,
      ],
      rotation: [
        0.21263110997159385,
        -0.6743797232066279,
        -0.2126311099715939,
        0.6743797232066279,
      ],
      scale: [
        0.1,
        0.1,
        0.1,
      ],
      geometry: {
        type: 'box',
        depth: 100,
        height: 1,
        width: 100,
      },
      material: {
        type: 'basic',
        color: '#452e99',
      },
      components: {},
      gltfModel: null,
      name: 'slope',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
      },
    },
    '95e54186-71ca-4c4a-b779-8c426b582f60': {
      id: '95e54186-71ca-4c4a-b779-8c426b582f60',
      position: [
        0,
        0,
        -6.000000000000001,
      ],
      rotation: [
        0,
        0,
        5.551115123125783e-17,
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
        height: 10,
        width: 100,
      },
      material: {
        type: 'basic',
        color: '#a8561f',
      },
      components: {},
      gltfModel: null,
      name: 'lefttwall',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
      },
      parentId: 'ad3d69ab-7d7f-4349-a7da-10dc2ab9fe43',
    },
    '6b68e50f-d003-4e85-851e-7cad6b4ef829': {
      id: '6b68e50f-d003-4e85-851e-7cad6b4ef829',
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
        0.1,
        0.1,
        0.1,
      ],
      geometry: {
        type: 'box',
        depth: 100,
        height: 1,
        width: 100,
      },
      material: {
        type: 'basic',
        color: '#d5bf4d',
      },
      components: {},
      gltfModel: null,
      name: 'floor',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
      },
    },
    '62aa1279-8f6f-488c-b78b-3f9f6156b6f7': {
      id: '62aa1279-8f6f-488c-b78b-3f9f6156b6f7',
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
      name: 'platforms',
    },
    '5fc091f4-ab6d-4326-a01c-fb60ccd9a319': {
      id: '5fc091f4-ab6d-4326-a01c-fb60ccd9a319',
      position: [
        0,
        0,
        0,
      ],
      rotation: [
        0.7071067811865475,
        0,
        0,
        0.7071067811865476,
      ],
      scale: [
        1,
        1,
        1,
      ],
      geometry: {
        type: 'box',
        depth: 11,
        height: 1,
        width: 11,
      },
      material: {
        type: 'basic',
        color: '#7936b0',
      },
      components: {},
      gltfModel: null,
      name: 'vane2',
      hidden: false,
      collider: {
        geometry: {
          type: 'auto',
        },
      },
      parentId: '7dcfb290-96ce-45b9-b701-95e7b7bfc480',
    },
    'a86dbc40-5ea9-47fb-ad42-d589ae01c24d': {
      id: 'a86dbc40-5ea9-47fb-ad42-d589ae01c24d',
      position: [
        -1.5,
        0,
        4,
      ],
      rotation: [
        0,
        0,
        0,
        1,
      ],
      scale: [
        3,
        0.3,
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
        color: '#e6620a',
      },
      components: {
        '8d5ffd49-5e5d-4b6f-871f-817a8cbeed8a': {
          id: '8d5ffd49-5e5d-4b6f-871f-817a8cbeed8a',
          name: 'custom-property-animation',
          parameters: {
            attribute: 'position',
            property: 'x',
            autoFrom: true,
            to: 1.5,
            duration: 4000,
            loop: true,
            reverse: true,
            easeIn: false,
            easeOut: false,
          },
        },
      },
      name: 'slider',
      collider: {
        geometry: {
          type: 'auto',
        },
      },
    },
    '93eb610b-81f3-4dc5-8afa-9845515d6c9b': {
      id: '93eb610b-81f3-4dc5-8afa-9845515d6c9b',
      position: [
        10,
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
      name: 'directional-light',
      light: {
        type: 'directional',
        intensity: 1,
      },
    },
    '76984540-844f-48d8-8775-983bcda6be82': {
      id: '76984540-844f-48d8-8775-983bcda6be82',
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
        intensity: 0.4,
      },
    },
  },
}

export {
  physicsScene,
}
