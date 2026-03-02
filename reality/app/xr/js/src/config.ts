import {singleton} from './factory'

const XrConfigFactory = singleton(() => {
  const cameraEnum = {
    FRONT: 'front',
    BACK: 'back',
    ANY: 'any',
  }

  const deviceEnum = {
    // Restrict camera pipeline on mobile-class devices, for example phones and tablets.
    MOBILE: 'mobile',
    MOBILE_AND_HEADSETS: 'mobile-and-headsets',
    // Start running camera pipeline without checking device capabilities. This may fail at some
    // point in the pipeline startup if a required sensor is not available at run time (for
    // example, a laptop has no camera).
    ANY: 'any',
  }

  const camera = () => ({...cameraEnum})
  const device = () => ({...deviceEnum})

  return {
    camera,
    device,
  }
})

export {
  XrConfigFactory,
}
