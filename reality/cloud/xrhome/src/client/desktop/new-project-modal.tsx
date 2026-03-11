import React from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'
import {useQueryClient} from '@tanstack/react-query'
import {useHistory} from 'react-router-dom'

import {PrimaryButton} from '../ui/components/primary-button'
import {SecondaryButton} from '../ui/components/secondary-button'
import {SpaceBetween} from '../ui/layout/space-between'
import AutoHeading from '../widgets/auto-heading'
import AutoHeadingScope from '../widgets/auto-heading-scope'
import {FloatingTrayModal} from '../ui/components/floating-tray-modal'
import {initializeLocal} from '../studio/local-sync-api'
import {getLocalStudioPath} from './desktop-paths'
import {useBooleanUrlState} from '../hooks/url-state'
import {Icon} from '../ui/components/icon'
import {JointToggleButton} from '../ui/components/joint-toggle-button'
import {StandardFieldLabel} from '../ui/components/standard-field-label'
import {StandardTextField} from '../ui/components/standard-text-field'

const useStyles = createUseStyles({
  modalContainer: {
    display: 'flex',
    width: '56.25rem',
    padding: '5rem 3rem',
    minHeight: '60vh',
    justifyContent: 'center',
  },
  header: {
    fontFamily: 'Geist Mono !important',
    margin: 0,
  },
  appImage: {
    width: '12.25rem',
    aspectRatio: '98 / 55',
    borderRadius: '0.5rem',
    objectFit: 'cover',
  },
})

interface INewProjectContent {
  onClose: () => void
}

const NewProjectContent: React.FC<INewProjectContent> = ({
  onClose,
}) => {
  const {t} = useTranslation(['studio-desktop-pages', 'common'])
  const classes = useStyles()
  const [rawProjectTitle, setProjectTitle] = React.useState('')
  const [location, setLocation] = React.useState<'default' | 'prompt'>('default')
  const queryClient = useQueryClient()
  const history = useHistory()
  const [loading, setLoading] = React.useState(false)

  const projectTitle = rawProjectTitle.trim()

  return (
    <AutoHeadingScope>
      <form
        className={classes.modalContainer}
        onSubmit={async (e) => {
          setLoading(true)
          try {
            e.preventDefault()
            const res = await initializeLocal(projectTitle, location)
            await queryClient.invalidateQueries({queryKey: ['listProjects']})
            history.push(getLocalStudioPath(res.appKey))
          } finally {
            setLoading(false)
          }
        }}
      >
        <SpaceBetween direction='vertical'>
          <SpaceBetween direction='vertical' extraNarrow>
            <AutoHeading className={classes.header}>
              {t('new_project_modal.title.new')}
            </AutoHeading>
            <span> {t('new_project_modal.text.tagline')}</span>
          </SpaceBetween>
          <SpaceBetween direction='vertical'>
            <SpaceBetween direction='vertical' narrow>
              <StandardTextField
                label={t('new_project_modal.input.prompt.title')}
                value={projectTitle}
                onChange={(e) => {
                  const {value} = e.target
                  setProjectTitle(value)
                }}
              />
              <div>
                <StandardFieldLabel label={t('new_project_modal.input.label.folder_location')} />
                <JointToggleButton
                  options={[
                    {
                      value: 'default',
                      content: t('new_project_modal.input.label.default_location'),
                    },
                    {
                      value: 'prompt',
                      content: t('new_project_modal.input.label.custom_location'),
                    },
                  ] as const}
                  value={location}
                  onChange={e => setLocation(e)}
                />
              </div>
            </SpaceBetween>
            <PrimaryButton
              type='submit'
              disabled={!projectTitle}
              loading={loading}
            >
              {t('button.create', {ns: 'common'})}
            </PrimaryButton>
            <SecondaryButton onClick={() => onClose()}>
              {t('button.cancel', {ns: 'common'})}
            </SecondaryButton>
          </SpaceBetween>
        </SpaceBetween>
      </form>
    </AutoHeadingScope>
  )
}

interface INewProjectModalWithContext {
  onClose: () => void
}

const NewProjectModal: React.FC<INewProjectModalWithContext> = ({
  onClose,
}) => {
  useTranslation(['common'])

  return (
    <FloatingTrayModal
      startOpen
      onOpenChange={(open) => {
        if (!open) {
          onClose()
        }
      }}
      trigger={undefined}
    >
      {() => (
        <NewProjectContent
          onClose={onClose}
        />
      )}
    </FloatingTrayModal>
  )
}

const NewProjectButton: React.FC = () => {
  const {t} = useTranslation(['studio-desktop-pages'])
  const [isNewProjectModalOpen, setIsNewProjectModalOpen] = useBooleanUrlState('newProject', false)
  return (
    <>
      <PrimaryButton onClick={() => setIsNewProjectModalOpen(true)}>
        <Icon inline stroke='plus' />
        <span>{t('home_page.button.new_project')}</span>
      </PrimaryButton>
      {isNewProjectModalOpen &&
        <NewProjectModal
          onClose={() => {
            setIsNewProjectModalOpen(false)
          }}
        />
              }
    </>
  )
}

export {
  NewProjectModal,
  NewProjectButton,
}
