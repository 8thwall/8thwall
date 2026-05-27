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
import {initializeLocal} from '../studio/local-sync-api'
import {getLocalStudioPath} from './desktop-paths'
import {Icon} from '../ui/components/icon'
import {JointToggleButton} from '../ui/components/joint-toggle-button'
import {StandardFieldLabel} from '../ui/components/standard-field-label'
import {StandardTextField} from '../ui/components/standard-text-field'
import {GITHUB_TEMPLATES} from './github-templates'
import {TemplateCard} from '../browse/template-card'
import {StandardModal} from '../ui/components/standard-modal'
import coverImg from '../static/cover-image.png'

const useStyles = createUseStyles({
  newProjectModal: {
    display: 'flex',
    width: '56.25rem',
    padding: '5rem 3rem',
    minHeight: '60vh',
    justifyContent: 'center',
    flexDirection: 'column',
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
  templateCarousel: {
    minWidth: 0,
    width: '100%',
    overflowX: 'scroll',
    display: 'flex',
    gap: '1rem',
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
  const [selectedTemplate, setSelectedTemplate] = React.useState<string | null>(null)

  const projectTitle = rawProjectTitle.trim()

  return (
    <AutoHeadingScope>
      <form
        className={classes.newProjectModal}
        onSubmit={async (e) => {
          setLoading(true)
          try {
            e.preventDefault()
            const res = await initializeLocal(projectTitle, location, selectedTemplate)
            await queryClient.invalidateQueries({queryKey: ['listProjects']})
            history.push(getLocalStudioPath(res.appKey))
          } finally {
            setLoading(false)
          }
        }}
      >
        <SpaceBetween direction='vertical'>
          <AutoHeading className={classes.header}>
            {t('new_project_modal.title.new')}
          </AutoHeading>
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

            <label htmlFor='new-project-template'>
              {t('new_project_modal.input.label.choose_template')}
            </label>
            <div className={classes.templateCarousel}>
              <TemplateCard
                name='new-project-template'
                checked={selectedTemplate === null}
                onChange={() => {
                  setSelectedTemplate(null)
                }}
                title={t('new_project_modal.input.title.empty_project')}
                imageUrl={coverImg}
              />
              {GITHUB_TEMPLATES.map(template => (
                <TemplateCard
                  key={template.zipUrl}
                  name='new-project-template'
                  checked={selectedTemplate === template.zipUrl}
                  onChange={() => {
                    setSelectedTemplate(template.zipUrl)
                  }}
                  title={template.title}
                  imageUrl={template.imageUrl}
                />
              ))}
            </div>

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

const NewProjectButton: React.FC = () => {
  const {t} = useTranslation(['studio-desktop-pages'])
  return (
    <StandardModal
      trigger={(
        <PrimaryButton>
          <Icon inline stroke='plus' />
          <span>{t('home_page.button.new_project')}</span>
        </PrimaryButton>
      )}
    >
      {onClose => (
        <NewProjectContent
          onClose={onClose}
        />
      )}
    </StandardModal>
  )
}

export {
  NewProjectButton,
}
