import {
  AspectOptionData,
  aspectOptionData,
  makeAspectRatioValue,
  RESPONSIVE_OPTION_DATA,
} from './app-preview-utils'
import {useSimulator} from './use-simulator-state'

type SimulatorDimension = {
  iframeWidth: number
  iframeHeight: number
  disableOrientationToggle: boolean
  optionData: AspectOptionData
  aspectRatioValue: string
}

export const useSimulatorDimension = (): SimulatorDimension => {
  const {simulatorState} = useSimulator()
  const {isLandscape, responsive, aspectOptionId: stateAspectOptionId} = simulatorState

  // Note(Dale): The first option is responsive which does not have width and height.
  // Explicitly find one with width and height.
  const optionDataFallback = aspectOptionData.find(({width, height}) => width && height)
  const deviceHeight = simulatorState?.height || optionDataFallback.height
  const deviceWidth = simulatorState?.width || optionDataFallback.width
  const aspectRatioValue = makeAspectRatioValue(deviceWidth, deviceHeight, stateAspectOptionId)
  let optionData

  if (stateAspectOptionId === 'responsive') {
    optionData = RESPONSIVE_OPTION_DATA
  } else {
    optionData = (aspectOptionData.find(
      ({width, height, name, aspectOptionId}) => {
        const matchingAspectRatioValue =
          makeAspectRatioValue(width, height) === aspectRatioValue
        const matchingAspectOptionId = aspectOptionId
          ? aspectOptionId === stateAspectOptionId
          : false
        const matchingName = name === aspectRatioValue

        return matchingAspectRatioValue ||
          matchingAspectOptionId ||
          matchingName
      }
    )) || optionDataFallback
  }

  const disableOrientationToggle = optionData.section === 'DESKTOP' || responsive
  const showLandScape = isLandscape && !disableOrientationToggle
  const iframeHeight = showLandScape ? deviceWidth : deviceHeight
  const iframeWidth = showLandScape ? deviceHeight : deviceWidth

  return {
    iframeHeight,
    iframeWidth,
    disableOrientationToggle,
    optionData,
    aspectRatioValue,
  }
}
