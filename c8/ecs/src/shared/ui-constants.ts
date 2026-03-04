import {DEFAULT_FONT_NAME} from './fonts'

// Note(cindyhu): all optional properties are default to empty string to avoid unwanted overrides,
// and will resolve to a fallback value in runtime.
const UI_DEFAULT_ALIGN_CONTENT = 'flex-start'
const UI_DEFAULT_ALIGN_ITEMS = 'flex-start'
const UI_DEFAULT_ALIGN_SELF = ''
const UI_DEFAULT_BACKGROUND = '#ffffff'
const UI_DEFAULT_BACKGROUND_OPACITY = -1
const UI_DEFAULT_BACKGROUND_SIZE = 'contain'
const UI_DEFAULT_BORDER_COLOR = '#000000'
const UI_DEFAULT_BORDER_RADIUS = 0
const UI_DEFAULT_BORDER_RADIUS_CORNER = ''
const UI_DEFAULT_BORDER_WIDTH = 0
const UI_DEFAULT_BOTTOM = ''
const UI_DEFAULT_COLOR = '#ffffff'
const UI_DEFAULT_DIRECTION = 'ltr'
const UI_DEFAULT_DISPLAY = ''
const UI_DEFAULT_FIXED_SIZE = false
const UI_DEFAULT_FLEX = 0
const UI_DEFAULT_FLEX_BASIS = ''
const UI_DEFAULT_FLEX_DIRECTION = 'row'
const UI_DEFAULT_FLEX_GROW = 0
const UI_DEFAULT_FLEX_SHRINK = 0
const UI_DEFAULT_FLEX_WRAP = 'nowrap'
const UI_DEFAULT_FONT_SIZE = 16
const UI_DEFAULT_GAP = ''
const UI_DEFAULT_HEIGHT = '100'
const UI_DEFAULT_IGNORE_RAYCAST = false
const UI_DEFAULT_IMAGE = ''
const UI_DEFAULT_JUSTIFY_CONTENT = 'flex-start'
const UI_DEFAULT_LEFT = ''
const UI_DEFAULT_MARGIN = ''
const UI_DEFAULT_MAX_HEIGHT = ''
const UI_DEFAULT_MAX_WIDTH = ''
const UI_DEFAULT_MIN_HEIGHT = ''
const UI_DEFAULT_MIN_WIDTH = ''
const UI_DEFAULT_OPACITY = 1
const UI_DEFAULT_OVERFLOW = ''
const UI_DEFAULT_PADDING = ''
const UI_DEFAULT_POSITION = 'static'
const UI_DEFAULT_RIGHT = ''
const UI_DEFAULT_TEXT = ''
const UI_DEFAULT_TEXT_ALIGN = 'center'  // NOTE(jeffha): "three-mesh-ui" defaults it to center
const UI_DEFAULT_VERTICAL_TEXT_ALIGN = 'start'
const UI_DEFAULT_TOP = ''
const UI_DEFAULT_TYPE = 'overlay'
const UI_DEFAULT_WIDTH = '100'
const UI_DEFAULT_NINE_SLICE = '0'
const UI_DEFAULT_NINE_SLICE_SCALE_FACTOR = 1
const UI_DEFAULT_VIDEO = ''
const UI_DEFAULT_STACKING_ORDER = 0

const getBackgroundOpacity = (value: number | undefined, hasImage: boolean): number => {
  if (value !== undefined && value !== -1) {
    return value
  }
  return hasImage ? 1 : 0
}

