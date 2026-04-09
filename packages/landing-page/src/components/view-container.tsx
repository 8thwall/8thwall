import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

interface IViewContainer {
  backgroundColor: string
  backgroundSrc: string
  backgroundBlur: number
  centered?: boolean
}

const ViewContainer: FC<IViewContainer> = ({
  backgroundColor, backgroundSrc, backgroundBlur, children, centered = true,
}) => (
  <div className='landing8-container'>
    <div
      className='landing8-background'
      style={{background: backgroundColor}}
    />
    {backgroundSrc &&
      <div
        className='landing8-background-image'
        style={{
          backgroundImage: backgroundSrc ? `url('${backgroundSrc}')` : undefined,
          filter: backgroundBlur ? `blur(${backgroundBlur}vmax)` : undefined,
          top: backgroundBlur ? `-${backgroundBlur}vmax` : 0,
          left: backgroundBlur ? `-${backgroundBlur}vmax` : 0,
          bottom: backgroundBlur ? `-${backgroundBlur}vmax` : 0,
          right: backgroundBlur ? `-${backgroundBlur}vmax` : 0,
        }}
      />
    }
    {centered
      ? <div className='landing8-centered-container'>{children}</div>
      : children
    }
  </div>
)

export {
  ViewContainer,
}
