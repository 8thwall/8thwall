import React from 'react'
import {useQueryClient} from '@tanstack/react-query'
import {useTranslation} from 'react-i18next'
import {useHistory} from 'react-router-dom'

import {ApiFetchError, openDiskLocation} from '../studio/local-sync-api'
import {SecondaryButton} from '../ui/components/secondary-button'
import {getLocalStudioPath} from './desktop-paths'
import useActions from '../common/use-actions'
import appsActions from '../apps/apps-actions'

const OpenProjectButton = () => {
  const {t} = useTranslation(['studio-desktop-pages', 'common'])
  const queryClient = useQueryClient()
  const history = useHistory()
  const [loading, setLoading] = React.useState(false)
  const {error} = useActions(appsActions)

  const handleOpen = async () => {
    setLoading(true)
    try {
      const {appKey, initialization, canceled} = await openDiskLocation()
      if (canceled) {
        return
      } else {
        queryClient.invalidateQueries({queryKey: ['listProjects']})
        if (initialization === 'v2') {
          history.push(getLocalStudioPath(appKey))
        }
      }
    } catch (err: any) {
      if ((await (err as ApiFetchError)?.res?.json())?.containsPackageJsonAndReadme) {
        error(t('project_list_page.error.non_studio_project', {ns: 'studio-desktop-pages'}))
      } else {
        error(t('project_list_page.error.invalid_open_location', {ns: 'studio-desktop-pages'}))
      }
    } finally {
      setLoading(false)
    }
  }

  return (
    <SecondaryButton onClick={handleOpen} disabled={loading}>
      {t('button.open', {ns: 'common'})}
    </SecondaryButton>
  )
}

export {
  OpenProjectButton,
}
