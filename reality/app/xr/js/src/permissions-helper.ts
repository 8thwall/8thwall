// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
//
// Provides helper functions for requesting permissions
//

import {XrPermissionsFactory} from './permissions'
import {ResourceUrls} from './resources'
import type {XrPermission} from './types/common'

let immersivePromptButton_ = null
let resumeImmersivePrompt_ = null
let promptBox_ = null
let didInjectPromptStyle_ = false
let didInjectImmersivePromptStyle_ = false

const injectPromptStyle = () => {
  if (didInjectPromptStyle_) {
    return
  }
  didInjectPromptStyle_ = true
  const styleElement = document.createElement('style')
  styleElement.textContent = `
  .prompt-box-8w {
    font-family: 'Nunito-SemiBold', 'Nunito', sans-serif;
    position: fixed;
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
    width: 15em;
    max-width: 100%;
    font-size: 20px;
    z-index: 888;
    background-color: white;
    filter: drop-shadow(0 0 3px #0008);
    overflow: hidden;
    border-radius: 10px;
    padding: 0.5em;
    color: #323232;
    text-align: center;
  }

  .prompt-box-8w * {
    font-family: inherit;
  }

  .prompt-box-8w p {
    margin: 0.5em 0.5em 1em;
  }

  .prompt-button-container-8w {
    display: flex;
  }

  .prompt-button-8w {
    flex: 1 0 0;
    min-width: 5em;
    text-align: center;
    color: white;
    background-color: #818199;
    font-size: inherit;
    font-family: inherit;
    display: block;
    outline: none;
    border: none;
    margin: 0;
    border-radius: 10px;
    padding: 0.37em;
  }

  .prompt-button-8w:not(:last-child) {
    margin-right: 0.5em;
  }

  .button-primary-8w {
    background-color: #7611B7;
  }
  `
  document.head.prepend(styleElement)
}

const injectImmersivePromptStyle = () => {
  if (didInjectImmersivePromptStyle_) {
    return
  }
  didInjectImmersivePromptStyle_ = true
  const styleElement = document.createElement('style')
  styleElement.textContent = `
  .immersive-background-8w {
    background:linear-gradient(rgb(70, 71, 102), rgb(45, 46, 67));
    position:absolute;
    top:0;
    left:0;
    width:100vw;
    height:100vh;
    z-index:888;
  }

  .immersive-enter-button-8w {
    color: white;
    background:
      linear-gradient(#464766, #464766) padding-box,
      linear-gradient(#D688FF, #AD50FF) border-box;
    border-radius: 15px;
    border: 4px solid transparent;
    padding: 0 15px;

    font-family: 'Nunito-SemiBold', 'Nunito', sans-serif;
    text-align: center;
    font-size: 3em;

    position: fixed;
    left: 50%;
    top: 50%;

    transform: translate(-50%, -50%);
    max-width: 100%;
    z-index: 888;
  }

  .immersive-enter-button-8w:hover {
    background: #2D2E43;
    border: 4px solid #FFF;
    transition: transform 125ms;
    transform: translate(-50%, -50%) scale(1.025);
  }

  .poweredby-img-8w {
    width: 35vw;
    max-width: 200px;
    position: fixed;
    bottom: 2%;
    left: 50%;
    transform: translateX(-50%);
  }

  .immersive-enter-button-8w:active {
    background: #8083A2;
  }

  `
  document.head.prepend(styleElement)
}

const hidePermissionPrompt = () => {
  if (promptBox_) {
    document.body.removeChild(promptBox_)
    promptBox_ = null
  }
}

const showPermissionPrompt = () => {
  injectPromptStyle()
  promptBox_ = document.createElement('div')
  promptBox_.classList.add('prompt-box-8w')

  const promptText = document.createElement('p')
  promptText.textContent = 'AR requires access to device motion sensors'
  promptBox_.appendChild(promptText)

  const promptButtonContainer = document.createElement('div')
  promptButtonContainer.classList.add('prompt-button-container-8w')

  const cancelButton = document.createElement('button')
  cancelButton.classList.add('prompt-button-8w')
  cancelButton.textContent = 'Cancel'
  promptButtonContainer.appendChild(cancelButton)

  const continueButton = document.createElement('button')
  continueButton.classList.add('prompt-button-8w', 'button-primary-8w')
  continueButton.textContent = 'Continue'

  promptButtonContainer.appendChild(continueButton)
  promptBox_.appendChild(promptButtonContainer)
  document.body.appendChild(promptBox_)

  return new Promise<void>((resolve, reject) => {
    cancelButton.addEventListener('click', () => {
      hidePermissionPrompt()
      const properties = {type: 'permission', permission: 'prompt', status: 'denied'}
      reject(Object.assign(new Error(), properties))
    }, {once: true})
    continueButton.addEventListener('click', () => {
      hidePermissionPrompt()
      resolve()
    }, {once: true})
  })
}

