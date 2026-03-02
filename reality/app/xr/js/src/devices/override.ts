import type {RenderContext} from '../types/pipeline'
import {create8wCanvas} from '../canvas'
import {memo, singleton} from '../factory'
import {getRenderer} from './getRenderer'

const getRendererAsync = singleton(() => new Promise<string>((resolve) => {
  getRenderer((info) => {
    resolve(info)
  })
}))

// Note: Works up until iOS 12.2 - WEBGL_debug_renderer_info not provided after.
const getUnmaskedWebGlRenderer = memo((): string => {
  const canvas = create8wCanvas('js-device')
  const context = (
    canvas.getContext('webgl') || canvas.getContext('experimental-webgl')
  ) as RenderContext
  canvas.width = 0
  canvas.height = 0
  if (!context) {
    return ''
  }
  const info = context.getExtension('WEBGL_debug_renderer_info')
  if (!info) {
    return ''
  }
  return context.getParameter(info.UNMASKED_RENDERER_WEBGL)
})

/**
 * @param priorityList List of [gpuName, iPhone ID] to match against renderers.
 * @param fallback if nothing in priority list match the renderer, use this value.
 * */
const pickFromRenderer = (
  renderer: string, extraRendererInfo: string, priorityList: [string, string][], fallback?: string
): string => {
  // A list of potential GPU for this device
  // e.g. ['Apple A10 GPU', 'Apple A11 GPU', 'Apple A13 GPU']
  let renderers: string[] = extraRendererInfo?.split('|') || []
  if (renderer !== 'Apple GPU') {
    renderers.push(renderer)
  }
  renderers = renderers.map(r => r.trim())

  if (renderers.length > 0 && priorityList.length > 0) {
    const foundDevice = priorityList.find(([gpuName]) => {
      const fullGpuName = `Apple ${gpuName} GPU`
      return renderers.includes(fullGpuName)
    })
    if (foundDevice) {
      return `iPhone${foundDevice[1]}`
    }
  }

  return fallback || 'iPhone'
}

// Note: Display Zoom refers to an iPhone magnification feature. See:
// https://support.apple.com/en-in/guide/iphone/iphd6804774e/ios
const detectIOsModel = (extraRendererInfo?: string): string => {
  const height = window.screen.height * window.devicePixelRatio
  const renderer = getUnmaskedWebGlRenderer()

  // Data from calibration at go/calibration8
  // Height is slightly more distinguishable than width (less collisions)
  // Device strings `iPhone10,1` needs to be kept in-sync with device-infos.cc
  switch (height) {
    // Keep these case in order
    case 1136:
      return pickFromRenderer(renderer, extraRendererInfo, [
        // iPhone 5 or 5c, but let's assume 5c since that's our minimum that we have
        // calibrated.
        ['A6', '5,3'],
        // iPhone 5s
        ['A7', '6,1'],
        // iPhone 6 in display zoom mode
        ['A8', '7,2'],
        // iPhone SE
        // This may also be an iPhone 6s in display zoom mode.
        ['A9', '8,4'],
      ], 'iPhone 5/5s/5c/SE')
    case 1334:
      return pickFromRenderer(renderer, extraRendererInfo, [
        // iPhone 8
        // This could also be iPhone10,4, but we're just going to assume 10,1.
        ['A11', '10,1'],
        // iPhone 7
        // This could also be iPhone9,3, but we're just going to assume 9,1.
        ['A10', '9,1'],
        // iPhone 6s
        ['A9', '8,1'],
        // iPhone 6
        ['A8', '7,2'],
      ], 'iPhone 6/6s/7/8')
    case 1624:  // With display Zoom
    case 1792:
      return pickFromRenderer(renderer, extraRendererInfo, [
        // iPhone 11
        ['A13', '12,2'],
        // iPhone XR
        ['A12', '11,8'],
      ], 'iPhone XR/11')
    case 2001:  // Display Zoom
    case 2208:
      return pickFromRenderer(renderer, extraRendererInfo, [
        // iPhone 8 Plus
        ['A11', '10,2'],
        // iPhone 7 Plus
        ['A10', '9,2'],
        // iPhone 6s Plus
        ['A9', '8,2'],
        // iPhone 6 Plus
        ['A8', '7,1'],
      ], 'iPhone 6+/6s+/7+/8+')
    case 2436:
      return pickFromRenderer(renderer, extraRendererInfo, [
        // iPhone X
        // This could also be iPhone10,6
        ['A11', '10,3'],
        // iPhone XS
        // This could also be iPhone11,6 in display zoom mode.
        ['A12', '11,2'],
        // iPhone 13 Mini, few were sold
        ['A15', '14,4'],
        // iPhone 12 Mini (iPhone13,1)
        ['A14', '13,1'],
        // TODO(dat): Confirm iPhone 13 Pro Max Display Zoom does not fall into this
        // TODO(dat): Verify 11Pro values
      ], 'iPhone X/XS/11Pro')
    case 2079:  // When display zoom is on
    case 2532:
      // These values can be for 12/12Pro/13/13Pro
      // There is no way to distinguish without doing CPU bench
      return 'iPhone 13/13Pro'
    case 2556:
      // iPhone 14 Pro
      return 'iPhone15,3'
    case 2688:
      return pickFromRenderer(renderer, extraRendererInfo, [
        // iPhone XS Max
        ['A12', '11,4'],
        // iPhone 11 Pro Max
        // NOTE(dat): Not available in device-infos.cc
        ['A13', '12,5'],
      ], 'iPhone XSMax/11ProMax')
    case 2778:
      // iPhone 12 Pro Max
      return 'iPhone13,4'
      // TODO(dat): Confirm iPhone 13 Pro Max (IPHONE14,3) does not fall into this
      // This can also be iPhone 14 Plus. We are picking 12 Pro Max since it has more sale.
      // return 'iPhone15,2'
    case 2796:
      // iPhone 14 Pro Max (default) or iPhone 15 Pro Max
      return 'iPhone15,4'
    default:
      return 'iPhone'
  }
}

