/* eslint react-hooks/exhaustive-deps: error */

import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useEffect, useState} from 'preact/hooks'

import type {CoachingOverlayUiConfig} from '../parameters'
import {updateVisibility} from '../visibility'
import {MovementAnimation} from './movement-animation'

const CoachingFlow: FC<CoachingOverlayUiConfig> = (
  {trackingStatus, trackingReason, promptText, promptColor, animationColor, disablePrompt}
) => {
  const [visible, setVisible] = useState(false)
  const [classNames, setClassNames] = useState('')

  const {shouldUpdate, show} = updateVisibility(visible, trackingStatus, trackingReason)

  useEffect(() => {
    if (!shouldUpdate) {
      return undefined
    }

    if (show) {
      setVisible(true)
      setClassNames('coaching-overlay-fade-in')
    } else {
      setClassNames('coaching-overlay-fade-out')
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

  if (!disablePrompt && visible) {
    return (
      <div className='coaching-overlay-container'>
        <div className={`coaching-overlay-prompt-container ${classNames}`}>
          <MovementAnimation animationColor={animationColor} />
          <p className='coaching-overlay-prompt-status' style={{'color': promptColor}}>
            {promptText}
          </p>
        </div>
      </div>
    )
  }

  return <div />
}

export {
  CoachingFlow,
}
