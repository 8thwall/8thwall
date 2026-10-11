import {h} from 'preact'

const AlertIcon = () => (
  <svg width='32' height='32' viewBox='0 0 32 32' fill='none' aria-hidden='true'>
    <path
      d='M16 4 28 26H4L16 4Z'
      stroke='currentColor'
      stroke-width='2'
      stroke-linejoin='round'
    />
    <path
      d='M16 12v5'
      stroke='currentColor'
      stroke-width='2'
      stroke-linecap='round'
    />
    <circle cx='16' cy='21.5' r='1.5' fill='currentColor' />
  </svg>
)

const OpenBrowserIcon = () => (
  <svg width='32' height='32' viewBox='0 0 32 32' fill='none' aria-hidden='true'>
    <rect
      x='4'
      y='6'
      width='24'
      height='20'
      rx='3'
      stroke='currentColor'
      stroke-width='2'
    />
    <path
      d='M4 11h24'
      stroke='currentColor'
      stroke-width='2'
    />
    <path
      d='m 13 22 l 7 -7 m -4.5 0 H 20 v 4.5'
      stroke='currentColor'
      stroke-width='2'
      stroke-linecap='round'
      stroke-linejoin='round'
    />
  </svg>
)

export {AlertIcon, OpenBrowserIcon}
