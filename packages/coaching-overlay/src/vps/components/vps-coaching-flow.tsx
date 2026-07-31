/* eslint react-hooks/exhaustive-deps: error */

import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useEffect, useState} from 'preact/hooks'

import type {VpsCoachingOverlayUiConfig} from '../parameters'
import {updateVisibility} from '../visibility'
import {MovementAnimation} from './movement-animation'
import {HintImage} from './hint-image'

const VpsCoachingFlow: FC<VpsCoachingOverlayUiConfig> = (
  {
    trackingStatus, trackingReason, promptColor, animationColor, animationDuration,
    promptPrefix, promptSuffix, statusText, disablePrompt, wayspotConditionSatisfied,
    selectedWayspotName, selectedWayspotHintImage,
  }
) => {
  const [visible, setVisible] = useState(false)
  const [classNames, setClassNames] = useState('')

  const {shouldUpdate, show} = updateVisibility(
    visible, wayspotConditionSatisfied, trackingStatus, trackingReason
  )

  useEffect(() => {
    if (!shouldUpdate) {
      return undefined
    }

    if (show) {
      setVisible(true)
      setClassNames('vps-coaching-overlay-remove-blur')
    } else {
      setClassNames('vps-coaching-overlay-fade-out')
    }

    // Since the fade-in/fade-out animation is half a second, we will set the display to 'block' or
    // 'none'.  We typically would prefer to use CSS animations for work like this, but display is
    // not a CSS animatable attribute.
    const timeout = setTimeout(() => {
      // Remove the fade-in/fade-out classes.
      setClassNames('')

      if (!show) {
        // We're successfully completed the transition to a NORMAL status and the fade-out should be
        // complete.  Let's also remove the element from the screen.
        setVisible(false)
      }
    }, 500)

    // Clear the timeout in the effect cleanup.
    return () => {
      clearTimeout(timeout)
    }
  }, [shouldUpdate, show])

  // If there is not a selectedWayspotName, then we don't even have the coverage api response yet.
  // Don't render anything.
  if (selectedWayspotName && !disablePrompt && visible) {
    return (
      <div className='vps-coaching-overlay-fade-in'>
        <div
          className={`vps-coaching-overlay-container ${classNames}`}
          style={{
            color: promptColor,
            // We want the remove-blur class to start its animation after the animationDuration.
            // However, we don't want to delay the fade-out class's animation
            animationDelay: show ? `${animationDuration}ms` : '0ms',
          }}
        >
          <div className='vps-coaching-overlay-padding'>
            {/* The prompt-container takes up 50% of the vertical screen space and is centered */}
            <div className='vps-coaching-overlay-prompt-container'>
              <MovementAnimation
                animationColor={animationColor}
                animationDuration={animationDuration}
              />
              {/* If there is not a hintImage available, don't show a stock image or anything. Just
                  let the phone animate over the blurred background */}
              {selectedWayspotHintImage &&
                <HintImage
                  hintImage={selectedWayspotHintImage}
                  wayspotName={selectedWayspotName}
                  animationDuration={animationDuration}
                />
              }
            </div>
            {/* Shows the prompt text in the bottom 25% of the screen and fades in first. */}
            <div
              className='vps-coaching-overlay-prompt-text'
              style={{animationDelay: `${animationDuration - 500}ms`, color: promptColor}}
            >
              <p>{promptPrefix}</p>
              <p>{selectedWayspotName}</p>
              <p>{promptSuffix}</p>
            </div>
            {/* Shows the status text at the very bottom of the screen and fades in second */}
            <div className='vps-coaching-overlay-status-text-container'>
              <p
                className='vps-coaching-overlay-status-text'
                style={{animationDelay: `${animationDuration + 500}ms`, color: promptColor}}
              >
                {statusText}<span className='vps-coaching-overlay-ellipsis-animated' />
              </p>
            </div>
          </div>
        </div>
      </div>
    )
  }

  return <div />
}

export {
  VpsCoachingFlow,
}
