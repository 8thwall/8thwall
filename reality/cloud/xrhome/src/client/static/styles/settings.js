const fontSize = '14px'

// Colors from go/brand
const brandPurple = '#7611b6'
const brandHighlight = '#ad50ff'  // A lighter purple
const brandPurpleDark = '#5c0d8f'
const brandBlack = '#2d2e43'
const brandAltBlack = '#2c2d41'
const brandWhite = '#ffffff'
const brandPop = '#c86dd7'
const brandLight = '#f4f0f8'
const popGradient = `linear-gradient(-75deg, ${brandPop}, ${brandPurple})`
const popGradientHover = `linear-gradient(135deg, ${brandPurple}, 80%, ${brandPop})`
const purpleish = '#fcf9ff'  // super light purple for background

// Gray shades Lightest to Darkest
const moonlight = '#f9f9fa'  // super light
const eggshell = '#f2f1f3'  // much light
const gray0 = eggshell
const gray1 = '#ebebf1'
const gray2 = '#d5d7e4'
const gray3 = '#b5b8d0'
const gray4 = '#8083a2'
const gray5 = '#464766'
const gray6 = '#3a3b55'

const dustyViolet = '#d9d0e3'  // lighter than gray3

const paleBlue = '#f6fdff'  // super light blue for background
const blogBlue = '#eaf0f7'
const lightBlue = '#57bfff'
const linkBlue = '#4183c4'
const darkBlue = '#1c1d2a'
const almostBlack = '#101118'

const orangish = '#fffcf4'  // super light orange for background
const orangishHighlight = '#f5a623'  // light orange for border
const mango = '#ffc828'
const darkMango = '#deaf23'
const cherry = '#dd0065'
const mint = '#00edaf'
const green = '#4cd964'
const orange = '#ff9500'
const darkOrange = '#9f5c00'
const deletedRed = '#ff4713'
const changedYellow = '#ffc758'
const darkRed = '#3f0008'
const darkMint = '#009770'
const skyBlue = '#b2ddfa'
const blueberry = '#57bfff'
const darkBlueberry = '#408CBA'
const persimmon = '#FF9068'

const mainBackgroundColor = '#fafafb'
const leftNavBackground = '#2d2e43'

const lightText = '#8184a3'
const darkBackground = '#20292e'
const darkBackground2 = '#2f3940'
const darkBackground3 = '#262e34'
const darkBackground4 = '#29292b'
const darkBackground5 = '#323232'
const darkBackground6 = '#171717'
const lightBackground1 = '#f0f1f1'
const lightBackground2 = '#d7d7d7'
const lightBackground3 = '#d3d5e9'
const lightBackground4 = '#f0f1f6'

const csBlack = '#0F0E1A'
const csGridLight = '#9BA0BF'

const semanticPlaceholderGray = '#C7C7C7'

// Largest width that is rendered with a mobile view.
// A mobile-only rule would use max-width: 1024px
// A desktop-only rule would use min-width: 1025px
// These are based on semantic's width breakpoints (except "mobileWidthBreakpoint")
// TODO(wayne): rename and redefine these breakpoints to be consistent with website8
const tinyWidthBreakpoint = '767px'
const mobileWidthBreakpoint = '1024px'
const smallMonitorWidthBreakpoint = '1200px'

// Use these directly in your jss
const tinyViewOverride = `@media (max-width: ${tinyWidthBreakpoint})`
const mobileViewOverride = `@media (max-width: ${mobileWidthBreakpoint})`
const smallMonitorViewOverride = `@media (max-width: ${smallMonitorWidthBreakpoint})`

const buttonBoxShadow = '-0.5em 0.5em 1em 0 #0004'
const videoBoxShadow = '0 0 1em 0.25em #0004'
const postBoxShadow = '0 0 0.5em 0.25em #46476673'

// cherry with 2% opacity
const twoPercentCherry = '#f8f4f7'
const cherryLight = '#ff0060'

// highlight for dark mode
const accessibleHighlight = '#bd71ff'

const editorMonospace = 'Menloish, monospace'
const editorFontSize = '12px'
const headerSanSerif = 'Noto sans JP, sans-serif'
const bodySanSerif = 'Nunito, sans-serif'

