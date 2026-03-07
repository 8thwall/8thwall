// @ts-nocheck
import {START} from './block-layout/JustifyContent'
import {CENTER} from './block-layout/AlignItems'
import {COLUMN} from './block-layout/ContentDirection'
import {CENTER as textAlign} from './inline-layout/TextAlign'
import {PRE_LINE as whiteSpace} from './inline-layout/Whitespace'
import {STRETCH} from './background-size'

const {Color, CanvasTexture} = window.THREE

let defaultTexture

function getDefaultTexture() {
  if (!defaultTexture) {
    const ctx = document.createElement('canvas').getContext('2d')
    ctx.canvas.width = 1
    ctx.canvas.height = 1
    ctx.fillStyle = '#ffffff'
    ctx.fillRect(0, 0, 1, 1)
    defaultTexture = new CanvasTexture(ctx.canvas)
    defaultTexture.isDefault = true
  }

  return defaultTexture
}

/** List the default values of the lib components */
export default {
  container: null,
  fontFamily: null,
  fontSize: 0.05,
  activeLayers: [0],
  // FontKerning would act like css : "none"|"normal"|"auto"("auto" not yet implemented)
  fontKerning: 'normal',
  bestFit: 'none',
  offset: 0.01,
  interLine: 0.01,
  // added '\n' to also acts as friendly breaks when white-space:normal
  breakOn: '- ,.:?!\n',
  whiteSpace,
  alignContent: null,  // TODO (tri) remove this deprecated prop
  contentDirection: COLUMN,
  alignItems: CENTER,
  justifyContent: START,
  textAlign,
  textType: 'MSDF',
  fontColor: new Color(0xffffff),
  fontOpacity: 1,
  fontPXRange: 4,
  fontSupersampling: true,
  borderRadius: 0.01,
  borderRadiusTopLeft: '',
  borderRadiusTopRight: '',
  borderRadiusBottomLeft: '',
  borderRadiusBottomRight: '',
  borderWidth: 0,
  borderColor: new Color('black'),
  borderOpacity: 1,
  backgroundSize: STRETCH,
  backgroundColor: new Color(0x222222),
  backgroundWhiteColor: new Color(0xffffff),
  backgroundOpacity: 1.0,
  backgroundOpaqueOpacity: 1.0,
  backgroundDepthWrite: true,
  foregroundDepthWrite: true,
  // this default value is a function to avoid initialization issues (see issue #126)
  getDefaultTexture,
  hiddenOverflow: false,
  letterSpacing: 0,
  opacity: 1,
  nineSliceBorderTop: 0,
  nineSliceBorderBottom: 0,
  nineSliceBorderLeft: 0,
  nineSliceBorderRight: 0,
  nineSliceScaleFactor: 1,
  uiScale: 1,
  defines: {},
}
