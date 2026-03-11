import React from 'react'

import {useTranslation} from 'react-i18next'

import {ProgressMessageEvent, useAppPreviewStyles} from './app-preview-utils'
import {IconButton} from '../../ui/components/icon-button'
import {useEvent} from '../../hooks/use-event'
import {createThemedStyles, useUiTheme} from '../../ui/theme'
import {useSimulator} from './use-simulator-state'
import {combine} from '../../common/styles'
import {Popup} from '../../ui/components/popup'

const useStyles = createThemedStyles(theme => ({
  sequenceProgressBar: {
    flex: '1 0 0',
    alignSelf: 'center',
    height: '0.25rem',
    borderRadius: '2rem',
    backgroundColor: theme.playbarBackground,
    display: 'flex',
    alignItems: 'center',
    position: 'relative',
  },
  sequencePlaybackRangeMask: {
    position: 'absolute',
    height: '1rem',
    borderTopLeftRadius: '.25rem',
    borderTopRightRadius: '.25rem',
    backgroundColor: theme.playbarMaskBg,
  },
  sequenceProgressSeekMarker: {
    position: 'absolute',
    backgroundColor: theme.fgMain,
    height: '1rem',
    width: '4px',
    borderRadius: '.5rem',
    transform: 'translateX(-50%)',
    zIndex: 2,
  },
  sequenceProgressSeekMarkerBefore: {
    '&::before': {
      content: '""',
      position: 'absolute',
      top: 0,
      left: '-4px',
      right: '-4px',
      bottom: 0,
      backgroundColor: 'transparent',
      zIndex: 0,
    },
  },
  sequenceProgressStartMarker: {
    position: 'absolute',
    height: '1rem',
    width: '.75rem',
    zIndex: 1,
  },
  sequenceProgressEndMarker: {
    position: 'absolute',
    height: '1rem',
    width: '.75rem',
    zIndex: 1,
  },
}))

const StartMarkerIndicator: React.FC = () => {
  const theme = useUiTheme()
  return (
    <svg width='9' height='9'>
      <path
        // eslint-disable-next-line max-len
        d='M8.68018 8.19258V1.10443C8.68018 0.406494 7.83487 0.0586046 7.3436 0.554363L0.319661 7.64251C-0.169192 8.13584 0.18025 8.97406 0.874761 8.97406H7.8987C8.3303 8.97406 8.68018 8.62418 8.68018 8.19258Z'
        fill={theme.playbarMarkerBg}
      />
    </svg>
  )
}

const EndMarkerIndicator: React.FC = () => {
  const theme = useUiTheme()
  return (
    <svg width='9' height='9'>
      <path
            // eslint-disable-next-line max-len
        d='M0.119934 8.19258V1.10443C0.119934 0.406494 0.965244 0.0586046 1.45651 0.554363L8.48045 7.64251C8.9693 8.13584 8.61986 8.97406 7.92535 8.97406H0.901414C0.469815 8.97406 0.119934 8.62418 0.119934 8.19258Z'
        fill={theme.playbarMarkerBg}
      />
    </svg>
  )
}

interface ISequenceProgressBar {
  simulatorId: string
  handleScrub: (action: string, data?: {progress: number}) => void
  isPaused: boolean
}

