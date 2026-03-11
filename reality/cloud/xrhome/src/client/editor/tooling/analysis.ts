import type {ParsedComponents} from '@ecs/shared/studio-component'
import uuidv4 from 'uuid/v4'

let analysisWorker_: Worker = null

const requests: Record<string, (d: unknown) => void> = {}

// Worker is initialized on first use.
const getWorker = () => {
  if (!analysisWorker_) {
    analysisWorker_ = Build8.PLATFORM_TARGET === 'desktop'
      ? new Worker('desktop://dist/worker/client/analysis-worker.js')
      : new Worker(`/${Build8.DEPLOYMENT_PATH}/client/analysis-worker.js`)
    analysisWorker_.onmessage = (msg) => {
      requests[msg.data.requestId]?.(msg.data)
      delete requests[msg.data.requestId]
    }
  }
  return analysisWorker_
}

const getStudioComponents = (val: string) => (
  // NOTE(christoph): We might have issues serializing Error when we return from the worker.
  new Promise<ParsedComponents>((resolve) => {
    const requestId = uuidv4()
    requests[requestId] = resolve
    getWorker().postMessage({
      command: 'parse-components',
      val,
      requestId,
    })
  })
)

export {
  getStudioComponents,
}
