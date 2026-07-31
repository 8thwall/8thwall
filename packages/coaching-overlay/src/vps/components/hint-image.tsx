import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

interface IHintImage {
  hintImage: string
  wayspotName: string
  animationDuration: number
}

const HintImage: FC<IHintImage> = ({animationDuration, hintImage, wayspotName}) => (
  <img
    src={hintImage}
    alt={wayspotName}
    className='vps-coaching-hint-image'
    style={{animationDelay: `${animationDuration}ms`}}
  />
)

export {
  HintImage,
}
