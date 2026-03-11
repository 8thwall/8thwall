import {useEnclosedApp} from '../apps/enclosed-app-context'
import {useSimulatorState} from '../editor/app-preview/use-simulator-state'

const useTargetResolution = () => {
  const app = useEnclosedApp()
  const simulatorState = useSimulatorState(app?.appKey)
  const {width, height, responsive, responsiveWidth, responsiveHeight} = simulatorState
  const targetWidth = (responsive && responsiveWidth) || width || window.innerWidth
  const targetHeight = (responsive && responsiveHeight) || height || window.innerHeight
  return {width: targetWidth, height: targetHeight}
}

export {
  useTargetResolution,
}
