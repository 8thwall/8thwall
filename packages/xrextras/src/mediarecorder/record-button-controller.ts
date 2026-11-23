import type {TargetedEvent} from 'preact'
import {useCallback, useEffect, useRef, useState} from 'preact/hooks'

import {configure, getConfig} from './capture-config'
import {drawWatermark} from './watermark'

const ACTIVE_TIMEOUT = 300

type CaptureMode = 'fixed' | 'photo' | 'standard'
type RecordButtonState =
  {status: 'waiting'} |
  {status: 'preview-open'} |
  {status: 'active', fixedMode: boolean} |
  {status: 'flash'} |
  {status: 'recording', fixedMode: boolean, progress: number} |
  {status: 'loading', progress: number} |
  {status: 'finalize-blocked'}

const defaultState = (): RecordButtonState => ({status: 'waiting'})

const useRecordButton = (captureMode: CaptureMode) => {
  const stateRef = useRef<RecordButtonState>(defaultState())
  const [state, setState] = useState(stateRef.current)
  const activeTimeout = useRef<number>()
  const captureModeRef = useRef(captureMode)
  const isDown = useRef(false)
  const isWaitingOnFinal = useRef(false)
  captureModeRef.current = captureMode

  const transition = useCallback((nextState: RecordButtonState) => {
    stateRef.current = nextState
    setState(nextState)
  }, [])

  const clearDisplayState = useCallback(() => {
    window.clearTimeout(activeTimeout.current)
    isDown.current = false
    transition(defaultState())
  }, [transition])

  const clearState = useCallback(() => {
    clearDisplayState()
    isWaitingOnFinal.current = false
  }, [clearDisplayState])

  const takeScreenshot = useCallback(async () => {
    const currentConfig = getConfig()
    transition({status: 'flash'})
    try {
      const data = await window.XR8.CanvasScreenshot.takeScreenshot({
        onProcessFrame: ({ctx}) => {
          currentConfig.onProcessFrame?.({ctx})
          drawWatermark(ctx)
        },
      })
      const bytes = atob(data)
      const buffer = new ArrayBuffer(bytes.length)
      const array = new Uint8Array(buffer)
      for (let i = 0; i < bytes.length; i++) {
        array[i] = bytes.charCodeAt(i)
      }
      const blob = new Blob([buffer], {type: 'image/jpeg'})
      clearState()
      window.dispatchEvent(new CustomEvent('mediarecorder-photocomplete', {detail: {blob}}))
    } catch {
      clearState()
    }
  }, [clearState, transition])

  const showLoading = useCallback(() => {
    const {current} = stateRef
    if (current.status !== 'recording') {
      return
    }
    transition({status: 'loading', progress: current.progress})
  }, [transition])

  const endRecording = useCallback(() => {
    if (stateRef.current.status !== 'recording') {
      return
    }
    window.XR8.MediaRecorder.stopRecording()
    showLoading()
  }, [showLoading])

  const startRecording = useCallback(() => {
    const {current} = stateRef
    if (current.status !== 'active') {
      return
    }
    const currentConfig = getConfig()
    transition({status: 'recording', fixedMode: current.fixedMode, progress: 100})

    window.XR8.MediaRecorder.recordVideo({
      onVideoReady: (result) => {
        isWaitingOnFinal.current = false
        if (stateRef.current.status === 'finalize-blocked') {
          clearState()
        }
        window.dispatchEvent(new CustomEvent('mediarecorder-recordcomplete', {detail: result}))
      },
      onStart: result => window.dispatchEvent(
        new CustomEvent('mediarecorder-recordstart', {detail: result})
      ),
      onStop: (result) => {
        window.dispatchEvent(new CustomEvent('mediarecorder-recordstop', {detail: result}))
        showLoading()
      },
      onError: (result) => {
        window.dispatchEvent(new CustomEvent('mediarecorder-recorderror', {detail: result}))
        clearState()
      },
      onProcessFrame: (frameInfo) => {
        const {elapsedTimeMs, maxRecordingMs, ctx} = frameInfo
        const recorderState = stateRef.current
        if (recorderState.status === 'recording') {
          transition({
            ...recorderState,
            progress: 100 * (1 - elapsedTimeMs / maxRecordingMs),
          })
        }
        currentConfig.onProcessFrame?.(frameInfo)
        drawWatermark(ctx)
      },
      onPreviewReady: (result) => {
        isWaitingOnFinal.current = true
        window.dispatchEvent(new CustomEvent('mediarecorder-previewready', {detail: result}))
      },
      onFinalizeProgress: result => window.dispatchEvent(
        new CustomEvent('mediarecorder-finalizeprogress', {detail: result})
      ),
    })
  }, [clearState, showLoading, transition])

  const goActive = useCallback(() => {
    if (stateRef.current.status !== 'waiting') {
      return
    }
    transition({status: 'active', fixedMode: false})
    activeTimeout.current = window.setTimeout(startRecording, ACTIVE_TIMEOUT)
  }, [startRecording, transition])

  const cancelActive = useCallback(() => {
    if (stateRef.current.status !== 'active') {
      return
    }
    window.clearTimeout(activeTimeout.current)
    takeScreenshot()
  }, [takeScreenshot])

  const down = useCallback((event: TargetedEvent<HTMLButtonElement>) => {
    event.preventDefault()
    if (isDown.current) {
      return
    }
    isDown.current = true

    if (captureModeRef.current === 'fixed') {
      if (stateRef.current.status === 'waiting') {
        transition({status: 'active', fixedMode: true})
        startRecording()
      } else if (stateRef.current.status === 'recording') {
        endRecording()
      }
    } else if (captureModeRef.current === 'photo') {
      takeScreenshot()
    } else if (stateRef.current.status === 'waiting') {
      goActive()
    }
  }, [endRecording, goActive, startRecording, takeScreenshot, transition])

  useEffect(() => {
    const up = () => {
      if (!isDown.current) {
        return
      }
      isDown.current = false
      if (captureModeRef.current !== 'standard') {
        return
      }
      if (stateRef.current.status === 'active') {
        cancelActive()
      }
      if (stateRef.current.status === 'recording') {
        endRecording()
      }
    }

    const previewOpened = () => {
      window.clearTimeout(activeTimeout.current)
      isDown.current = false
      transition({status: 'preview-open'})
    }

    const previewClosed = () => {
      if (isWaitingOnFinal.current) {
        transition({status: 'finalize-blocked'})
      } else {
        clearState()
      }
    }

    window.XR8.addCameraPipelineModule(window.XR8.MediaRecorder.pipelineModule())
    // Host integrations may already provide this shared module; XR8 warns and skips duplicates.
    window.XR8.addCameraPipelineModule(window.XR8.CanvasScreenshot.pipelineModule())
    window.addEventListener('mouseup', up)
    window.addEventListener('touchend', up)
    window.addEventListener('mediarecorder-previewclosed', previewClosed)
    window.addEventListener('mediarecorder-previewopened', previewOpened)
    configure()
    return () => {
      window.XR8.removeCameraPipelineModule(window.XR8.MediaRecorder.pipelineModule().name)
      // Do not remove CanvasScreenshot because XRExtras cannot determine whether the host owns it.
      window.removeEventListener('mouseup', up)
      window.removeEventListener('touchend', up)
      window.removeEventListener('mediarecorder-previewclosed', previewClosed)
      window.removeEventListener('mediarecorder-previewopened', previewOpened)
      window.clearTimeout(activeTimeout.current)
    }
  }, [cancelActive, clearState, endRecording, transition])

  const active = ['active', 'flash', 'loading', 'recording'].includes(state.status)
  const fixedMode = (state.status === 'active' || state.status === 'recording') &&
    state.fixedMode
  const loading = state.status === 'loading' || state.status === 'finalize-blocked'
  const progress = state.status === 'loading' || state.status === 'recording'
    ? state.progress
    : 100

  return {
    active,
    down,
    fixedMode,
    flashing: state.status === 'flash',
    loading,
    previewOpen: state.status === 'preview-open',
    progress,
    recording: state.status === 'recording',
  }
}

export {useRecordButton}
export type {CaptureMode}