const UI_DEFAULTS = {
  alignContent: UI_DEFAULT_ALIGN_CONTENT,
  alignItems: UI_DEFAULT_ALIGN_ITEMS,
  alignSelf: UI_DEFAULT_ALIGN_SELF,
  background: UI_DEFAULT_BACKGROUND,
  backgroundOpacity: UI_DEFAULT_BACKGROUND_OPACITY,
  backgroundSize: UI_DEFAULT_BACKGROUND_SIZE,
  borderColor: UI_DEFAULT_BORDER_COLOR,
  borderRadius: UI_DEFAULT_BORDER_RADIUS,
  borderRadiusTopLeft: UI_DEFAULT_BORDER_RADIUS_CORNER,
  borderRadiusTopRight: UI_DEFAULT_BORDER_RADIUS_CORNER,
  borderRadiusBottomLeft: UI_DEFAULT_BORDER_RADIUS_CORNER,
  borderRadiusBottomRight: UI_DEFAULT_BORDER_RADIUS_CORNER,
  borderWidth: UI_DEFAULT_BORDER_WIDTH,
  bottom: UI_DEFAULT_BOTTOM,
  color: UI_DEFAULT_COLOR,
  columnGap: UI_DEFAULT_GAP,
  direction: UI_DEFAULT_DIRECTION,
  display: UI_DEFAULT_DISPLAY,
  fixedSize: UI_DEFAULT_FIXED_SIZE,
  flex: UI_DEFAULT_FLEX,
  flexBasis: UI_DEFAULT_FLEX_BASIS,
  flexDirection: UI_DEFAULT_FLEX_DIRECTION,
  flexGrow: UI_DEFAULT_FLEX_GROW,
  flexShrink: UI_DEFAULT_FLEX_SHRINK,
  flexWrap: UI_DEFAULT_FLEX_WRAP,
  font: {type: 'font', font: DEFAULT_FONT_NAME},
  fontSize: UI_DEFAULT_FONT_SIZE,
  gap: UI_DEFAULT_GAP,
  height: UI_DEFAULT_HEIGHT,
  ignoreRaycast: UI_DEFAULT_IGNORE_RAYCAST,
  image: UI_DEFAULT_IMAGE,
  justifyContent: UI_DEFAULT_JUSTIFY_CONTENT,
  left: UI_DEFAULT_LEFT,
  margin: UI_DEFAULT_MARGIN,
  marginBottom: UI_DEFAULT_MARGIN,
  marginLeft: UI_DEFAULT_MARGIN,
  marginRight: UI_DEFAULT_MARGIN,
  marginTop: UI_DEFAULT_MARGIN,
  maxHeight: UI_DEFAULT_MAX_HEIGHT,
  maxWidth: UI_DEFAULT_MAX_WIDTH,
  minHeight: UI_DEFAULT_MIN_HEIGHT,
  minWidth: UI_DEFAULT_MIN_WIDTH,
  opacity: UI_DEFAULT_OPACITY,
  overflow: UI_DEFAULT_OVERFLOW,
  padding: UI_DEFAULT_PADDING,
  paddingBottom: UI_DEFAULT_PADDING,
  paddingLeft: UI_DEFAULT_PADDING,
  paddingRight: UI_DEFAULT_PADDING,
  paddingTop: UI_DEFAULT_PADDING,
  position: UI_DEFAULT_POSITION,
  right: UI_DEFAULT_RIGHT,
  rowGap: UI_DEFAULT_GAP,
  text: UI_DEFAULT_TEXT,
  textAlign: UI_DEFAULT_TEXT_ALIGN,
  verticalTextAlign: UI_DEFAULT_VERTICAL_TEXT_ALIGN,
  top: UI_DEFAULT_TOP,
  type: UI_DEFAULT_TYPE,
  width: UI_DEFAULT_WIDTH,
  nineSliceBorderTop: UI_DEFAULT_NINE_SLICE,
  nineSliceBorderBottom: UI_DEFAULT_NINE_SLICE,
  nineSliceBorderLeft: UI_DEFAULT_NINE_SLICE,
  nineSliceBorderRight: UI_DEFAULT_NINE_SLICE,
  nineSliceScaleFactor: UI_DEFAULT_NINE_SLICE_SCALE_FACTOR,
  video: UI_DEFAULT_VIDEO,
  stackingOrder: UI_DEFAULT_STACKING_ORDER,
} as const

export {
  getBackgroundOpacity,
  UI_DEFAULTS,
}
