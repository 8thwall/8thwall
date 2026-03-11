import React from 'react'

import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import type {ReleaseNotesData} from '@ecs/shared/release-notes-types'
import {useSuspenseQuery} from '@tanstack/react-query'

import {Loader} from '../ui/components/loader'
import {StaticBanner} from '../ui/components/banner'
import {ReleaseNote} from './release-note'
import {ErrorBoundary} from '../common/error-boundary'
import {resolveServerRoute} from '../common/paths'

const useStyles = createUseStyles({
  releaseNotes: {
    overflow: 'auto',
  },
})

const RELEASE_NOTES_QUERY = {
  queryKey: ['releaseNotes'],
  queryFn: async (): Promise<ReleaseNotesData> => {
    const response = await fetch(resolveServerRoute('/docs/assets/json/studio-release-notes.json'))
    if (!response.ok) {
      throw new Error(`Failed to fetch release notes: ${response.statusText}`)
    }
    return response.json()
  },
} as const

const ReleaseNotesInner: React.FC = () => {
  const classes = useStyles()

  const {data} = useSuspenseQuery(RELEASE_NOTES_QUERY)
  const latestId = data.notes[0]?.id

  return (
    <div className={classes.releaseNotes}>
      {data.notes.map(releaseNoteData => (
        <ReleaseNote
          key={releaseNoteData.id}
          isLatest={releaseNoteData.id === latestId}
          {...releaseNoteData}
        />
      ))}
    </div>
  )
}

const ReleaseNotes: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <React.Suspense fallback={<Loader centered size='small' inline />}>
      <ErrorBoundary
        fallback={({onReset}) => (
          <StaticBanner
            type='danger'
            message={t('release_notes.failed_to_load')}
            onClose={onReset}
          />
        )}
      >
        <ReleaseNotesInner />
      </ErrorBoundary>
    </React.Suspense>
  )
}

export {
  ReleaseNotes,
  RELEASE_NOTES_QUERY,
}
