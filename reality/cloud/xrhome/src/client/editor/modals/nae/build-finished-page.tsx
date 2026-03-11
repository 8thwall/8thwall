import React, {Dispatch, SetStateAction} from 'react'
import {useTranslation} from 'react-i18next'
import {useDispatch} from 'react-redux'

import type {HtmlShell} from '../../../../shared/nae/nae-types'
import {getDownloadedFileName} from '../../../../shared/nae/nae-utils'
import useActions from '../../../common/use-actions'
import useCurrentApp from '../../../common/use-current-app'
import naeActions from '../../../studio/actions/nae-actions'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {IconPreview} from '../../../apps/widgets/icon-preview'
import {PublishTipBanner} from '../../publishing/publish-tip-banner'
import {formatBytesToText} from '../../../../shared/asset-size-limits'
import {usePublishingStateContext} from '../../publishing/publish-context'
import {downloadBuild, getBuildModeName, getDateAndHourText, getEnvName} from './utils'
import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import {useNaeStyles} from './nae-styles'
import type {Steps} from './export-flow'

interface IBuildFinishedPage {
  platform: HtmlShell
  exportDisabled: boolean
  iconPreview?: string
  setCurrentStep: Dispatch<SetStateAction<Steps>>
}

const BuildFinishedPage: React.FC<IBuildFinishedPage> = ({
  platform,
  exportDisabled,
  iconPreview,
  setCurrentStep,
}) => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useNaeStyles()
  const app = useCurrentApp()
  const naeInfo = React.useMemo(
    // @ts-expect-error TODO(christoph): Clean up
    () => app.NaeInfos?.find(info => info.platform === platform.toUpperCase()),
    // @ts-expect-error TODO(christoph): Clean up
    [app.NaeInfos, platform]
  )
  const {htmlShellToNaeBuildData, setHtmlShellToNaeBuildData} = usePublishingStateContext()
  const naeBuildData = htmlShellToNaeBuildData[platform]
  const {signedUrlForNaeBuild, uploadToAppStoreConnect} = useActions(naeActions)
  const dispatch = useDispatch()
  const {
    exportType: buildExportType,
    buildId,
    buildEndTimestampMs,
    sizeBytes,
    buildMode,
    refHead,
    versionName: versionNameFromBuild,
  } = naeBuildData?.buildNotification ?? {}
  const {dateText, hoursText} = getDateAndHourText(buildEndTimestampMs)

  const handleDownload = async (downloadExportType: string, downloadBuildId: string) => {
    if (!downloadExportType || !downloadBuildId) {
      // eslint-disable-next-line no-console
      console.error('Missing required parameters for download - this should not happen.')
      return
    }

    // Get the signed URL for the build.
    const {signedUrl} = await signedUrlForNaeBuild({
      appUuid: app.uuid,
      buildId: downloadBuildId,
      appExtension: downloadExportType,
    })

    if (!signedUrl || signedUrl.includes('aws.amazon.com/s3')) {
      dispatch({
        type: 'ERROR',
        msg: t('editor_page.error.download_url_failed'),
      })
      return
    }

    downloadBuild(signedUrl)
  }

  const handleUploadToAppStore = async () => {
    await uploadToAppStoreConnect(
      app.uuid,
      buildId
    )
  }

  const showBuildMode = platform !== 'html'
  const showVersionName = platform !== 'html'
  return (
    <PublishPageWrapper
      headline={t('editor_page.native_publish_modal.publish_headline_complete')}
      headlineType='mobile'
      onBack={() => {
        if (exportDisabled) {
          setCurrentStep('start')
        } else {
          setCurrentStep('configure')
        }
        setHtmlShellToNaeBuildData(platform, {
          naeBuildStatus: 'noBuild',
          buildRequest: null,
          buildNotification: null,
        })
      }}
    >
      <div className={classes.downloadPage}>
        <div className={classes.appCompletedPane}>
          <div className={classes.downloadAppContainer}>
            <div className={classes.downloadPageAppIcon}>
              {iconPreview && <IconPreview
                className={classes.appIconImg}
                src={iconPreview}
                shape='square'
              />}
              <div className={classes.downloadPageApp}>
                <span className={classes.downloadPageAppDisplayName}>
                  {naeInfo?.displayName || app.appName}
                </span>
                <span className={classes.downloadPageFileName}>
                  {getDownloadedFileName(app.appName, buildExportType)}
                </span>
              </div>
            </div>
            <div className={classes.downloadPageMetadata}>
              <div className={classes.downloadPageMetadataText}>
                <span>
                  {`${t('editor_page.export_modal.download_page_metadata.size')}: ${
                    formatBytesToText(sizeBytes)
                  }`}
                </span>
                <span>
                  {`${t('editor_page.export_modal.download_page_metadata.date')}: ${
                    dateText
                  }`}
                </span>
                <span>
                  {`${t('editor_page.export_modal.download_page_metadata.time')}: ${
                    hoursText
                  }`}
                </span>
                {showBuildMode && (
                  <span>
                    {`${t('editor_page.export_modal.build_mode')}: ${
                      getBuildModeName(t, buildMode)
                    }`}
                  </span>
                )}
                <span>
                  {`${t('editor_page.export_modal.build_env')}: ${
                    getEnvName(t, refHead)
                  }`}
                </span>
                {showVersionName && (
                  <span>
                    {`${t('editor_page.export_modal.version_name')}: ${
                      versionNameFromBuild
                    }`}
                  </span>
                )}
              </div>
            </div>
          </div>
          {platform === 'ios' && BuildIf.NAE_IOS_APP_CONNECT_API_20250822 && (
            <PrimaryButton
              // eslint-disable-next-line local-rules/ui-component-styling
              className={classes.buildDownloadButton}
              height='small'
              onClick={handleUploadToAppStore}
              a8='click;cloud-editor-export-flow;send-to-app-store'
            >
              {t('editor_page.native_publish_modal.send_to_app_store')}
            </PrimaryButton>
          )}
          <PrimaryButton
          // eslint-disable-next-line local-rules/ui-component-styling
            className={classes.buildDownloadButton}
            height='small'
            onClick={() => {
              if (naeBuildData.naeBuildStatus !== 'success') {
                return
              }
              handleDownload(buildExportType, buildId)
            }}
            a8='click;cloud-editor-export-flow;download-build'
          >
            {t('editor_page.export_modal.download_build')}
          </PrimaryButton>
        </div>
        <div className={classes.downloadPageBanners}>
          {platform === 'android' && (
            <>
              <PublishTipBanner
                url='https://8th.io/nae-docs#installing-directly-on-an-android-device'
                title={t('editor_page.native_publish_modal.test_on_your_device_android.title')}
                content={
              t('editor_page.native_publish_modal.test_on_your_device_android.description')
            }
                iconStroke='phone'
                showLearnMore
              />
              <PublishTipBanner
                url='https://8th.io/nae-docs#publishing-to-the-google-play-store'
                title={t('editor_page.native_publish_modal.publish_google_play.title')}
                content={t('editor_page.native_publish_modal.publish_google_play.description')}
                iconStroke='googlePlay'
                showLearnMore
              />
            </>
          )}
        </div>
      </div>
    </PublishPageWrapper>
  )
}

export {BuildFinishedPage}