const SequenceProgressBar: React.FC<ISequenceProgressBar> = (
  {simulatorId, handleScrub, isPaused}
) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-editor-pages'])

  const {simulatorState, updateSimulatorState} = useSimulator()

  const start = simulatorState?.start || 0
  const end = simulatorState?.end || 1

  const progressBarRef = React.useRef<HTMLDivElement>(null)
  const draggingRef = React.useRef<'start' | 'end' | 'progress'>(null)
  const [_startPoint, setStartPoint] = React.useState(undefined)
  const [_endPoint, setEndPoint] = React.useState(undefined)
  const startPoint = _startPoint ?? start
  const endPoint = _endPoint ?? end
  const [currentProgress, setCurrentProgress] = React.useState(start)

  const [hasUpdatedStart, setHasUpdatedStart] = React.useState(false)
  const [hasUpdatedEnd, setHasUpdatedEnd] = React.useState(false)

  const isProgressMidway = currentProgress > startPoint && currentProgress < endPoint

  React.useEffect(() => {
    const handleMessage = (event: ProgressMessageEvent) => {
      if (event.data.action === 'SEQUENCE_PROGRESS') {
        if (event.data.data.simulatorId === simulatorId && !isPaused) {
          setCurrentProgress(event.data.data.currentProgress)
        }
      }
    }

    window.addEventListener('message', handleMessage)

    return () => {
      window.removeEventListener('message', handleMessage)
    }
  }, [isPaused])

  React.useEffect(() => {
    if (start !== startPoint && !hasUpdatedStart) {
      setStartPoint(start)
      setHasUpdatedStart(true)
    }
  }, [start])

  React.useEffect(() => {
    if (end !== endPoint && !hasUpdatedEnd) {
      setEndPoint(end)
      setHasUpdatedEnd(true)
    }
  }, [end])

  const handlePointMarkerDrag = useEvent((e: MouseEvent) => {
    if (!progressBarRef.current || !draggingRef.current) {
      return
    }
    const progressBarRect = progressBarRef.current?.getBoundingClientRect()
    const progressWidth = progressBarRect.width
    const progressLeft = progressBarRect.left
    const mouseX = e.clientX
    if (draggingRef.current === 'start') {
      const newPoint = Math.max(0, Math.min(endPoint, (mouseX - progressLeft) / progressWidth))
      setStartPoint(newPoint)
    } else if (draggingRef.current === 'end') {
      const newPoint = Math.max(startPoint, Math.min(1, (mouseX - progressLeft) / progressWidth))
      setEndPoint(newPoint)
    } else if (draggingRef.current === 'progress') {
      const newPoint = Math.max(
        startPoint, Math.min(endPoint, (mouseX - progressLeft) / progressWidth)
      )
      handleScrub('SIMULATOR_SCRUB', {progress: currentProgress})
      setCurrentProgress(newPoint)
    }
    e.stopPropagation()
    e.preventDefault()
  })

  const handleStartPointMarkerDrag = (e: React.MouseEvent<HTMLDivElement, MouseEvent>) => {
    draggingRef.current = 'start'
    e.stopPropagation()
    e.preventDefault()
  }

  const handleEndPointMarkerDrag = (e: React.MouseEvent<HTMLDivElement, MouseEvent>) => {
    draggingRef.current = 'end'
    e.stopPropagation()
    e.preventDefault()
  }

  const handleProgressMarkerDrag = (e: React.MouseEvent<HTMLDivElement, MouseEvent>) => {
    draggingRef.current = 'progress'
    handleScrub('SIMULATOR_SCRUB', {progress: currentProgress})
    e.stopPropagation()
    e.preventDefault()
  }

  const handlePointMarkerRelease = useEvent((
    e: React.MouseEvent<HTMLDivElement, MouseEvent> | MouseEvent
  ) => {
    if (!draggingRef.current) {
      return
    }
    e.stopPropagation()
    e.preventDefault()
    updateSimulatorState({start: startPoint, end: endPoint})
    setStartPoint(undefined)
    setEndPoint(undefined)
    handleScrub('SIMULATOR_STOP_SCRUB')
    draggingRef.current = null
  })

  React.useEffect(() => {
    document.addEventListener('mousemove', handlePointMarkerDrag)
    document.addEventListener('mouseup', handlePointMarkerRelease)

    return () => {
      document.removeEventListener('mousemove', handlePointMarkerDrag)
      document.removeEventListener('mouseup', handlePointMarkerRelease)
    }
  }, [])

  return (
    <div
      className={classes.sequenceProgressBar}
      ref={progressBarRef}
    >
      <div
        className={classes.sequencePlaybackRangeMask}
        style={{
          width: `${(endPoint - startPoint) * 100}%`,
          left: `${startPoint * 100}%`,
        }}
      />
      <div
        className={classes.sequenceProgressStartMarker}
        role='button'
        aria-label={t('editor_page.inline_app_preview.iframe.playbar.start_point_button.text')}
        style={{left: `calc(${startPoint * 100}% - 9px)`}}
        onMouseDown={handleStartPointMarkerDrag}
        onMouseUp={handlePointMarkerRelease}
        tabIndex={0}
      >
        <StartMarkerIndicator />
      </div>
      <div
        className={combine(classes.sequenceProgressSeekMarker,
          isProgressMidway && classes.sequenceProgressSeekMarkerBefore)}
        role='button'
        aria-label={t('editor_page.inline_app_preview.iframe.playbar.current_progress_button.text')}
        style={{left: `${currentProgress * 100}%`}}
        onMouseDown={handleProgressMarkerDrag}
        onMouseUp={handlePointMarkerRelease}
        tabIndex={0}
      />
      <div
        className={classes.sequenceProgressEndMarker}
        role='button'
        aria-label={t('editor_page.inline_app_preview.iframe.playbar.end_point_button.text')}
        style={{left: `${endPoint * 100}%`}}
        onMouseDown={handleEndPointMarkerDrag}
        onMouseUp={handlePointMarkerRelease}
        tabIndex={0}
      >
        <EndMarkerIndicator />
      </div>
    </div>
  )
}

interface IAppPreviewPlaybar {
  simulatorId: string
  broadcastSimulatorMessage: (action: string, data: unknown) => void
}

const AppPreviewPlaybar: React.FC<IAppPreviewPlaybar> = ({
  simulatorId, broadcastSimulatorMessage,
}) => {
  const appPreviewStyles = useAppPreviewStyles()
  const {t} = useTranslation(['cloud-editor-pages'])

  const {simulatorState, updateSimulatorState} = useSimulator()

  const isPaused = !!simulatorState?.isPaused || (
    !!simulatorState?.studioPause && simulatorState.inlinePreviewDebugActive
  )
  return (
    <div className={appPreviewStyles.appPreviewPlaybar}>
      <div className={appPreviewStyles.actionButton}>
        <IconButton
          a8={`click;studio;simulator-${isPaused ? 'play' : 'pause'}-button`}
          stroke={isPaused ? 'play' : 'pause'}
          disabled={!!simulatorState.studioPause && simulatorState.inlinePreviewDebugActive}
          onClick={(e) => {
            e.stopPropagation()
            updateSimulatorState({isPaused: !isPaused})
          }}
          onKeyDown={(e) => {
            e.stopPropagation()
          }}
          text={t(isPaused
            ? t('editor_page.inline_app_preview.iframe.playbar.pause_button.text')
            : t('editor_page.inline_app_preview.iframe.playbar.play_button.text'))}
        />
      </div>
      <SequenceProgressBar
        simulatorId={simulatorId}
        handleScrub={broadcastSimulatorMessage}
        isPaused={isPaused}
      />
      <div className={combine(appPreviewStyles.actionButton, appPreviewStyles.withFeedback)}>
        <Popup
          content={t('editor_page.inline_app_preview.button.recenter')}
          position='top'
          alignment='right'
          size='tiny'
          delay={750}
        >
          <IconButton
            stroke='recenter'
            text={t('editor_page.inline_app_preview.button.recenter')}
            onClick={() => {
              broadcastSimulatorMessage('SIMULATOR_RECENTER', null)
            }}
          />
        </Popup>
      </div>
    </div>
  )
}

export {
  AppPreviewPlaybar,
}
