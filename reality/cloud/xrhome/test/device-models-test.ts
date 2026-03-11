import {expect} from 'chai'

import {
  detectIOSDeviceTitle,
  getCorrectedAndroidModel,
  isMac,
} from '../src/client/editor/device-models'

describe('Detecting iOS device title for', () => {
  it('iPhone 6.7"', () => {
    const h = 2778
    const w = 1284
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 6.7"')
  })

  it('iPhone 6.5"', () => {
    const h = 2688
    const w = 1242
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 6.5"')
  })

  it('iPhone 6.1"', () => {
    let h = 1792
    let w = 828
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 6.1"')

    h = 2532
    w = 1170
    expect(title).to.equal('iPhone 6.1"')
  })

  it('iPhone 5.8"', () => {
    const h = 2436
    const w = 1125
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 5.8"')
  })

  it('iPhone 5.5"', () => {
    let h = 1920
    let w = 1080
    let title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 5.5"')

    h = 2208
    w = 1242
    title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 5.5"')
  })

  it('iPhone 4.7"', () => {
    const h = 1334
    const w = 750
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 4.7"')
  })

  it('iPhone 4.0"', () => {
    const h = 1136
    const w = 640
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 4.0"')
  })

  it('iPhone 3.5"', () => {
    let h = 960
    let w = 640
    let title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 3.5"')

    h = 480
    w = 320
    title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPhone 3.5"')
  })

  it('iPad 12.9"', () => {
    const h = 2732
    const w = 2048
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPad 12.9"')
  })

  it('iPad 11.0"', () => {
    const h = 2388
    const w = 1668
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPad 11.0"')
  })

  it('iPad 10.5"', () => {
    const h = 2224
    const w = 1668
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPad 10.5"')
  })

  it('iPad Air 10.2"', () => {
    const h = 2160
    const w = 1620
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPad Air 10.2"')
  })

  it('iPad 9.7"', () => {
    let h = 2048
    let w = 1536
    let title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPad 9.7"')

    h = 1024
    w = 768
    title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iPad 9.7"')
  })

  it('iOS devices of unknown or undetectable size', () => {
    const h = -1
    const w = -1
    const title = detectIOSDeviceTitle(h, w)
    expect(title).to.equal('iOS Device')
  })
})

