import React from 'react'
import {Link, useHistory} from 'react-router-dom'

import {useQueryClient} from '@tanstack/react-query'

import {Trans, useTranslation} from 'react-i18next'

import type {ProjectClientSide} from '../../shared/desktop/local-sync-types'
import {getLocalStudioPath} from './desktop-paths'
import {deriveAppCoverImageUrl} from '../../shared/app-utils'
import {COVER_IMAGE_PREVIEW_SIZES} from '../../shared/app-constants'
import {SpaceBetween} from '../ui/layout/space-between'
import {combine} from '../common/styles'
import {ProjectListItemOptions} from './project-list-item-options'
import {SecondaryButton} from '../ui/components/secondary-button'
import {
  deleteProject, migrateProject, moveProject, pickNewProjectLocation,
} from '../studio/local-sync-api'
import {ProjectListModal} from './project-list-modal'
import {basename} from '../editor/editor-common'
import {createThemedStyles} from '../ui/theme'
import {StandardLink} from '../ui/components/standard-link'
import {Icon} from '../ui/components/icon'

const useStyles = createThemedStyles(theme => ({
  projectListItem: {
    'display': 'flex',
    'fontFamily': 'Geist Mono !important',
    'listStyleType': 'none',
    'padding': '0.5rem 1rem 0.5rem 0.5rem',
    'border': '1px solid transparent',
    'position': 'relative',
  },
  coverImage: {
    width: '10rem',
    aspectRatio: '70/43',
    borderRadius: '0.5rem',
    objectFit: 'cover',
  },
  buttonArea: {
    position: 'relative',
    zIndex: 2,
    pointerEvents: 'auto',
    display: 'flex',
    gap: '0.5rem',
    alignItems: 'center',
  },
  link: {
    'position': 'absolute',
    'top': 0,
    'left': 0,
    'right': 0,
    'bottom': 0,
    'zIndex': 1,
    'borderRadius': '0.5rem',
    '&:hover': {
      'background': theme.studioBtnHoverBg,
    },
    '&:focus-visible': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
  },
  content: {
    display: 'flex',
    flexGrow: 1,
    position: 'relative',
    zIndex: 2,
    pointerEvents: 'none',
    width: '100%',
    minWidth: 0,
  },
  textContainer: {
    minWidth: 0,
    maxWidth: '100%',
    flex: 1,
    overflow: 'hidden',
  },
  appName: {
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
    display: 'block',
  },
  projectLocation: {
    color: theme.fgMuted,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
    direction: 'rtl',
  },
  projectError: {
    color: theme.fgError,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
  },
  projectWarning: {
    color: theme.fgWarning,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
  },
}))

interface IProjectListItem {
  project: ProjectClientSide & {appKey: string}
}

