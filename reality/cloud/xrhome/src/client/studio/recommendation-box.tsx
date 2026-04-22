import React from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'
import {queryOptions, useQuery, useQueryClient} from '@tanstack/react-query'

import {useSceneContext} from './scene-context'
import {useCurrentGit} from '../git/hooks/use-current-git'
import {isAssetPath} from '../common/editor-files'
import {fileExt} from '../editor/editor-common'
import {StaticBanner} from '../ui/components/banner'
import {BoldButton} from '../ui/components/bold-button'
import {replaceAssetInScene} from './replace-asset'
import {SpaceBetween} from '../ui/layout/space-between'
import {StandardModal} from '../ui/components/standard-modal'
import {StandardModalContent} from '../ui/components/standard-modal-content'
import {StandardModalActions} from '../ui/components/standard-modal-actions'
import {TertiaryButton} from '../ui/components/tertiary-button'
import {StandardModalHeader} from '../ui/components/standard-modal-header'
import {
  applyProjectConfigFix, checkConfigStatus,
} from './local-sync-api'
import useCurrentApp from '../common/use-current-app'
import {useLocalSyncContext} from './local-sync-context'
import {MILLISECONDS_PER_HOUR} from '../../shared/time-utils'

const useStyles = createUseStyles({
  recommendationBox: {
    overflowX: 'auto',
    whiteSpace: 'pre',
  },
})

const AssetBundleRecommendation = () => {
  const git = useCurrentGit()
  const ctx = useSceneContext()
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const [replacements, setReplacements] = React.useState(() => {
    const suggestedReplacements: [string, string][] = []

    const sceneString = JSON.stringify(ctx.scene)

    Object.values(git.filesByPath)
      .filter(f => f.isDirectory && isAssetPath(f.filePath))
      .forEach((folder) => {
        const ext = fileExt(folder.filePath)
        if (ext !== 'font8' && ext !== 'gltf') {
          return
        }
        if (!sceneString.includes(JSON.stringify(folder.filePath))) {
          return
        }
        const mainFile = git.filesByPath[`${folder}/.main`]
        if (mainFile?.content) {
          suggestedReplacements.push([folder.filePath, `${folder.filePath}/${mainFile.content}`])
        } else {
          const child = git.childrenByPath[folder.filePath]?.find(c => fileExt(c) === ext)
          if (child) {
            suggestedReplacements.push([folder.filePath, child])
          }
        }
      })

    return suggestedReplacements
  })

  if (!replacements?.length) {
    return null
  }

  return (
    <StaticBanner type='warning'>
      <SpaceBetween direction='vertical'>
        {t('recommendation_box.banner_message')}
        <SpaceBetween>
          <StandardModal
            width='narrow'
            trigger={<BoldButton>{t('recommendation_box.review_button')}</BoldButton>}
          >{collapse => (
            <>
              <StandardModalHeader>
                <h2>{t('recommendation_box.modal_title')}</h2>
              </StandardModalHeader>
              <StandardModalContent>
                <div className={classes.recommendationBox}>
                  {replacements.map(([folder, replacement]) => (
                    <div key={folder}>
                      <span>{folder}</span> &rarr; <span>{replacement}</span>
                    </div>
                  ))}
                </div>
              </StandardModalContent>
              <StandardModalActions>
                <BoldButton onClick={collapse}>
                  {t('button.cancel', {ns: 'common'})}
                </BoldButton>
                <TertiaryButton
                  onClick={() => {
                    replacements.forEach(([folder, replacement]) => {
                      ctx.updateScene(scene => replaceAssetInScene(scene, folder, replacement))
                    })
                    setReplacements(null)
                  }}
                >
                  {t('recommendation_box.update_references')}
                </TertiaryButton>
              </StandardModalActions>
            </>
          )}
          </StandardModal>
          <BoldButton onClick={() => setReplacements(null)}>
            {t('recommendation_box.dismiss')}
          </BoldButton>
        </SpaceBetween>
      </SpaceBetween>
    </StaticBanner>
  )
}

const getProjectConfigStatusQuery = (appKey: string) => queryOptions({
  queryKey: ['project-config', appKey],
  queryFn: () => checkConfigStatus(appKey),
  staleTime: MILLISECONDS_PER_HOUR,
})

const WebpackInjectFixRecommendation = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const {appKey} = useCurrentApp()
  const localSyncContext = useLocalSyncContext()
  const needsInjectFix = useQuery(getProjectConfigStatusQuery(appKey)).data?.needsInjectFix
  const queryClient = useQueryClient()
  const [dismissed, setDismissed] = React.useState(false)
  const visible = needsInjectFix && !dismissed

  if (!visible) {
    return null
  }

  return (
    <StaticBanner type='warning'>
      <SpaceBetween direction='vertical'>
        {t('recommendation_box.inject_message')}
        <SpaceBetween>
          <BoldButton onClick={async () => {
            await applyProjectConfigFix(appKey, 'inject')
            await localSyncContext.restartServer()
            queryClient.invalidateQueries(getProjectConfigStatusQuery(appKey))
          }}
          >
            {t('recommendation_box.fix')}
          </BoldButton>
          <BoldButton onClick={() => setDismissed(true)}>
            {t('recommendation_box.dismiss')}
          </BoldButton>
        </SpaceBetween>
      </SpaceBetween>
    </StaticBanner>
  )
}

const CopyPluginFixRecommendation = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const {appKey} = useCurrentApp()
  const localSyncContext = useLocalSyncContext()
  const needsCopyPluginFix = useQuery(getProjectConfigStatusQuery(appKey)).data?.needsCopyPluginFix
  const queryClient = useQueryClient()
  const [dismissed, setDismissed] = React.useState(false)
  const visible = needsCopyPluginFix && !dismissed

  if (!visible) {
    return null
  }

  return (
    <StaticBanner type='warning'>
      <SpaceBetween direction='vertical'>
        {t('recommendation_box.copy_plugin_message')}
        <SpaceBetween>
          <BoldButton onClick={async () => {
            await applyProjectConfigFix(appKey, 'copy-plugin')
            await localSyncContext.restartServer()
            queryClient.invalidateQueries(getProjectConfigStatusQuery(appKey))
          }}
          >
            {t('recommendation_box.fix')}
          </BoldButton>
          <BoldButton onClick={() => setDismissed(true)}>
            {t('recommendation_box.dismiss')}
          </BoldButton>
        </SpaceBetween>
      </SpaceBetween>
    </StaticBanner>
  )
}

const RecommendationBox = () => (
  <>
    <AssetBundleRecommendation />
    <WebpackInjectFixRecommendation />
    <CopyPluginFixRecommendation />
  </>
)

export {RecommendationBox}
