type OrbitIdle = 'none' | 'spin' | 'bounce'
type OrbitInteraction = 'none' | 'drag'
type AnimationName = 'none' | '*' | string
type MediaControlMode = 'none' | 'browser' | 'minimal'

type LandingParameters = {
  url: string
  promptPrefix: string
  promptSuffix: string
  vrPromptPrefix: string
  logoSrc: string
  logoAlt: string
  backgroundSrc: string
  backgroundColor: string
  backgroundBlur: number
  mediaSrc: string
  mediaAlt: string
  mediaAutoplay: boolean
  mediaAnimation: AnimationName
  mediaControls: MediaControlMode
  font: string
  textColor: string
  textShadow: boolean
  sceneEnvMap: string
  sceneOrbitIdle: OrbitIdle
  sceneOrbitInteraction: OrbitInteraction
  sceneLightingIntensity: number
}

const defaultParameters: LandingParameters = {
  url: '',
  promptPrefix: 'Scan or visit',
  promptSuffix: 'to continue',
  vrPromptPrefix: 'or visit',
  logoSrc: '',
  logoAlt: 'Logo',
  backgroundSrc: '',
  backgroundColor: 'linear-gradient(#464766, #2D2E43)',
  backgroundBlur: 0,
  mediaSrc: '',
  mediaAlt: 'Preview',
  mediaAutoplay: true,
  mediaAnimation: '',
  mediaControls: 'minimal',
  font: '\'Nunito-SemiBold\', \'Nunito\', sans-serif',
  textColor: 'white',
  textShadow: false,
  sceneEnvMap: 'field',
  sceneOrbitIdle: 'spin',
  sceneOrbitInteraction: 'drag',
  sceneLightingIntensity: 1,
}

// Default is first option
const KNOWN_ORBIT_IDLE: OrbitIdle[] = ['none', 'spin', 'bounce']
const KNOWN_ORBIT_INTERACTIONS: OrbitInteraction[] = ['drag', 'none']

// eslint-disable-next-line arrow-parens
const toKnown = <T>(known: Readonly<T[]>, value: any) => (
  known.includes(value) ? (value as T) : known[0]
)

const toNumber = (value: string | number, fallback: number) => {
  let n: number = NaN
  if (typeof value === 'string') {
    n = parseFloat(value)
  } else if (typeof value === 'number') {
    n = value
  }

  if (Number.isNaN(n)) {
    return fallback
  }

  return n
}

type InputParameters = Record<keyof LandingParameters, any>

const normalizeParameters = (input: InputParameters): LandingParameters => ({
  ...input,
  sceneOrbitIdle: toKnown(KNOWN_ORBIT_IDLE, input.sceneOrbitIdle),
  sceneOrbitInteraction: toKnown(KNOWN_ORBIT_INTERACTIONS, input.sceneOrbitInteraction),
  sceneLightingIntensity: toNumber(input.sceneLightingIntensity, 1),
})

export {
  AnimationName,
  OrbitIdle,
  OrbitInteraction,
  MediaControlMode,
  LandingParameters,
  defaultParameters,
  normalizeParameters,
}
