import React from 'react'
import {useTranslation} from 'react-i18next'
import {useDispatch} from 'react-redux'

import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import {PublishTipBanner} from '../../publishing/publish-tip-banner'
import {useNaeStyles} from './nae-styles'
import {combine} from '../../../common/styles'
import useCurrentApp from '../../../common/use-current-app'
import type {Steps} from './utils'
import {BuildFinishedPage} from './build-finished-page'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {SupportedPlatforms} from './supported-platforms'
import {buildZip} from '../../../studio/local-sync-api'
import {downloadBlob} from '../../../common/download-utils'
import {useLogStreams} from '../../logs/use-log-streams'
import {SYSTEM_STREAM_NAME} from '../../logs/log-constants'
import CopyableBlock from '../../../widgets/copyable-block'
import {StaticBanner} from '../../../ui/components/banner'

interface IExportFlowHtml {
  onClose: () => void
}

const ExportFlowHtml: React.FC<IExportFlowHtml> = () => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useNaeStyles()
  const {appKey} = useCurrentApp()
  const dispatch = useDispatch()
  const [isBuilding, setIsBuilding] = React.useState(false)
  const [currentStep, setCurrentStep] = React.useState<Steps>('start')
  const [buildStartTime, setBuildStartTime] = React.useState(0)
  const [buildCompleteTime, setBuildCompleteTime] = React.useState(0)
  const [buildFailed, setBuildFailed] = React.useState(false)

  const systemLogs = useLogStreams(appKey).find(e => buildFailed && e.name === SYSTEM_STREAM_NAME)

  const buildLogs = systemLogs
    ? systemLogs.logs
      .filter(({timestamp}) => timestamp > buildStartTime &&
      (!buildCompleteTime || timestamp < buildCompleteTime))
      .map(e => e.text).join('\n')
    : ''

  const handleExport = async () => {
    dispatch({type: 'ERROR', msg: null})
    setIsBuilding(true)
    try {
      setBuildStartTime(Date.now())
      setBuildCompleteTime(0)
      setBuildFailed(false)
      const blob = await buildZip(appKey)
      const time = new Date().toISOString().replace(/[:.-]/g, '')
      downloadBlob(blob, `html-export-${time}.zip`)
    } catch (err) {
      setBuildFailed(true)
    } finally {
      setBuildCompleteTime(Date.now())
      setIsBuilding(false)
    }
  }

  const getPublishPageWrapperDisplayText = () => {
    if (isBuilding) {
      return (
        <span className={classes.displayText}>
          {t('editor_page.native_publish_modal.building_text')}
        </span>
      )
    }

    return null
  }

  if (currentStep === 'finished') {
    return (
      <BuildFinishedPage
        platform='html'
        exportDisabled={false}
        setCurrentStep={setCurrentStep}
      />
    )
  }

  return (
    <PublishPageWrapper
      headline={t('editor_page.native_publish_modal.publish_headline_first.html')}
      headlineType='web'
      showProgressBar={isBuilding}
      displayText={getPublishPageWrapperDisplayText()}
      actionButton={(
        <PrimaryButton
          onClick={handleExport}
          disabled={isBuilding}
        >
          {t('button.build', {ns: 'common'})}
        </PrimaryButton>
      )}
    >
      <div className={classes.rightCol}>
        {isBuilding && <div className={combine(classes.dimmer, classes.smallMonitorVisible)} />}

        <PublishTipBanner
          iconStroke='pointLight'
          url='https://8th.io/offline-html-export'
          content={t(
            'editor_page.export_modal.html_export.explanation'
          )}
          showLearnMore
        />

        <SupportedPlatforms
          platforms={[
            {
              name:
              t('editor_page.export_modal.html_export.supported_platforms.your_server'),
              url: 'https://8th.io/offline-html-export-your-server',
            },
            // eslint-disable-next-line local-rules/hardcoded-copy
            {name: 'Itch.io', url: 'https://8th.io/offline-html-export-itch'},
            // eslint-disable-next-line local-rules/hardcoded-copy
            {name: 'Discord', url: 'https://8th.io/offline-html-export-discord'},
            // eslint-disable-next-line local-rules/hardcoded-copy
            {name: 'Viverse', url: 'https://8th.io/offline-html-export-viverse'},
          ]}
        />

        {buildFailed &&
          <>
            <StaticBanner type='danger'>
              {t('editor_page.export_modal.html_export.build_error')}
            </StaticBanner>
            {buildLogs && <CopyableBlock text={buildLogs} description='' />}
          </>
        }
      </div>
    </PublishPageWrapper>
  )
}

export {ExportFlowHtml}
