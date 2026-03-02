import {ResourceUrls} from '../resources'

// Start the mediaWorker.
let workerPromise

const initWorker = () => {
  if (!workerPromise) {
    workerPromise = new Promise((resolve) => {
      // to use your local version of media-recorder-worker.js, you can update the importScripts
      // link to https://<your_ip>:8888/media-recorder-worker.js.  This is defined in the
      // media-recorder-js-asm build rule as the prejs tag.  To update the CDN, use:
      //  code8/apps/client/public/web/cdn/resources/upload-resource.js
      const url = ResourceUrls.resolveMediaWorker()
      const blob = new Blob([`
        importScripts(${JSON.stringify(url)});
        MR8CC().then(() => self.initWorker())
      `], {type: 'application/javascript'})

      const workerUrl = URL.createObjectURL(blob)

      const mediaWorker = new Worker(workerUrl)

      // Initially listen only for whether the worker is loaded.
      mediaWorker.onmessage = (e) => {
        if (e.data.type === 'loaded') {
          mediaWorker.onmessage = null
          resolve(mediaWorker)
        }
      }
    })
  }
  return workerPromise
}

export {
  initWorker,
}
