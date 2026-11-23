import type {ComponentDefinition} from 'aframe'

import type {
  ConfigureAudioOutput, MediaRecorderConfigOptions,
} from '../../mediarecorder/capture-config'
import {defaultStrings} from '../../mediarecorder/strings'
import type {MediaRecorderStringOverrides} from '../../mediarecorder/strings'
import {ensureXrAndExtras} from '../ensure'

declare const THREE: any
declare const XRExtras: any

const stringKeys = Object.keys(defaultStrings) as Array<keyof typeof defaultStrings>
const stringSchema = Object.fromEntries(
  stringKeys.map(key => [key, {type: 'string'}])
)
const mediaRecorderConfigSchema = {
  enableEndCard: {type: 'boolean'},
  shortLink: {type: 'string'},
  coverImageUrl: {type: 'string'},
  footerImageUrl: {type: 'string'},
  maxDurationMs: {type: 'int'},
  endCardCallToAction: {type: 'string'},
  maxDimension: {type: 'int'},
  watermarkImageUrl: {type: 'string'},
  watermarkMaxWidth: {type: 'number'},
  watermarkMaxHeight: {type: 'number'},
  watermarkLocation: {type: 'string'},
  fileNamePrefix: {type: 'string'},
  requestMic: {type: 'string'},
} satisfies Record<
  Exclude<keyof MediaRecorderConfigOptions,
  'audioContext' | 'configureAudioOutput' | 'onProcessFrame'>,
  {type: string}
>
const captureConfigSchema = {
  ...mediaRecorderConfigSchema,
  includeSceneAudio: {type: 'boolean', default: true},
  ...stringSchema,
}
const captureConfigMappings = Object.fromEntries(
  Object.keys(captureConfigSchema).map(key => [
    key.replace(/[A-Z]/g, letter => `-${letter.toLowerCase()}`),
    `xrextras-capture-config.${key}`,
  ])
)

const getMediaRecorderStringOverrides = (
  values: Record<string, string>
): MediaRecorderStringOverrides => Object.fromEntries(
  stringKeys
    .filter(key => values[key] !== undefined)
    .map(key => [key, values[key]])
) as MediaRecorderStringOverrides

const captureButtonComponent: ComponentDefinition = {
  schema: {
    captureMode: {default: 'standard'},
  },
  async init() {
    await ensureXrAndExtras()
    if (this.removed) {
      return
    }
    this.added = true
    XRExtras.MediaRecorder.initRecordButton()
    XRExtras.MediaRecorder.setCaptureMode(this.data.captureMode)
  },
  update() {
    if (this.added) {
      XRExtras.MediaRecorder.setCaptureMode(this.data.captureMode)
    }
  },
  remove() {
    this.removed = true
    if (this.added) {
      XRExtras.MediaRecorder.removeRecordButton()
    }
  },
}

const capturePreviewComponent: ComponentDefinition = {
  // This schema must be duplicated to mappings xr-primitives.js
  schema: {
    actionButtonShareText: {default: ''},
    actionButtonViewText: {default: ''},
    finalizeText: {default: ''},
  },
  async init() {
    await ensureXrAndExtras()
    if (!this.removed) {
      this.added = true
      XRExtras.MediaRecorder.initMediaPreview(this.data)
    }
  },
  remove() {
    this.removed = true
    if (this.added) {
      XRExtras.MediaRecorder.removeMediaPreview()
    }
  },
}

interface CaptureComponentDefinition extends ComponentDefinition {
  includeSceneAudio: ConfigureAudioOutput
}

const captureConfigComponent: CaptureComponentDefinition = {
  schema: captureConfigSchema,
  init() {
    this.includeSceneAudio = this.includeSceneAudio.bind(this)
  },
  update() {
    const configValues = Object.fromEntries(
      (Object.keys(mediaRecorderConfigSchema) as Array<keyof MediaRecorderConfigOptions>)
        .filter(key => this.attrValue[key] !== undefined)
        .map(key => [key, this.data[key]])
    ) as Partial<MediaRecorderConfigOptions>
    const config: MediaRecorderConfigOptions = {
      ...configValues,
      audioContext: THREE.AudioContext.getContext(),
      configureAudioOutput: this.data.includeSceneAudio ? this.includeSceneAudio : null,
    }

    XRExtras.MediaRecorder.reset()
    XRExtras.MediaRecorder.configure(config)
    XRExtras.MediaRecorder.setStringOverrides(
      getMediaRecorderStringOverrides(this.attrValue)
    )
  },
  remove() {
    if (window.XRExtras) {
      XRExtras.MediaRecorder.reset()
    }
  },
  includeSceneAudio({microphoneInput, audioProcessor}) {
    const audioContext = audioProcessor.context

    // if the scene doesn't have any audio, then we'll create the listener for the scene.
    // That way, if they add sounds later, it will still connect without the user having to
    // re-call this function.
    if (!this.el.sceneEl.audioListener) {
      this.el.sceneEl.audioListener = new THREE.AudioListener()
    }

    // This connects the A-Frame audio to the audioProcessor so that all sound effects initialized
    // are part of the recorded video's audio.
    this.el.sceneEl.audioListener.gain.connect(audioProcessor)
    // This connects the A-Frame audio to the hardware output.  That way, the user can also hear
    // the sound effects during the experience
    this.el.sceneEl.audioListener.gain.connect(audioContext.destination)

    // you must return a node at the end.  This node is connected to the audioProcessor
    // automatically inside MediaRecorder
    return microphoneInput
  },
}

export {
  captureButtonComponent,
  captureConfigMappings,
  capturePreviewComponent,
  captureConfigComponent,
  CaptureComponentDefinition,
}
