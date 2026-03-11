import UAParser from 'ua-parser-js'

// Pixel height and pixel width are being used to calculate screen size.
const detectIOSDeviceTitle = (height: number, width: number) => {
  if (height === 2778 && width === 1284) {
    return 'iPhone 6.7"'
  } else if (height === 2688 && width === 1242) {
    return 'iPhone 6.5"'
  } else if ((height === 1792 && width === 828) || (height === 2532 && width === 1170)) {
    return 'iPhone 6.1"'
  } else if (height === 2436 && width === 1125) {
    return 'iPhone 5.8"'
  } else if ((height === 2208 && width === 1242) || (height === 1920 && width === 1080)) {
    return 'iPhone 5.5"'
  } else if (height === 1334 && width === 750) {
    return 'iPhone 4.7"'
  } else if (height === 1136 && width === 640) {
    return 'iPhone 4.0"'
  } else if ((height === 960 && width === 640) || (height === 480 && width === 320)) {
    return 'iPhone 3.5"'
  } else if (height === 2732 && width === 2048) {
    return 'iPad 12.9"'
  } else if (height === 2388 && width === 1668) {
    return 'iPad 11.0"'
  } else if (height === 2224 && width === 1668) {
    return 'iPad 10.5"'
  } else if (height === 2160 && width === 1620) {
    return 'iPad Air 10.2"'
  } else if ((height === 2048 && width === 1536) || (height === 1024 && width === 768)) {
    return 'iPad 9.7"'
  } else {
    return 'iOS Device'
  }
}

const getCorrectedAndroidModel = (model: string) => {
  if (model === 'SM-G960F' || model === 'SM-G960U' || model === 'SM-G960U1') {
    return 'Galaxy S9'
  } else if (model === 'SM-G950F' ||
             model === 'SM-G950U' ||
             model === 'SM-G950U1' ||
             model === 'SM-G9600') {
    return 'Galaxy S8'
  } else if (model === 'SM-G973F' || model === 'SM-G973U') {
    return 'Galaxy S10'
  } else if (model === 'SM-G610F') {
    return 'Galaxy J7 Prime'
  } else if (model === 'SM-G975F' || model === 'SM-G975U') {
    return 'Galaxy S10+'
  } else if (model === 'SM-G930F') {
    return 'Galaxy S7'
  } else if (model === 'SM-J701F') {
    return 'Galaxy J7 Neo'
  } else if (model === 'SM-G965U' || model === 'SM-G965F' || model === 'SM-G965U1') {
    return 'Galaxy S9+'
  } else if (model === 'SM-N960U' || model === 'SM-N960F') {
    return 'Galaxy Note9'
  } else if (model === 'SM-N950U' || model === 'SM-N950F') {
    return 'Galaxy Note8'
  } else if (model === 'CPH1803') {
    return 'A3s'
  } else if (model === 'SM-G935F') {
    return 'Galaxy S7 edge'
  } else if (model === 'SM-G955F' || model === 'SM-G955U') {
    return 'Galaxy S8+'
  } else if (model === 'SM-J600G') {
    return 'Galaxy J6'
  } else if (model === 'SM-N975U') {
    return 'Galaxy Note10+'
  } else if (model === 'SM-A505FN' || model === 'SM-A505F') {
    return 'Galaxy A50'
  } else if (model === 'SM-J200G') {
    return 'Galaxy J2'
  } else if (model === 'SM-A105F') {
    return 'Galaxy A10'
  } else if (model === 'SM-G970U' || model === 'SM-G970F') {
    return 'Galaxy S10e'
  } else if (model === 'SM-A520F') {
    return 'Galaxy A5'
  } else if (model === 'CPH1909') {
    return 'A5s'
  } else if (model === 'SM-J700F' || model === 'SM-J710F') {
    return 'Galaxy J7'
  } else if (model === 'SM-J250F') {
    return 'Galaxy J2 Pro'
  } else if (model === 'SM-G532G') {
    return 'Galaxy J2 Prime'
  } else if (model === 'SM-A205F') {
    return 'Galaxy A20'
  } else if (model === 'one') {
    return 'One'
  } else if (model === 'SM-T580') {
    return 'Galaxy Tab A'
  } else if (model === 'vivo 1904') {
    return 'Y12'
  } else if (model === 'vivo 1820') {
    return 'Y91i'
  } else if (model === 'ANE-LX1') {
    return 'P20 lite'
  } else if (model === 'SM-G977N') {
    return 'Galaxy S10 5G'
  } else if (model === 'vivo 1901') {
    return 'Y15'
  } else if (model === 'SM-J400F') {
    return 'Galaxy J4'
  } else if (model === 'VOG-L29') {
    return 'P30 Pro'
  } else if (model === 'ASUS_X00TD') {
    return 'ZenFone Max Pro M1'
  } else if (model === 'SM-G570F') {
    return 'Galaxy J5 Prime'
  } else if (model === 'SM-A530F') {
    return 'Galaxy A8'
  } else if (model === 'vivo 1811') {
    return 'Y91'
  } else if (model === 'vivo 1606') {
    return 'Y53i'
  } else if (model === 'vivo 1907') {
    return 'S1'
  } else if (model === 'SM-A705FN') {
    return 'Galaxy A70'
  } else if (model === 'CPH1801') {
    return 'Galaxy A71'
  } else if (model === 'SM-N976N') {
    return 'Galaxy Note10+ 5G'
  } else if (model === 'vivo 1807') {
    return 'Y81i'
  } else if (model === 'CPH1823') {
    return 'F9'
  } else if (model === 'CPH1901') {
    return 'Galaxy A7'
  } else if (model === 'SM-A107F') {
    return 'Galaxy A10s'
  } else {
    return model
  }
}

// eslint-disable-next-line local-rules/hardcoded-copy
const SIMULATOR_STRING = 'Simulator'

const getDeviceTitle = (ua: string, screenHeight: number, screenWidth: number) => {
  if (!ua) {
    return null
  }

  // NOTE(christoph): This matches the string sent by dev8.
  if (ua === SIMULATOR_STRING) {
    return SIMULATOR_STRING
  }

  if (ua.includes('Pacific')) {
    return 'Oculus Go'
  }

  if (ua.includes('Quest')) {
    return 'Oculus Quest'
  }

  if (ua.includes('Tesla')) {
    return 'Tesla'
  }

  const p = new UAParser(ua)
  const rawOs = p.getOS()
  const rawBrowser = p.getBrowser()
  const rawDevice = p.getDevice()

  if (rawDevice?.vendor && rawDevice?.model) {
    if (rawOs.name === 'iOS') {
      if (!!screenHeight && !!screenWidth) {
        return detectIOSDeviceTitle(screenHeight, screenWidth)
      }
      return `${rawDevice.vendor} ${rawDevice.model}`
    } else if (rawOs.name === 'Android') {
      return `${rawDevice.vendor} ${getCorrectedAndroidModel(rawDevice.model)}`
    }
  }

  if (rawOs && rawBrowser && rawOs.name && rawBrowser.name) {
    return `${rawOs.name} ${rawBrowser.name}`
  }

  return null
}

const isMac = (ua: string, platform: string) => {
  if (platform.startsWith('Mac')) {
    return true
  }
  const parser = new UAParser(ua)
  const OS = parser.getOS().name
  return (OS === 'Mac OS' || OS === 'iOS')
}

export {
  detectIOSDeviceTitle,
  getCorrectedAndroidModel,
  getDeviceTitle,
  isMac,
}
