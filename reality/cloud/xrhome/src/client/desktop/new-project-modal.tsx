import React from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'
import {useQueryClient} from '@tanstack/react-query'
import {useHistory} from 'react-router-dom'

import {PrimaryButton} from '../ui/components/primary-button'
import {SpaceBetween} from '../ui/layout/space-between'
import AutoHeading from '../widgets/auto-heading'
import AutoHeadingScope from '../widgets/auto-heading-scope'
import {extractApiError, initializeLocal} from '../studio/local-sync-api'
import {getLocalStudioPath} from './desktop-paths'
import {Icon} from '../ui/components/icon'
import {JointToggleButton} from '../ui/components/joint-toggle-button'
import {StandardFieldLabel} from '../ui/components/standard-field-label'
import {StandardTextField} from '../ui/components/standard-text-field'
import {GITHUB_TEMPLATES} from './github-templates'
import {TemplateCard} from '../browse/template-card'
import {StandardModal} from '../ui/components/standard-modal'
import coverImg from '../static/cover-image.png'
import {BoldButton} from '../ui/components/bold-button'
import {useTypography} from '../ui/typography'
import {StandardModalActions} from '../ui/components/standard-modal-actions'
import {StandardModalHeader} from '../editor/standard-modal-header'
import {StandardModalContent} from '../ui/components/standard-modal-content'
import {StaticBanner} from '../ui/components/banner'

const useStyles = createUseStyles({
  newProjectModal: {
    display: 'flex',
    width: '56.25rem',
    justifyContent: 'center',
    flexDirection: 'column',
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
  const typography = useTypography()
  const {t} = useTranslation(['studio-desktop-pages', 'common'])
  const classes = useStyles()
  const [rawProjectTitle, setProjectTitle] = React.useState('')
  const [location, setLocation] = React.useState<'default' | 'prompt'>('default')
  const queryClient = useQueryClient()
  const history = useHistory()
  const [loading, setLoading] = React.useState(false)
  const [selectedTemplate, setSelectedTemplate] = React.useState<string | null>(null)
  const [error, setError] = React.useState('')

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
            history.push(getLocalStudioPath(res.appKey))
            queryClient.invalidateQueries({queryKey: ['listProjects']})
          } catch (err) {
            setError(await extractApiError(err))
          } finally {
            setLoading(false)
          }
        }}
      >
        <StandardModalHeader>
          <AutoHeading className={typography.heading4}>
            {t('new_project_modal.title.new')}
          </AutoHeading>
        </StandardModalHeader>
        <StandardModalContent>
          <SpaceBetween direction='vertical'>
            <StandardTextField
              label={t('new_project_modal.input.prompt.title')}
              value={projectTitle}
              autoFocus
              onChange={(e) => {
                const {value} = e.target
                setProjectTitle(value)
              }}
            />
            <label htmlFor='new-project-template'>
              {t('new_project_modal.input.label.template')}
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
            {error && <StaticBanner type='danger'>{error}</StaticBanner>}
          </SpaceBetween>
        </StandardModalContent>

        <StandardModalActions>
          <BoldButton onClick={() => onClose()}>
            {t('button.cancel', {ns: 'common'})}
          </BoldButton>
          <PrimaryButton
            type='submit'
            disabled={!projectTitle}
            loading={loading}
          >
            {t('button.create', {ns: 'common'})}
          </PrimaryButton>
        </StandardModalActions>
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