const hideImmersivePrompt = () => {
  if (resumeImmersivePrompt_) {
    document.body.removeChild(resumeImmersivePrompt_)
    resumeImmersivePrompt_ = null
  }
  if (immersivePromptButton_) {
    document.body.removeChild(immersivePromptButton_)
    immersivePromptButton_ = null
  }
}

// Immersive Session Request
const showImmersivePrompt = (sessionType) => {
  const el = document.getElementById('loadImage')
  if (el && el.parentNode) {
    el.parentNode.removeChild(el)
  }
  injectImmersivePromptStyle()
  immersivePromptButton_ = document.createElement('button')
  immersivePromptButton_.classList.add('immersive-enter-button-8w')
  immersivePromptButton_.textContent = sessionType === 'immersive-ar' ? 'Enter AR' : 'Enter VR'

  document.body.appendChild(immersivePromptButton_)

  return new Promise<void>((resolve) => {
    immersivePromptButton_.addEventListener('click', () => {
      hideImmersivePrompt()
      resolve()
    }, {once: true})
  })
}

const showResumeImmersivePrompt = (sessionType) => {
  injectImmersivePromptStyle()
  resumeImmersivePrompt_ = document.createElement('div')
  resumeImmersivePrompt_.classList.add('immersive-background-8w')

  const poweredBy = document.createElement('img')
  poweredBy.classList.add('poweredby-img-8w')
  poweredBy.src = ResourceUrls.resolvePoweredLogo()
  poweredBy.alt = 'Powered By 8th Wall'

  resumeImmersivePrompt_.appendChild(poweredBy)
  document.body.appendChild(resumeImmersivePrompt_)

  return showImmersivePrompt(sessionType)
}

type PermissionState = PermissionStatus['state'] | 'retry' | ''

// Generalized request for motion and orientation sensors.
const requestSensorPermission = async (eventName, eventClass): Promise<PermissionState> => {
  // A new feature of the spec allows sensor permission to be directly requested and queried.
  // If it's available, use it, otherwise check manually using an event listener.

  // iOS 13+ will resolve here, depending on the result of the request.
  // This eventually calls DeviceMotionEvent.requestPermission() or
  // DeviceOrientationEvent.requestPermission()
  if (eventClass && eventClass.requestPermission) {
    try {
      return await eventClass.requestPermission()
    } catch {
      return 'retry' as const
    }
  }

  // If the sensor isn't a requestable permission, let it continue through.
  // If the sensor is disabled in settings, xrextras will catch in it onStart.
  return 'granted'
}

// Requests geolocation permissions by attempting to get the current position.
// NOTE: We could return an reason here as getCurrentPosition() returns a useful failure enum.
const getCurrentLocationNoop = (): Promise<PermissionState> => new Promise((resolve) => {
  navigator.geolocation.getCurrentPosition(
    () => resolve('granted'),
    () => resolve('')
  )
})

// Generalized request for geo sensors.
const requestGeoPermission = async (): Promise<PermissionState> => {
  if (!navigator.geolocation) {
    return ''
  }

  if (navigator.permissions) {
    // If the experimental `navigator.permissions` is available then check the geolocation
    // permissions directly.
    const result = await navigator.permissions.query({name: 'geolocation'})

    if (result.state === 'granted') {
      return 'granted'
    } else if (result.state === 'denied') {
      return ''
    } else {
      return getCurrentLocationNoop()
    }
  } else {
    // Just request the location directly which will cause the permissions prompt to pop-up.
    return getCurrentLocationNoop()
  }
}

// Request a permission from the permissions enum.
// Requesting camera is a noop because it happens in a later step.
const requestPermission = (permission: XrPermission): Promise<PermissionState> | null => {
  switch (permission) {
    case XrPermissionsFactory().permissions().DEVICE_MOTION:
      return typeof DeviceOrientationEvent !== 'undefined'
        ? requestSensorPermission('devicemotion', DeviceMotionEvent)
        : null
    case XrPermissionsFactory().permissions().DEVICE_ORIENTATION:
      return typeof DeviceOrientationEvent !== 'undefined'
        ? requestSensorPermission('deviceorientation', DeviceOrientationEvent)
        : null
    case XrPermissionsFactory().permissions().DEVICE_GPS:
      return requestGeoPermission()
    default:
      return null
  }
}

export {
  PermissionState,
  requestPermission,
  showPermissionPrompt,
  hidePermissionPrompt,
  showImmersivePrompt,
  hideImmersivePrompt,
  showResumeImmersivePrompt,
}
