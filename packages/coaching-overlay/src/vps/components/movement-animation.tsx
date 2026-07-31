import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

interface IMovementAnimation {
  animationColor: string
  animationDuration: number
}

const MovementAnimation: FC<IMovementAnimation> = ({animationColor, animationDuration}) => (
  <div
    className='vps-coaching-animation'
    style={{
      // We want this to fade away 0.5 seconds before the other animations start
      // TODO(nathan): right now on Safari this does not fade away due to a Safari browser bug where
      // keyframes with transforms and opacity changes interact poorly causing it to not fade.
      animationDelay: `${animationDuration - 500}ms`,
    }}
  >
    {/* This div will look like a phone and will animate using css */}
    <div
      className='vps-coaching-phone-animation'
      style={{borderColor: `${animationColor}`}}
    />
  </div>
)

export {
  MovementAnimation,
}
