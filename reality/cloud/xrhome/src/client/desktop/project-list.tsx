import React from 'react'
import {createUseStyles} from 'react-jss'
import {useSuspenseQuery} from '@tanstack/react-query'

import {useTranslation} from 'react-i18next'

import type {ListProjectsResponse} from '../../shared/desktop/local-sync-types'
import {SpaceBetween} from '../ui/layout/space-between'
import {ProjectListItem} from './project-list-item'
import AutoHeading from '../widgets/auto-heading'
import {combine} from '../common/styles'
import AutoHeadingScope from '../widgets/auto-heading-scope'
import {listProjects} from '../studio/local-sync-api'
import {SimpleTextInput} from './text-input'
import {NewProjectButton} from './new-project-modal'
import {OpenProjectButton} from './open-project-button'
import ErrorMessage from '../home/error-message'

const useStyles = createUseStyles({
  projectList: {
    'overflowY': 'auto',
    'display': 'grid',
    'gridTemplateColumns': 'repeat(auto-fit, minmax(40rem, 1fr))',
    // NOTE(johnny): Hide scrollbar but keep scrolling functionality
    'scrollbarWidth': 'none',  // Firefox
    'msOverflowStyle': 'none',  // IE and Edge
    '&::-webkit-scrollbar': {
      display: 'none',  // Chrome, Safari, Opera
    },
  },
  heading: {
    fontFamily: 'Geist Mono',
    fontSize: '1rem',
    letterSpacing: '0.075rem',
    flexGrow: 0,
  },
  searchInput: {
    display: 'flex',
    height: '3rem',
    width: '24rem',
  },
  sortDropdown: {
    width: '11rem',
  },
  emptyState: {
    paddingTop: '4rem',
  },
})

const ProjectList: React.FC = () => {
  const classes = useStyles()
  const {t} = useTranslation(['studio-desktop-pages'])
  const [appSearch, setAppSearch] = React.useState('')

  const listProjectsQuery = useSuspenseQuery<ListProjectsResponse>({
    queryKey: ['listProjects'],
    queryFn: listProjects,
  })

  const searchLower = appSearch.toLowerCase()

  const visibleProjects = Object.entries(listProjectsQuery.data.projectByAppKey)
    .filter(e => e[1].location.toLowerCase().includes(searchLower))
    .map(e => ({appKey: e[0], ...e[1]}))
    .sort((a, b) => (b.accessedAt || 0) - (a.accessedAt || 0))

  return (
    <AutoHeadingScope>
      <SpaceBetween direction='vertical' narrow grow>
        <SpaceBetween between centered>
          <div className={classes.searchInput}>
            <SimpleTextInput
              grow
              id='app-search'
              iconStroke='search'
              value={appSearch}
              onChange={e => setAppSearch(e.target.value)}
              placeholder={t('project_list_page.search.placeholder.search_apps')}
              label={t('project_list_page.search.label.search_apps')}
            />
          </div>
          <SpaceBetween>
            <OpenProjectButton />
            <NewProjectButton />
          </SpaceBetween>
        </SpaceBetween>
        <ErrorMessage />
        {visibleProjects.length > 0 &&
          <ul className={combine('style-reset', classes.projectList)}>
            {visibleProjects.map(project => (
              <ProjectListItem
                key={project.appKey}
                project={project}
              />
            ))}
          </ul>}
        {visibleProjects.length === 0 &&
          <div className={classes.emptyState}>
            <SpaceBetween direction='vertical' centered>
              <AutoHeading>
                {t('project_list_page.text.no_projects_found')}
              </AutoHeading>
            </SpaceBetween>
          </div>
        }
      </SpaceBetween>
    </AutoHeadingScope>
  )
}

export {
  ProjectList,
}
