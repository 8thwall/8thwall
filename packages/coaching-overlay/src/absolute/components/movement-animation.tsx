import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

interface IMovementAnimation {
  animationColor: string
}

const MovementAnimation: FC<IMovementAnimation> = ({animationColor}) => (
  <svg
    className='coaching-overlay-animation'
    xmlns='http://www.w3.org/2000/svg'
    viewBox='0 0 256 107'
  >
    <defs>
      <linearGradient id='coaching-overlay-gradient' x1='0' x2='0' y1='0' y2='1'>
        <stop
          offset='0%'
          stop-color='black'  // eslint-disable-line react/no-unknown-property
        />
        <stop
          offset='35%'
          stop-color='white'  // eslint-disable-line react/no-unknown-property
        />
        <stop
          offset='65%'
          stop-color='white'  // eslint-disable-line react/no-unknown-property
        />
        <stop
          offset='100%'
          stop-color='black'  // eslint-disable-line react/no-unknown-property
        />
      </linearGradient>
      <mask id='coaching-overlay-gradient-mask'>
        <rect fill='url(#coaching-overlay-gradient)' x='0' y='0' width='256' height='40'>
          <animate
            attributeName='y'
            values='67;0;67'
            calcMode='spline'
            keySplines='0.5 0 0.5 1; 0.5 0 0.5 1'
            dur='2s'
            repeatCount='indefinite'
          />
        </rect>
      </mask>
    </defs>

    <path
      stroke={animationColor}
      fill='none'
      stroke-width='3'  // eslint-disable-line react/no-unknown-property
      stroke-linejoin='round'  // eslint-disable-line react/no-unknown-property
      d='M 90 23 L 166 23 L 238 84 L 18 84 Z'
      mask='url(#coaching-overlay-gradient-mask)'
    />

    <g transform='translate(128, 53)'>
      <animateTransform
        attributeName='transform'
        type='scale'
        additive='sum'
        values='1 1;0.65 0.7;1 1'
        calcMode='spline'
        keySplines='0.5 0 0.5 1; 0.5 0 0.5 1'
        begin='0s'
        dur='2s'
        repeatCount='indefinite'
      />
      <animateTransform
        attributeName='transform'
        type='translate'
        additive='sum'
        values='0 0; 0 -43; 0 0'
        calcMode='spline'
        keySplines='0.5 0 0.5 1; 0.5 0 0.5 1'
        begin='0s'
        dur='2s'
        repeatCount='indefinite'
      />
      <g transform='scale(0.22 0.2) translate(-92 -100)'>
        <path
          fill='#556b'
          d={('M132.08,11.46H51.39c-10.42,0-19.52,7.21-20.28,16.3L17.52,191.26c-1.17,14,8.77,' +
              '25.67,22.22,25.67H143.59c13.45,0,23.38-11.67,22.24-25.67L152.34,27.76C151.59,' +
              '18.67,142.53,11.46,132.08,11.46Z')}
        />
        <path
          fill={animationColor}
          d={('M132,0H52.55c-18,0-33.87,12.34-35.49,28.12L.26,192c-2.55,24.81,14.84,46,38.88,' +
              '46H145.39c24,0,41.43-21.22,38.88-46L167.46,28.12C165.84,12.34,150,0,132,0Zm33.88,' +
              '191.33c1.15,14-8.8,25.69-22.25,25.69H39.7c-13.44,0-23.38-11.67-22.21-25.68L31.14' +
              ',27.7C31.9,18.6,41,11.39,51.43,11.39h80.71c10.45,0,19.52,7.21,20.27,16.31Z')}
        />
      </g>
    </g>

  </svg>
)

export {
  MovementAnimation,
}
