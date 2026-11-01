import {Fragment, h} from 'preact'
import type {FunctionComponent as FC, TargetedEvent} from 'preact'
import {useCallback, useEffect, useRef, useState} from 'preact/hooks'

import {combine} from '../classnames'
import {configure, getConfig} from './capture-config'
import {drawWatermark} from './watermark'

const ACTIVE_TIMEOUT = 300

type CaptureMode = 'fixed' | 'photo' | 'standard'
type RecorderStatus = 'active' | 'finalize-blocked' | 'flash' | 'loading' | 'recording' |
  'waiting'

interface RecordButtonState {
  active: boolean
  faded: boolean
  fixedMode: boolean
  flashing: boolean
  loading: boolean
  progress: number
  recording: boolean
}

interface RecordButtonViewProps {
  captureMode: CaptureMode
}

const defaultState = (): RecordButtonState => ({
  active: false,
  faded: false,
  fixedMode: false,
  flashing: false,
  loading: false,
  progress: 100,
  recording: false,
})

const RecordButtonView: FC<RecordButtonViewProps> = ({captureMode}) => {
  const [state, setState] = useState(defaultState)
  const activeTimeout = useRef<number>()
  const captureModeRef = useRef(captureMode)
  const isDown = useRef(false)
  const isWaitingOnFinal = useRef(false)
  const status = useRef<RecorderStatus>('waiting')
  captureModeRef.current = captureMode

  const clearDisplayState = useCallback(() => {
    window.clearTimeout(activeTimeout.current)
    isDown.current = false
    status.current = 'waiting'
    setState(defaultState())
  }, [])

  const clearState = useCallback(() => {
    clearDisplayState()
    isWaitingOnFinal.current = false
  }, [clearDisplayState])

  const previewOpened = useCallback(() => {
    window.clearTimeout(activeTimeout.current)
    isDown.current = false
    status.current = 'waiting'
    setState({...defaultState(), faded: true})
  }, [])

  const previewClosed = useCallback(() => {
    if (isWaitingOnFinal.current) {
      status.current = 'finalize-blocked'
      setState(current => ({...current, loading: true, faded: false}))
    } else {
      clearState()
    }
  }, [clearState])

  const takeScreenshot = useCallback(() => {
    const currentConfig = getConfig()
    status.current = 'flash'
    setState(current => ({...current, flashing: true}))
    window.XR8.CanvasScreenshot.takeScreenshot({
      onProcessFrame: ({ctx}) => {
        currentConfig.onProcessFrame?.({ctx})
        drawWatermark(ctx)
      },
    }).then((data) => {
      const bytes = atob(data)
      const buffer = new ArrayBuffer(bytes.length)
      const array = new Uint8Array(buffer)
      for (let i = 0; i < bytes.length; i++) {
        array[i] = bytes.charCodeAt(i)
      }
      const blob = new Blob([buffer], {type: 'image/jpeg'})
      clearState()
      window.dispatchEvent(new CustomEvent('mediarecorder-photocomplete', {detail: {blob}}))
    }).catch(clearState)
  }, [clearState])

  const showLoading = useCallback(() => {
    if (status.current !== 'recording') {
      return
    }
    status.current = 'loading'
    setState(current => ({
      ...current,
      fixedMode: false,
      recording: false,
      loading: true,
    }))
  }, [])

  const endRecording = useCallback(() => {
    if (status.current !== 'recording') {
      return
    }
    window.XR8.MediaRecorder.stopRecording()
    showLoading()
  }, [showLoading])

  const startRecording = useCallback(() => {
    if (status.current !== 'active') {
      return
    }
    const currentConfig = getConfig()
    status.current = 'recording'
    setState(current => ({...current, recording: true}))

    window.XR8.MediaRecorder.recordVideo({
      onVideoReady: (result) => {
        isWaitingOnFinal.current = false
        if (status.current === 'finalize-blocked') {
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
        setState(current => ({
          ...current,
          progress: 100 * (1 - elapsedTimeMs / maxRecordingMs),
        }))
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
  }, [clearState, showLoading])

  const goActive = useCallback(() => {
    if (status.current !== 'waiting') {
      return
    }
    status.current = 'active'
    setState(current => ({...current, active: true}))
    activeTimeout.current = window.setTimeout(startRecording, ACTIVE_TIMEOUT)
  }, [startRecording])

  const cancelActive = useCallback(() => {
    if (status.current !== 'active') {
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
      if (status.current === 'waiting') {
        status.current = 'active'
        setState(current => ({...current, fixedMode: true, active: true}))
        startRecording()
      } else if (status.current === 'recording') {
        endRecording()
      }
    } else if (captureModeRef.current === 'photo') {
      setState(current => ({...current, active: true}))
      takeScreenshot()
    } else if (status.current === 'waiting') {
      goActive()
    }
  }, [endRecording, goActive, startRecording, takeScreenshot])

  const up = useCallback(() => {
    if (!isDown.current) {
      return
    }
    isDown.current = false
    if (captureModeRef.current !== 'standard') {
      return
    }
    if (status.current === 'active') {
      cancelActive()
    }
    if (status.current === 'recording') {
      endRecording()
    }
  }, [cancelActive, endRecording])

  useEffect(() => {
    window.XR8.addCameraPipelineModule(window.XR8.MediaRecorder.pipelineModule())
    window.addEventListener('mouseup', up)
    window.addEventListener('touchend', up)
    window.addEventListener('mediarecorder-previewclosed', previewClosed)
    window.addEventListener('mediarecorder-previewopened', previewOpened)
    configure()
    return () => {
      window.XR8.removeCameraPipelineModule(window.XR8.MediaRecorder.pipelineModule().name)
      window.removeEventListener('mouseup', up)
      window.removeEventListener('touchend', up)
      window.removeEventListener('mediarecorder-previewclosed', previewClosed)
      window.removeEventListener('mediarecorder-previewopened', previewOpened)
      window.clearTimeout(activeTimeout.current)
    }
  }, [previewClosed, previewOpened, up])

  return (
    <Fragment>
      <div id='recorder' className={combine('recorder-container', 
        state.active && 'active',
    state.faded && 'fade-container',
    state.fixedMode && 'fixed-mode',
    state.loading && 'loading',
    state.recording && 'recording'
      )}>
        <svg viewBox='0 0 38 38' className='progress-container'>
          <circle className='progress-track' r='16' cx='19' cy='19' />
          <circle
            id='progressBar'
            className='progress-bar'
            r='16'
            cx='19'
            cy='19'
            style={{strokeDashoffset: state.progress}}
          />
          <circle className='loading-circle' r='16' cx='19' cy='19' />
        </svg>

        <button
          id='recorder-button'
          className='style-reset'
          type='button'
          onMouseDown={down}
          onTouchStart={down}
        >
          Record
        </button>
      </div>

      <div
        id='flashElement'
        className={combine('flash-element', state.flashing && 'flashing')}
      />
    </Fragment>
  )
}

export {RecordButtonView}
export type {CaptureMode}
