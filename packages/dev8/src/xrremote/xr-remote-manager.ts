import {initXrRemote, removeXrRemote} from './index'

const checkForThreeOnNextFrame = (resolve) => {
  if (window.THREE) {
    resolve()
  } else {
    requestAnimationFrame(() => {
      checkForThreeOnNextFrame(resolve)
    })
  }
}

const waitForThreeOnWindow = () => new Promise((resolve) => {
  checkForThreeOnNextFrame(resolve)
})

/**
 * Dependencies:
 * - THREE.JS
 */
const XrRemoteManager = () => {
  const waitForThree = waitForThreeOnWindow()

  const enable = async () => {
    await waitForThree

    initXrRemote()
  }

  const disable = () => {
    removeXrRemote()
  }

  return {
    enable,
    disable,
  }
}

export {
  XrRemoteManager,
}
