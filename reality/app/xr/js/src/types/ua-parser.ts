interface IBrowser {
  name: string | undefined
  version: string | undefined
  major: string | undefined
}

interface IDevice {
  model: string | undefined
  type: 'console' | 'mobile' | 'tablet' | 'smarttv' | 'wearable' | 'embedded' | undefined
  vendor: string | undefined
}

interface IEngine {
  name: 'Amaya' | 'Blink' | 'EdgeHTML' | 'Flow' | 'Gecko' | 'Goanna' | 'iCab' | 'KHTML' | 'LibWeb' |
        'Links' | 'Lynx' | 'NetFront' | 'NetSurf' | 'Presto' | 'Tasman' | 'Trident' | 'w3m' |
        'WebKit' | undefined
  version: string | undefined
}

interface IOS {
  name: string | undefined
  version: string | undefined
}

interface ICPU {
  architecture: '68k' | 'amd64' | 'arm[64/hf]' | 'avr' | 'ia[32/64]' | 'irix[64]' | 'mips[64]' |
                'pa-risc' | 'ppc' | 'sparc[64]' | undefined
}

interface IData {
  ua: string
  browser: IBrowser
  device: IDevice
  engine: IEngine
  os: IOS
  cpu: ICPU

  withClientHints(): Promise<IData>
}

interface IUAParser {
  getBrowser(): IBrowser
  getOS(): IOS
  getEngine(): IEngine
  getDevice(): IDevice
  getCPU(): ICPU
  getUA(): string
  getResult(): IData
}

export {
  IData,
  IUAParser,
}