const ProjectListItem: React.FC<IProjectListItem> = ({project}) => {
  const classes = useStyles()
  const {t} = useTranslation(['studio-desktop-pages'])
  const queryClient = useQueryClient()
  const [isMovingAppModalOpen, setIsMovingAppModalOpen] = React.useState(false)
  const [targetLocation, setTargetLocation] = React.useState<string>('')
  const [isMigrateModalOpen, setIsMigrateModalOpen] = React.useState(false)
  const history = useHistory()

  const onClose = () => {
    setIsMovingAppModalOpen(false)
    setIsMigrateModalOpen(false)
  }

  let requiredAction: 'locate' | 'migrate' | null = null
  if (!project.validLocation) {
    requiredAction = 'locate'
  } else if (project.initialization !== 'v2') {
    requiredAction = 'migrate'
  }

  return (
    <li className={classes.projectListItem}>
      {!requiredAction &&
        <Link
          to={getLocalStudioPath(project.appKey)}
          className={combine('style-reset', classes.link)}
        />
      }
      <div className={classes.content}>
        <SpaceBetween grow centered between noWrap>
          <SpaceBetween centered>
            <img
              className={classes.coverImage}
              draggable={false}
              src={deriveAppCoverImageUrl(null, COVER_IMAGE_PREVIEW_SIZES[600])}
              alt=''
            />
            <div className={classes.textContainer}>
              <SpaceBetween direction='vertical' narrow>
                <span className={classes.appName}>{basename(project.location)}</span>
                <span className={classes.projectLocation}>
                  {project.location}
                </span>
                {requiredAction === 'locate' &&
                  <span className={classes.projectError}>
                    {t('project_list_item.project_missing')}
                  </span>
                }
                {requiredAction === 'migrate' &&
                  <span className={classes.projectWarning}>
                    {t('project_list_item.project_needs_migration')}
                  </span>
                }
              </SpaceBetween>
            </div>
          </SpaceBetween>
          <div className={classes.buttonArea}>
            {requiredAction === 'locate' &&
              <SecondaryButton onClick={async () => {
                await moveProject(project.appKey)
                queryClient.invalidateQueries({queryKey: ['listProjects']})
              }}
              >
                {t('project_list_item.button.locate')}
              </SecondaryButton>
            }
            {requiredAction === 'migrate' &&
              <SecondaryButton onClick={async () => setIsMigrateModalOpen(true)}>
                {t('project_list_item.button.migrate')}
              </SecondaryButton>
            }
            <ProjectListItemOptions
              project={project}
              onDelete={() => {
                deleteProject(project.appKey)
                queryClient.invalidateQueries({queryKey: ['listProjects']})
              }}
              onMove={async () => {
                const {projectPath} = await pickNewProjectLocation(project.appKey)
                setTargetLocation(projectPath)
                setIsMovingAppModalOpen(true)
              }}
            />
          </div>
        </SpaceBetween>
      </div>
      {isMovingAppModalOpen && (
        <ProjectListModal
          onClose={onClose}
          onSubmit={async () => {
            await moveProject(project.appKey, targetLocation)
            queryClient.invalidateQueries({queryKey: ['listProjects']})
            onClose()
          }}
          header={`${t('project_list_item.modal.move')}`}
          content={(
            <SpaceBetween direction='vertical' centered extraNarrow>
              <Trans
                ns='studio-desktop-pages'
                i18nKey='project_list_item.move_project_confirmation'
                values={{
                  oldPath: project?.location || '',
                  newPath: targetLocation,
                }}
                components={{
                  oldBlock: <b />,
                  newBlock: <b />,
                }}
              />
            </SpaceBetween>
          )}
          submitContent={t('project_list_item.modal.button.move')}
        />
      )}
      {isMigrateModalOpen &&
        <ProjectListModal
          onClose={onClose}
          onSubmit={async () => {
            await migrateProject(project.appKey)
            queryClient.invalidateQueries({queryKey: ['listProjects']})
            history.push(getLocalStudioPath(project.appKey))
          }}
          header={t('project_list_item.modal.migrate.title')}
          content={(
            <SpaceBetween direction='vertical'>
              <p>{t('project_list_item.modal.migrate.paragraph_1')}</p>
              <div>
                <p>{t('project_list_item.modal.migrate.paragraph_2')}</p>
                <ul>
                  <li>{t('project_list_item.modal.migrate.item.project_structure')}</li>
                  <li>{t('project_list_item.modal.migrate.item.depdendencies')}</li>
                  <li>{t('project_list_item.modal.migrate.item.features')}</li>
                </ul>
              </div>
              <p>{t('project_list_item.modal.migrate.paragraph_3')}</p>
              <StandardLink newTab href='https://8th.io/migrate-project-offline'>
                {t('button.learn_more', {ns: 'common'})}
                <Icon inline stroke='external' />
              </StandardLink>
            </SpaceBetween>
          )}
          submitContent={t('project_list_item.modal.migrate.button')}
        />}
    </li>
  )
}

export {
  ProjectListItem,
}