// project card stuffs
const cardImageRatio = '52.5%'
const cardShadowBlur = '12px'
const cardShadowSpread = '1px'
const cardShadowColor = 'rgba(70,71,102,0.15)'
const cardBorderRadius = '0.5em'

// Editor colors
const editorBlue = '#59c8fa'

// Public browse stuffs
const statusRounding = '0.25em'
const statusShadow = `0 0 1px 1px ${brandPurple}`

const centeredSectionMaxWidth = '70em'
const centeredSectionMargin = '4rem'

const twentyPercentBlack = `${brandBlack}32`
const fiftyPercentBlack = `${brandBlack}80`

const tenPercentDarkMint = `${darkMint}1a`
const tenPercentDarkMango = `${darkMango}1a`
const tenPercentCherry = `${cherry}1a`

// category colors
const outdoorsGreen = '#438A4A'
const objectsBrown = '#9B6E00'
const buildingsOrange = '#F4A261'
const transportationBlue = '#548A9F'
const sportsRecOrange = '#E76F51'
const artPurple = '#7611B7'
const otherBlue = '#3453a2'
const createLocationPurple = '#9747FF'

// colors used in ScaniverseGsb
const scaniverseLightestGray = '#f2f2f9'  // used in other non-scaniverse places as well
const scaniverseLighterGray = '#B5B8D0'
const scaniverseButtonBackgroundGray = '#f0f1f6'
const scaniverseDividerGray = '#545458A6'

// NAE Colors
const naeGradient = `linear-gradient(135deg, ${brandPurpleDark}, ${cherry}, ${mango})`

// NOTE(dat): More styles can be copied from _settings.scss
module.exports = {
  fontSize,
  brandPurple,
  brandHighlight,
  brandPurpleDark,
  brandBlack,
  brandAltBlack,
  brandWhite,
  brandPop,
  brandLight,
  popGradient,
  popGradientHover,
  naeGradient,
  purpleish,
  moonlight,
  eggshell,
  gray0,
  gray1,
  gray2,
  gray3,
  gray4,
  gray5,
  gray6,
  scaniverseButtonBackgroundGray,
  scaniverseLighterGray,
  scaniverseLightestGray,
  scaniverseDividerGray,
  dustyViolet,
  paleBlue,
  blogBlue,
  lightBlue,
  linkBlue,
  darkBlue,
  almostBlack,
  orangish,
  orangishHighlight,
  mango,
  darkMango,
  cherry,
  mint,
  changedYellow,
  deletedRed,
  green,
  orange,
  darkOrange,
  darkRed,
  darkMint,
  skyBlue,
  blueberry,
  darkBlueberry,
  mainBackgroundColor,
  leftNavBackground,
  lightText,
  darkBackground,
  darkBackground2,
  darkBackground3,
  darkBackground4,
  darkBackground5,
  darkBackground6,
  csBlack,
  csGridLight,
  lightBackground1,
  lightBackground2,
  lightBackground3,
  lightBackground4,
  tinyWidthBreakpoint,
  mobileWidthBreakpoint,
  smallMonitorWidthBreakpoint,
  tinyViewOverride,
  mobileViewOverride,
  smallMonitorViewOverride,
  buttonBoxShadow,
  videoBoxShadow,
  postBoxShadow,
  twoPercentCherry,
  cherryLight,
  accessibleHighlight,
  editorMonospace,
  editorFontSize,
  headerSanSerif,
  bodySanSerif,
  cardImageRatio,
  cardShadowBlur,
  cardShadowSpread,
  cardShadowColor,
  cardBorderRadius,
  editorBlue,
  statusRounding,
  statusShadow,
  semanticPlaceholderGray,
  centeredSectionMaxWidth,
  centeredSectionMargin,
  twentyPercentBlack,
  fiftyPercentBlack,
  tenPercentDarkMint,
  tenPercentDarkMango,
  tenPercentCherry,
  outdoorsGreen,
  objectsBrown,
  buildingsOrange,
  transportationBlue,
  sportsRecOrange,
  artPurple,
  otherBlue,
  createLocationPurple,
  persimmon,
}
