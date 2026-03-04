import {internalConfig} from './internal-config'
import type {
  ConfigureResponse,
  LoadBytesRequestData,
  LoadBytesResponse,
  LoadFilesResponse,
  MergeBytesResponse,
  ModelManagerConfiguration,
  ModelManagerRequest,
  ModelManagerResponse,
  ModelSrc,
  UpdateViewResponse,
} from './model-manager-types'

enum CoordinateSystem {
  UNKNOWN = 0,
  RUF = 1,
  RUB = 2,
}

// Creates an interface that allows loading models and updating them based on view. Under the hood
// This spins up a woker to run code asynchronously, off of the main thread. While a given piece of
// calls that come in will be buffered until the previous call is finished. Only the most recent
// call will be processed, others will be ignored. After a call is processed, a pending call will be
// processed immediately, if one exists.
const create = () => {
  const {workerUrl} = internalConfig()

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  let loadedCallback_ = (response: LoadBytesResponse | LoadFilesResponse | MergeBytesResponse) => {}
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  let updatedCallback_ = (response: UpdateViewResponse) => {}
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  let configAppliedCallback_ = (response: ConfigureResponse) => {}
  const workerBlob = URL.createObjectURL(new Blob([`\
    importScripts(${JSON.stringify(workerUrl)})
  `], {type: 'application/javascript'}))
  let worker: Worker | null = new Worker(workerBlob)

  const workerPromiseResolve = {}
  const workerPromise = {}
  const pendingMessages = {}

  const postNextMessage = (msg: ModelManagerRequest, transferables?: Transferable[]) => {
    workerPromise[msg.msg] = new Promise<ModelManagerResponse>((resolve) => {
      workerPromiseResolve[msg.msg] = resolve
      if (transferables) {
        worker?.postMessage(msg, transferables)
      } else {
        worker?.postMessage(msg)
      }
    }).then(() => {
      delete workerPromise[msg.msg]
      delete workerPromiseResolve[msg.msg]
      if (pendingMessages[msg.msg]) {
        postNextMessage(pendingMessages[msg.msg])
        delete pendingMessages[msg.msg]
      }
    })
  }

  const postMessage = (msg: ModelManagerRequest, transferables?: Transferable[]) => {
    if (workerPromise[msg.msg]) {
      pendingMessages[msg.msg] = msg
      return
    }
    postNextMessage(msg, transferables)
  }

  const dispose = () => {
    postMessage({msg: 'dispose'})
    URL.revokeObjectURL(workerBlob)
  }

  worker.onmessage = (e) => {
    const response: ModelManagerResponse = e.data
    switch (response.msg) {
      case 'loadBytes':
        loadedCallback_(response)
        break
      case 'mergeBytes':
        loadedCallback_(response)
        break
      case 'loadFiles':
        loadedCallback_(response)
        break
      case 'updateView':
        if (response.updated) {
          updatedCallback_(response)
        }
        break
      case 'workerLoaded':
        // Worker loaded
        break
      case 'configure':
        configAppliedCallback_(response)
        break
      case 'dispose':
        worker?.terminate()
        worker = null
        break
      default:
        console.error('Unknown message', response)  // eslint-disable-line no-console
    }
    if (workerPromiseResolve[response.msg]) {
      workerPromiseResolve[response.msg]()
    }
  }

  postMessage({msg: 'workerLoaded'})

  const filePromise = (file): Promise<LoadBytesRequestData> => new Promise((resolve) => {
    const fr = new FileReader()
    fr.onload = () => {
      const bytes = fr ? new Uint8Array(fr.result as ArrayBufferLike) : new Uint8Array()
      resolve({filename: file.name, bytes})
    }
    fr.readAsArrayBuffer(file)
  })

  const modelSrcPromise = (src: ModelSrc): Promise<LoadBytesRequestData> => {
    const filename = src.filename || src.url.split('/').pop() || ''
    return fetch(src.url, {mode: 'cors'})
      .then(response => Promise.all([response.arrayBuffer(), response.status]))
      .then(([buffer, status]) => {
        if (status !== 200) {
          throw new Error(`Failed to load ${src.url}: ${status}`)  // eslint-disable-line no-console
        }
        const bytes = new Uint8Array(buffer)
        return {filename, bytes}
      })
  }

  const loadFiles = (fileList: FileList) => {
    const files: File[] = []
    for (let i = 0; i < fileList.length; i++) {
      files.push(fileList[i])
    }

    return Promise.all(files.map(filePromise))
      .then((fileData: LoadBytesRequestData[]) => {
        postMessage({msg: 'loadFiles', data: fileData}, fileData.map(f => f.bytes.buffer))
      })
  }

  const loadBytes = (filename: string, bytes: Uint8Array) => {
    postMessage({msg: 'loadBytes', data: {filename, bytes}}, [bytes.buffer])
  }

  const loadModel = (srcs: ModelSrc[]) => Promise.all(srcs.map(modelSrcPromise))
    .then((fileData: LoadBytesRequestData[]) => {
      postMessage({msg: 'loadFiles', data: fileData}, fileData.map(f => f.bytes.buffer))
    })
    .catch((e) => {
      console.error('Failed to load model', e)  // eslint-disable-line no-console
    })

  const mergeBytes = (filename: string, bytes: Uint8Array, position, rotation) => {
    postMessage({msg: 'mergeBytes', data: {filename, bytes, position, rotation}}, [bytes.buffer])
  }

  const updateView = ({cameraPos, cameraRot, modelPos, modelRot}) => {
    postMessage({msg: 'updateView', data: {cameraPos, cameraRot, modelPos, modelRot}})
  }

  const configure = (config: Partial<ModelManagerConfiguration>) => {
    postMessage({msg: 'configure', config})
  }

  const onloaded = (cb) => {
    loadedCallback_ = cb
  }

  const onupdated = (cb) => {
    updatedCallback_ = cb
  }

  const onconfigApplied = (cb) => {
    configAppliedCallback_ = cb
  }

  return {
    loadBytes,
    loadFiles,
    loadModel,
    mergeBytes,
    updateView,
    configure,
    onloaded,
    onupdated,
    onconfigApplied,
    dispose,
  }
}

const ModelManager = {
  create,
  CoordinateSystem,
}

export {
  ModelManager,
  CoordinateSystem,
}
