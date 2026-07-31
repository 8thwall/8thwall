/* eslint-disable react/no-unknown-property */
/* eslint-disable max-len */
// NOTE(dat): Preact doesn't mess with svg property. The above rule is so eslint doesn't change this
// file's svg code.
import * as React from 'preact'
import type { FunctionComponent as FC } from 'preact'

interface IMovementAnimation {
  animationColor: string
}

const MovementAnimation: FC<IMovementAnimation> = ({ animationColor }) => (
  <svg
    className='sky-coaching-overlay-animation'
    xmlns='http://www.w3.org/2000/svg'
    viewBox='0 0 614 358'
    fill='none'
  >

    <path
      d='M10 343.893C239.063 312.272 369.511 311.8 604 343.893'
      stroke={animationColor}
      stroke-width='12'
      stroke-linecap='round'
      fill='none'
    />
    <path
      d='M478.466 88.7488C471.703 78.1807 461.979 73.1078 449.297 75.6443C437.037 78.603 430.274 87.0583 428.583 99.7405C426.469 100.163 424.356 100.586 422.664 101.855C417.591 104.813 415.055 111.154 416.323 116.651C417.591 122.146 422.242 126.797 427.737 127.643H430.696H504.253C511.016 127.643 516.513 122.992 518.203 118.343C521.162 109.888 515.667 101.01 506.789 99.7417C505.099 99.7417 504.675 98.8958 503.831 97.6276C499.18 89.596 490.302 85.3677 481.003 87.4818C480.58 88.3265 479.734 88.7488 478.466 88.7488V88.7488Z'
      fill={animationColor}
    />
    <path
      d='M173.439 127.876C186.967 106.738 205.991 97.0163 230.087 102.511C254.605 108.007 267.287 124.917 270.67 149.436C274.897 149.858 278.702 151.126 282.506 153.663C292.23 160.004 297.303 171.418 294.766 182.832C292.652 193.823 283.352 202.277 272.361 204.392C270.67 204.814 268.556 204.814 266.865 204.814H122.71C109.182 204.814 99.0358 195.936 95.6548 186.213C89.736 169.726 100.305 151.971 117.637 149.434C120.596 149.012 121.864 148.166 123.556 145.207C132.433 129.143 150.188 121.534 167.943 125.762C169.634 126.608 171.748 127.452 173.439 127.876Z'
      fill={animationColor}
    />
    <g className='sky-coaching-phone-animation'>
      <rect
        x="227"
        width="159"
        height="283"
        rx="23.6986"
        fill='#6F6C74'
        fill-opacity="0.75"
        shape-rendering="crispEdges" />
      <rect
        x="235.887"
        y="8.88699"
        width="141.226"
        height="265.226"
        rx="14.8116"
        stroke={animationColor}
        stroke-width="17.774"
        shape-rendering="crispEdges" />
    </g>
  </svg>
)

export {
  MovementAnimation,
}