const detectIPadModel = () => {
  const renderer = getUnmaskedWebGlRenderer()

  if (window.screen.height / window.screen.width === 1024 / 768) {
    // iPad, iPad 2, iPad Mini
    if (window.devicePixelRatio === 1) {
      const modelMap = {
        // iPad
        'PowerVR SGX 535': 'iPad1,1',

        // iPad 2 (Could be iPad2,1, iPad2,2, iPad2,3, iPad2,4. Could also be Mini)
        'PowerVR SGX 543': 'iPad2,1',
      }
      return modelMap[renderer] || 'iPad/iPad2/iPad Mini'  // sync this string to device-infos.cc
    // iPad 3, 4, 5, 6, Mini 2, Mini 3, Mini 4, Air, Air 2
    } else {
      const modelMap = {
        // iPad 3 (Could be iPad3,1, iPad3,2, iPad3,3)
        'PowerVR SGX 543': 'iPad3,1',

        // iPad 4 (Could be iPad3,4, iPad3,5, iPad3,6)
        'PowerVR SGX 554': 'iPad3,4',

        // iPad Air (Could be iPad4,1, iPad4,2, iPad4,3. Could also be Mini 2 or Mini 3)
        'Apple A7 GPU': 'iPad4,1',

        // iPad Air 2 (Could be iPad5,3, iPad5,4)
        'Apple A8X GPU': 'iPad5,3',

        // iPad Mini 4 (Could be iPad5,1, iPad5,2)
        'Apple A8 GPU': 'iPad5,1',

        // iPad 5 (Could be iPad6,11 or iPad6,12. Could also be iPad Pro 9.7 -- iPad6,3, iPad6,4)
        'Apple A9 GPU': 'iPad6,11',

        // iPad 6
        'Apple A10 GPU': 'iPad7,5',
      }
      // sync this string to device-infos.cc
      return modelMap[renderer] || 'iPad3/4/5/6/Mini2/Mini3/Mini4/Air/Air2'
    }
  // iPad Pro 10.5
  } else if (window.screen.height / window.screen.width === 1112 / 834) {
    // Could be iPad7,3, iPad7,4
    return 'iPad7,4'
  // iPad Pro 12.9, Pro 12.9 (2nd Gen)
  } else if (window.screen.height / window.screen.width === 1366 / 1024) {
    const modelMap = {
      // iPad Pro 12.9 (2nd Gen) (Could be iPad7,1, iPad7,2)
      'Apple A10X GPU': 'iPad7,1',

      // iPad Pro 12.9 (Could be iPad6,7, iPad6,8)
      'Apple A9 GPU': 'iPad6,7',
    }

    return modelMap[renderer] || 'iPadPro12/iPadPro12G2'  // sync this string to device-infos.cc
  } else {
    return 'iPad'  // sync this string to device-infos.cc
  }
}

const correctDeviceManufacturer = (manufacturer) => {
  const manufacturerMap = {
    'LG': 'LGE',
    // TODO (alvin): Add more corrections as necessary.
  }
  return (manufacturer && manufacturerMap[manufacturer]) || manufacturer
}

const correctDeviceModel = (model, extraRendererInfo?: string) => {
  // iOS devices don't specify their model in the user-agent. Let's update that ourselves.
  if (model === 'iPhone') {
    return detectIOsModel(extraRendererInfo)
  } else if (model === 'iPad') {
    return detectIPadModel()
  }

  const modelMap = {
    'A002A': 'ASUS_A002A',
    'K53a48': 'Lenovo K53a48',
    'H910': 'LG-910',
    'One M8': 'HTC One_M8',
    'PB2-690Y': 'Lenovo PB2-690Y',
    'Z00_AD': 'ASUS_Z00AD',
    'mi 4X': 'Redmi 4x',
    'mi 5A': 'Redmi 5A',
    'oG3': 'MotoG3',
    'motorola razr plus 2023': 'RAZR+',
  }

  return (model && modelMap[model]) || model
}

export {
  correctDeviceManufacturer,
  correctDeviceModel,
  getRendererAsync,
  // export for test purposes
  pickFromRenderer,
}
