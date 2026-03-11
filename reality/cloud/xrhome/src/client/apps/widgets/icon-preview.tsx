import * as React from 'react'
import {Image} from 'semantic-ui-react'
import '../../static/styles/pwa-settings.scss'

const ICON_DISPLAY_SHAPES = ['circle', 'drop', 'square', 'squircle'] as const
type IconDisplayShape = typeof ICON_DISPLAY_SHAPES[number]

interface IIconPreview {
  // Additional classes.
  className?: string

  // The shape of the app icon. Default is square.
  shape?: IconDisplayShape

  // The source of the icon image.
  src: string

  // Text to display below the icon.
  text?: string
}

const IconPreview: React.FunctionComponent<IIconPreview> =
  ({className, shape = 'square', src, text}) => {
    let classes = className || ''

    switch (shape) {
      case 'circle':
        classes = `${classes} circle`
        break
      case 'squircle':
        classes = `${classes} squircle`
        break
      case 'drop':
        classes = `${classes} drop`
        break
      default:
        break
    }

    return (
      <div className='icon-preview-container'>
        <div className='icon-preview'>
          <div className='icon-shadow'>
            <Image src={src} className={classes} />
          </div>
          {text && <p>{text}</p>}
        </div>
      </div>
    )
  }

export {
  IconPreview,
  ICON_DISPLAY_SHAPES,
  type IconDisplayShape,
}
