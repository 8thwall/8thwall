import React from 'react'
import {useQueryClient} from '@tanstack/react-query'
import {useTranslation} from 'react-i18next'
import {useHistory} from 'react-router-dom'

import {ApiFetchError, openDiskLocation} from '../studio/local-sync-api'
import {SecondaryButton} from '../ui/components/secondary-button'
import {getLocalStudioPath} from './desktop-paths'
import useActions from '../common/use-actions'
import appsActions from '../apps/apps-actions'
import {ConfirmNonStudioModal, getAlreadyConfirmedNonStudio} from './confirm-non-studio-modal'
import {StandardModal} from '../ui/components/standard-modal'
import {NewProjectContent} from './new-project-modal'

const OpenProjectButton = () => {
  const {t} = useTranslation(['studio-desktop-pages', 'common'])
  const queryClient = useQueryClient()
  const history = useHistory()
  const [loading, setLoading] = React.useState(false)
  const {error} = useActions(appsActions)
  const [nonStudioLocation, setNonStudioLocation] = React.useState('')
  const [templateZipUrl, setTemplateZipUrl] = React.useState<string | null>(null)

  const handleOpen = async () => {
    setLoading(true)
    try {
      const result = await openDiskLocation({
        acceptNonStudio: getAlreadyConfirmedNonStudio(),
      })
      if (result.canceled) {
        return
      }
      if ('templateZipUrl' in result) {
        setTemplateZipUrl(result.templateZipUrl)
        return
      }
      queryClient.invalidateQueries({queryKey: ['listProjects']})
      if (result.initialization === 'v2') {
        history.push(getLocalStudioPath(result.appKey))
      }
    } catch (err: any) {
      const {containsPackageJson, location} = (await (err as ApiFetchError)?.res?.json())
      if (location && containsPackageJson) {
        setNonStudioLocation(location)
      } else {
        error(t('project_list_page.error.invalid_open_location', {ns: 'studio-desktop-pages'}))
      }
    } finally {
      setLoading(false)
    }
  }

  return (
    <>
      <SecondaryButton onClick={handleOpen} disabled={loading}>
        {t('button.open', {ns: 'common'})}
      </SecondaryButton>
      {templateZipUrl &&
        <StandardModal
          trigger='render'
          onOpenChange={(open) => {
            if (!open) {
              setTemplateZipUrl(null)
            }
          }}
        >
          {onClose => (
            <NewProjectContent
              onClose={() => {
                setTemplateZipUrl(null)
                onClose()
              }}
              templateZipUrl={templateZipUrl}
            />
          )}
        </StandardModal>
      }
      {nonStudioLocation &&
        <ConfirmNonStudioModal
          location={nonStudioLocation}
          onClose={() => setNonStudioLocation('')}
        />
      }
    </>
  )
}

export {
  OpenProjectButton,
}
