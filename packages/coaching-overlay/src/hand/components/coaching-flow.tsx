/* eslint react-hooks/exhaustive-deps: error */

import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useEffect, useState} from 'preact/hooks'

import type {CoachingOverlayUiConfig} from '../parameters'
import {MovementAnimation} from './movement-animation'

const CoachingFlow: FC<CoachingOverlayUiConfig> = (
  {shouldShow, promptText, promptColor, animationColor, disablePrompt}
) => {
  const [visible, setVisible] = useState(false)
  const [classNames, setClassNames] = useState('')

  useEffect(() => {
    if (shouldShow === visible) {
      return undefined
    }

    if (shouldShow) {
      setVisible(true)
      setClassNames('hand-coaching-overlay-fade-in')
    } else {
      setClassNames('hand-coaching-overlay-fade-out')
    }

    // Since the fade-in/fade-out animation is half a second, we will set the display to 'block' or
    // 'none'.  We typically would prefer to use CSS animations for work like this, but display is
    // not a CSS animatable attribute.
    const timeout = setTimeout(() => {
      // Remove the fade-in/fade-out classes.
      setClassNames('')

      // We're successfully completed the transition to a NORMAL status and the fade-out should be
      // complete.  Let's also remove the element from the screen.
      setVisible(shouldShow)
    }, 500)

    // Clear the timeout in the effect cleanup.
    return () => {
      clearTimeout(timeout)
    }
  }, [shouldShow, visible])

  if (!disablePrompt && visible) {
    return (
      <div className='hand-coaching-overlay-container'>
        <div className={`hand-coaching-overlay-prompt-container ${classNames}`}>
          <MovementAnimation animationColor={animationColor} />
          <p className='hand-coaching-overlay-prompt-status' style={{'color': promptColor}}>
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
