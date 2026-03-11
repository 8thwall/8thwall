import type {ConePixelPoints} from './unconify'

type UnconifyRequest = {
  imgData: ImageData
  pixelPoints: ConePixelPoints
  outputWidth: number
}

type UnconifyResponse = {
  data: ImageData
}

declare class UnconifyWorker {
  constructor()

  postMessage(message: UnconifyRequest): void

  terminate(): void

  onmessage: (message: UnconifyResponse) => void
}

export default UnconifyWorker
