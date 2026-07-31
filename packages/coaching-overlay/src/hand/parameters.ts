// Parameters that we pass from the 8thWall modules to the React renderer

// The options that the user can specify.
type CoachingOverlayParameters = {
  promptColor: string
  animationColor: string
  promptText: string
  disablePrompt: boolean
}

interface CoachingOverlayUiConfig extends CoachingOverlayParameters {
  shouldShow: boolean
}

const defaultParameters: CoachingOverlayParameters = {
  promptText: 'Point your phone towards your hand',
  promptColor: 'white',
  animationColor: 'white',
  disablePrompt: false,
}

export {
  CoachingOverlayParameters,
  CoachingOverlayUiConfig,
  defaultParameters,
}
