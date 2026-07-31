// Parameters that we pass from the 8thWall modules to the React renderer

// The options that the user can specify.
type CoachingOverlayParameters = {
  promptColor: string
  animationColor: string
  promptText: string
  disablePrompt: boolean

  // The threshold value below only take effect if this is true
  autoByThreshold: boolean
  // Hide a currently shown overlay if percentage of sky pixel is above this threshold.
  showThreshold: number
  // Show a currently hidden overlay if percentage of sky pixel is below this threshold.
  hideThreshold: number
}

type ScanningData = {
  scanningStatus: SCANNING_STATUS
}

interface CoachingOverlayUiConfig extends CoachingOverlayParameters {
  shouldShow: boolean
}

enum SCANNING_STATUS {
  UNKNOWN = 'UNKNOWN',
  SKY_VISIBLE = 'SKY_VISIBLE',
  SKY_NOT_VISIBLE = 'SKY_NOT_VISIBLE',
}

const defaultParameters: CoachingOverlayParameters = {
  promptText: 'Point your phone towards the sky',
  promptColor: 'white',
  animationColor: 'white',
  disablePrompt: false,
  autoByThreshold: true,
  showThreshold: 0.1,
  hideThreshold: 0.05,
}

const defaultScanningData: ScanningData = {
  scanningStatus: SCANNING_STATUS.UNKNOWN,
}

export {
  CoachingOverlayParameters,
  CoachingOverlayUiConfig,
  ScanningData,
  defaultParameters,
  defaultScanningData,
  SCANNING_STATUS,
}
