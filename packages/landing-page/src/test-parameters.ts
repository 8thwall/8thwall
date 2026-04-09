import type {LandingParameters} from './parameters'

const makeCoverImage = (id: string) => `https://cdn.8thwall.com/apps/cover/${id}-preview-1200x630`

// These overrides can be used like:
//   ?portrait&nobackground&basic=0&textColor=blue
const overrides: Record<string, Partial<LandingParameters>> = {
  basic: {
    mediaSrc: makeCoverImage('2tau4rloja7unbxx43exgfu9sf24g6dvmuzdqm2thfuq5tylwocuxxro'),
    backgroundBlur: 1,
    backgroundSrc: makeCoverImage('2nxlhkz21uv6xzal0wkig7jtulvtn3bm4uxryr4kvf7u9uyyvmsebz1x'),
    logoSrc: 'https://placehold.co/450x350',
    logoAlt: 'Logo placeholder',
  },
  portrait: {
    mediaSrc: 'https://placehold.co/630x1200',
    mediaAlt: 'Portrait placeholder',
  },
  video: {
    mediaSrc: 'https://cdn.8thwall.com/web/assets/beach.mp4',
  },
  youtube: {
    mediaSrc: 'https://www.youtube.com/watch?v=qFLhGq0060w',
  },
  square: {
    mediaSrc: 'https://placehold.co/1200x1200',
    mediaAlt: 'Square placeholder',
  },
  squareport: {
    mediaSrc: 'https://placehold.co/1100x1200',
    mediaAlt: 'Squarish portrait placeholder',
  },
  widelogo: {
    logoSrc: 'https://placehold.co/350x150',
    logoAlt: 'Landscape logo image',
  },
  longprompt: {
    promptPrefix: 'Check out:',
    promptSuffix: 'It\'s the place to go for the coolest AR experiences!',
  },
  nomedia: {
    mediaSrc: undefined,
  },
  noblur: {
    backgroundBlur: 0,
  },
  noauto: {
    mediaAutoplay: false,
  },
  nologo: {
    logoSrc: null,
  },
  nobackground: {
    backgroundSrc: null,
  },
  helmet: {
    mediaSrc: 'https://threejs.org/examples/models/gltf/DamagedHelmet/glTF/DamagedHelmet.gltf',
  },
  ball: {
    mediaSrc: 'https://static.8thwall.app/assets/justBall-fgjafdc5t8.glb',
  },
  robot: {
    mediaSrc: 'https://static.8thwall.app/assets/robot-i1wciu9sai.glb',
    mediaAnimation: 'ThumbsUp',
  },
  chatbot: {
    mediaSrc: 'https://static.8thwall.app/assets/chatbot-sybjgsc3dg.glb',
  },
  car: {
    mediaSrc: 'https://static.8thwall.app/assets/car-1cap702sy5.glb',
  },
}

const toVal = (s: string) => {
  if (s === 'true') {
    return true
  } else if (s === 'false') {
    return false
  }
  return s
}

const getTestParameters = () => {
  const params = {} as Partial<LandingParameters>

  const p = new URLSearchParams(window.location.search)
  if (!p.has('basic')) {
    Object.assign(params, overrides.basic)
  }

  Array.from(p.keys()).forEach((param) => {
    params[param] = toVal(p.get(param))
    if (overrides[param] && p.get(param) !== '0') {
      Object.assign(params, overrides[param])
    }
  })

  return params
}

export {
  getTestParameters,
}
