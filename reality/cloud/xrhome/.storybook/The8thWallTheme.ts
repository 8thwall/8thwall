import {create} from '@storybook/theming'

import logoImg from '../src/client/static/8th-Wall-Horizontal-Logo-Purple.svg'
import {
  brandBlack, brandPop, brandWhite, gray4, gray5, leftNavBackground, mainBackgroundColor,
} from '../src/client/static/styles/settings'

export default create({
  base: 'light',

  colorPrimary: brandWhite,
  colorSecondary: gray4,

  // UI
  appBg: brandWhite,
  appContentBg: mainBackgroundColor,
  appBorderColor: 'grey',
  appBorderRadius: 4,

  // Typography
  fontBase: 'Nunito, Roboto, san-serif',
  fontCode: 'monospace',

  // Text colors
  textColor: brandBlack,
  textInverseColor: gray4,

  // Toolbar default and active colors
  barTextColor: brandWhite,
  barSelectedColor: brandPop,
  barBg: leftNavBackground,

  // Form colors
  inputBg: brandWhite,
  inputBorder: gray5,
  inputTextColor: brandBlack,
  inputBorderRadius: 4,

  // Branding
  brandTitle: '8th Wall',
  brandUrl: 'https://8thwall.com',
  brandImage: logoImg,
  brandTarget: '_self',
})