describe('Correcting Android raw model name for', () => {
  it('Galaxy S9', () => {
    let m = 'SM-G960F'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S9')

    m = 'SM-G960U'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S9')

    m = 'SM-G960U1'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S9')
  })

  it('Galaxy S8', () => {
    let m = 'SM-G950F'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S8')

    m = 'SM-G950U'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S8')

    m = 'SM-G960U1'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S9')

    m = 'SM-G9600'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S8')
  })

  it('Galaxy S10', () => {
    let m = 'SM-G973F'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S10')

    m = 'SM-G973U'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S10')
  })

  it('Galaxy J7 Prime', () => {
    const m = 'SM-G610F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J7 Prime')
  })

  it('Galaxy S10+', () => {
    let m = 'SM-G975F'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S10+')

    m = 'SM-G975U'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S10+')
  })

  it('Galaxy S7', () => {
    const m = 'SM-G930F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S7')
  })

  it('Galaxy J7 Neo', () => {
    const m = 'SM-J701F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J7 Neo')
  })

  it('Galaxy S9+', () => {
    let m = 'SM-G965U'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S9+')

    m = 'SM-G965F'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S9+')

    m = 'SM-G965U1'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S9+')
  })

  it('Galaxy Note9', () => {
    let m = 'SM-N960U'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy Note9')

    m = 'SM-N960F'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy Note9')
  })

  it('Galaxy Note8', () => {
    let m = 'SM-N950U'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy Note8')

    m = 'SM-N950F'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy Note8')
  })

  it('A3s', () => {
    const m = 'CPH1803'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('A3s')
  })

  it('Galaxy S7 edge', () => {
    const m = 'SM-G935F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S7 edge')
  })

  it('Galaxy S8+', () => {
    let m = 'SM-G955F'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S8+')

    m = 'SM-G955U'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S8+')
  })

  it('Galaxy J6', () => {
    const m = 'SM-J600G'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J6')
  })

  it('Galaxy Note10+', () => {
    const m = 'SM-N975U'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy Note10+')
  })

  it('Galaxy A50', () => {
    let m = 'SM-A505FN'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A50')

    m = 'SM-A505F'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A50')
  })

  it('Galaxy J2', () => {
    const m = 'SM-J200G'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J2')
  })

  it('Galaxy A10', () => {
    const m = 'SM-A105F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A10')
  })

  it('Galaxy S10e', () => {
    let m = 'SM-G970U'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S10e')

    m = 'SM-G970F'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S10e')
  })

  it('Galaxy A5', () => {
    const m = 'SM-A520F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A5')
  })

  it('A5s', () => {
    const m = 'CPH1909'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('A5s')
  })

  it('Galaxy J7', () => {
    let m = 'SM-J700F'
    let title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J7')

    m = 'SM-J710F'
    title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J7')
  })

  it('Galaxy J2 Pro', () => {
    const m = 'SM-J250F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J2 Pro')
  })

  it('Galaxy J2 Prime', () => {
    const m = 'SM-G532G'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J2 Prime')
  })

  it('Galaxy A20', () => {
    const m = 'SM-A205F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A20')
  })

  it('One', () => {
    const m = 'one'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('One')
  })

  it('Galaxy Tab A', () => {
    const m = 'SM-T580'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy Tab A')
  })

  it('Y12', () => {
    const m = 'vivo 1904'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Y12')
  })

  it('Y91i', () => {
    const m = 'vivo 1820'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Y91i')
  })

  it('P20 lite', () => {
    const m = 'ANE-LX1'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('P20 lite')
  })

  it('Galaxy S10 5G', () => {
    const m = 'SM-G977N'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy S10 5G')
  })

  it('Y15', () => {
    const m = 'vivo 1901'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Y15')
  })

  it('Galaxy J4', () => {
    const m = 'SM-J400F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J4')
  })

  it('P30 Pro', () => {
    const m = 'VOG-L29'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('P30 Pro')
  })

  it('ZenFone Max Pro M1', () => {
    const m = 'ASUS_X00TD'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('ZenFone Max Pro M1')
  })

  it('Galaxy J5 Prime', () => {
    const m = 'SM-G570F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy J5 Prime')
  })

  it('Galaxy A8', () => {
    const m = 'SM-A530F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A8')
  })

  it('Y91', () => {
    const m = 'vivo 1811'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Y91')
  })

  it('Y53i', () => {
    const m = 'vivo 1606'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Y53i')
  })

  it('S1', () => {
    const m = 'vivo 1907'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('S1')
  })

  it('Galaxy A70', () => {
    const m = 'SM-A705FN'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A70')
  })

  it('Galaxy A71', () => {
    const m = 'CPH1801'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A71')
  })

  it('Galaxy Note10+ 5G', () => {
    const m = 'SM-N976N'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy Note10+ 5G')
  })

  it('Y81i', () => {
    const m = 'vivo 1807'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Y81i')
  })

  it('F9', () => {
    const m = 'CPH1823'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('F9')
  })

  it('Galaxy A7', () => {
    const m = 'CPH1901'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A7')
  })

  it('Galaxy A10s', () => {
    const m = 'SM-A107F'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('Galaxy A10s')
  })

  it('Unknown or undetectable Android model', () => {
    const m = 'ANDROID_PHONE'
    const title = getCorrectedAndroidModel(m)
    expect(title).to.equal('ANDROID_PHONE')
  })
})

describe('Verifying whether a device is a Mac.', () => {
  it('MacOS user agent verification success', () => {
    const ua = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_3) AppleWebKit/537.36 (KHTML,' +
      ' like Gecko) Chrome/85.0.4183.121 Safari/537.36'
    // eslint-disable-next-line no-unused-expressions
    expect(isMac(ua, 'bogus platform')).to.be.true
  })

  it('iOS user agent verification success', () => {
    const ua = 'Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_2 like Mac OS X) ' +
      'AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1'
    // eslint-disable-next-line no-unused-expressions
    expect(isMac(ua, 'bogus platform')).to.be.true
  })

  it('MacOS platform verification success', () => {
    // eslint-disable-next-line no-unused-expressions
    expect(isMac('', 'MacIntel')).to.be.true
  })

  it('MacOS verification fail', () => {
    const ua = 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) ' +
      'AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36'
    // eslint-disable-next-line no-unused-expressions
    expect(isMac(ua, 'bogus platform')).to.be.false
  })
})
