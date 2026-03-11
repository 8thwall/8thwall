import React from 'react'
import {useTranslation} from 'react-i18next'
import {useDispatch} from 'react-redux'

import {createNaeProjectUrl} from '../../../../shared/hosting-urls'
import type {
  ExportType,
  ScreenOrientation,
  HistoricalBuildData,
  HtmlShell,
  NaeBuilderRequest,
  AndroidClientConfig,
  AppleClientConfig,
  NaeBuildMode,
  RefHead,
  AppleSigningType,
  SigningFilesInfoResponse,
} from '../../../../shared/nae/nae-types'
import {
  getPreviewIconUrl,
  getRandomIcon,
  validateBuildMode,
  validateExportType,
  sanitizeBundleId,
  cleanupStringForBundleId,
} from '../../../../shared/nae/nae-utils'
import useActions from '../../../common/use-actions'
import useCurrentAccount from '../../../common/use-current-account'
import useCurrentApp from '../../../common/use-current-app'
import {useCurrentGit, useGitActiveClient} from '../../../git/hooks/use-current-git'
import {useCurrentRepoId} from '../../../git/repo-id-context'
import naeActions from '../../../studio/actions/nae-actions'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {useAbandonableEffect} from '../../../hooks/abandonable-effect'
import {TextNotification} from '../../../ui/components/text-notification'
import {
  RowTextField,
  RowSelectField,
  RowJointToggleButton,
} from '../../../studio/configuration/row-fields'
import type {JointToggleOption} from '../../../ui/components/joint-toggle-button'
import {Icon} from '../../../ui/components/icon'
import {PublishTipBanner} from '../../publishing/publish-tip-banner'
import {isActiveCommercialApp} from '../../../../shared/app-utils'
import {StandardFieldLabel} from '../../../ui/components/standard-field-label'
import {LearnMoreText} from '../../publishing/learn-more-text'
import {UNSUPPORTED_FEATURES_ANDROID, UNSUPPORTED_FEATURES_IOS} from './features'
import {UploadPane} from './upload-pane'
import {usePublishingStateContext} from '../../publishing/publish-context'
import {useStyles as useNaeModalsStyles} from './nae-modals-styles'
import {
  getBuildModeOptions,
  getAppleSigningTypeOptions,
  getScreenOrientationOptions,
  validateScreenOrientation,
  getExportDisabled,
} from './utils'
import {validateAppDisplayName, validateBundleId, validateVersionName} from './validation'
import {useSceneContext} from '../../../studio/scene-context'
import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import {combine} from '../../../common/styles'
import {PermissionsTable} from './permissions-table'
import {IosSigningPage} from './ios-signing-page'
import {SecondaryButton} from '../../../ui/components/secondary-button'
import {useNaeStyles} from './nae-styles'
import {BuildButton} from './build-button'
import {DebugShellVersion, useDebugShellVersion} from './debug-shell-version'
import {getBuildEnvOptions} from './build-env-options'
import {BuildFinishedPage} from './build-finished-page'
import type {Steps} from './utils'

const getDefaultExportType = (platform: HtmlShell): ExportType => {
  switch (platform) {
    case 'ios':
      return 'ipa'
    case 'android':
      return 'apk'
    default:
      return 'apk'
  }
}

interface IExportFlow {
  onClose: () => void
  platform: HtmlShell
}

const ExportFlow: React.FC<IExportFlow> = ({onClose, platform}) => {
  const naeModalsStyles = useNaeModalsStyles()
  const classes = useNaeStyles()
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const app = useCurrentApp()
  const {htmlShellToNaeBuildData, setHtmlShellToNaeBuildData} = usePublishingStateContext()
  const naeBuildData = htmlShellToNaeBuildData[platform]
  const currentNaeBuilderRequest = naeBuildData.buildRequest
  const repoId = useCurrentRepoId()
  const {name: activeClient} = useGitActiveClient()
  const account = useCurrentAccount()
  const git = useCurrentGit()
  const dispatch = useDispatch()
  const {postNaeBuild, updateNaeInfo, getRecentNaeBuilds, signingFilesInfo} =
    useActions(naeActions)
  const {shellVersion, removeExistingShellVersion, setShellVersion, setRemoveExistingShellVersion} =
    useDebugShellVersion()

  const naeInfo = React.useMemo(
    // @ts-expect-error TODO(christoph): Clean up
    () => app.NaeInfos?.find(info => info.platform === platform.toUpperCase()),
    // @ts-expect-error TODO(christoph): Clean up
    [app.NaeInfos, platform]
  )

  const getDefaultDisplayAppName = () => naeInfo?.displayName || app.appName

  const getDefaultBundleId = () => {
    const bundleId = naeInfo?.bundleId || `com.${cleanupStringForBundleId(platform,
      account.shortName)}.${
      cleanupStringForBundleId(platform, getDefaultDisplayAppName() || app.uuid)}`
    return sanitizeBundleId(platform, bundleId)
  }

  const randomIconId = React.useMemo(() => getRandomIcon(), [])

  const getDefaultIconId = () => naeInfo?.iconId || randomIconId

  const activeRef: RefHead = `${git.repo.handle}-${activeClient}`

  const [appDisplayName, setAppDisplayName] = React.useState(getDefaultDisplayAppName())
  const [bundleId, setBundleId] = React.useState(getDefaultBundleId())
  const [versionName, setVersionName] = React.useState('')
  const [exportType, setExportType] = React.useState<ExportType>(getDefaultExportType(platform))
  const [screenOrientation, setScreenOrientation] = React.useState<ScreenOrientation>('portrait')
  const [statusBarVisible, setStatusBarVisible] = React.useState<boolean>(platform === 'ios')
  const [naeBuildMode, setNaeBuildMode] = React.useState<NaeBuildMode>('hot-reload')
  const [appleSigningType, setAppleSigningType] = React.useState<AppleSigningType>('development')
  const [ref, setRef] = React.useState<RefHead>(activeRef)
  const [existingSigningInfo, setExistingSigningInfo] = React.useState<SigningFilesInfoResponse>({})

  const [iconFile, setIconFile] = React.useState<File>(null)
  const [iconPreview, setIconPreview] = React.useState(getPreviewIconUrl(getDefaultIconId()))

  const [launchScreenFile, setLaunchScreenFile] = React.useState<File>(null)
  const [launchScreenPreview, setLaunchScreenPreview] = React.useState('')

  const [recentNaeBuilds, setRecentNaeBuilds] = React.useState<HistoricalBuildData[]>([])

  const [currentStep, setCurrentStep] = React.useState<Steps>('start')

  const developmentSigningInfo = existingSigningInfo?.developmentSigningInfo?.certificateUuid
  const distributionSigningInfo = existingSigningInfo?.distributionSigningInfo?.certificateUuid

  const sceneCtx = useSceneContext()
  const exportDisabled = React.useMemo(
    () => getExportDisabled(sceneCtx, platform),
    [sceneCtx.scene.objects]
  )

  const buildEnvOptions = getBuildEnvOptions(t, git, activeRef, classes)

  const buildModeOptions = getBuildModeOptions(t)

  const appleSigningTypeOptions = getAppleSigningTypeOptions(t).map(option => ({
    ...option,
    disabled: option.value === 'development'
      ? !developmentSigningInfo
      : !distributionSigningInfo,
  })) as [
    JointToggleOption<AppleSigningType>,
    JointToggleOption<AppleSigningType>,
    ...JointToggleOption<AppleSigningType>[]
  ]

  useAbandonableEffect(async () => {
    if (platform === 'ios') {
      const [signingResponse] = await Promise.all([
        signingFilesInfo(app.uuid),
      ])

      if (signingResponse) {
        setExistingSigningInfo(signingResponse)
      }
    }
  }, [app.uuid, signingFilesInfo])

  React.useEffect(() => {
    switch (naeBuildData.naeBuildStatus) {
      case 'success':
        setCurrentStep('finished')
        break
      case 'failed':
        setHtmlShellToNaeBuildData(platform, {
          naeBuildStatus: 'noBuild',
          buildRequest: null,
          buildNotification: null,
        })

        setCurrentStep('configure')
        break
      default:
        break
    }
  }, [naeBuildData.naeBuildStatus])

  const handleContinueInStart = async (step: Steps) => {
    setCurrentStep(step)
    await updateNaeInfo({
      platform,
      appUuid: app.uuid,
      displayName: appDisplayName,
      bundleId,
      file: iconFile,
      launchScreenFile,
    })
  }

  const handleExport = async () => {
    setHtmlShellToNaeBuildData(platform, {
      naeBuildStatus: 'building',
      buildRequest: null,
      buildNotification: null,
    })
    dispatch({type: 'ERROR', msg: null})

    // NOTE: We want to complete the NaeInfo update before starting the build, so that if the user
    // updated their app icon, it is uploaded before the build starts.
    const {naeInfo: updatedNaeInfo} = await updateNaeInfo({
      platform,
      appUuid: app.uuid,
      lastBuiltVersionName: versionName,
      file: iconFile,
      launchScreenFile,
    })

    const androidClientConfig: AndroidClientConfig = {
      packageName: bundleId,
      versionName,
    }
    const appleClientConfig: AppleClientConfig = {
      // Note(lreyna): During the build process, the server replaces this placeholder with the
      // actual team identifier from the user's Apple signing configuration.
      teamIdentifier: 'PLACEHOLDER',
      bundleIdentifier: bundleId,
      versionName,
      signingType: appleSigningType,
    }

    const naeBuilderRequest: NaeBuilderRequest = {
      // We add this b/c we want the type to have it, but we let the server fill it.
      requestUuid: '',
      shellVersion,
      removeExistingShellVersion,
      projectUrl: createNaeProjectUrl(account.shortName, app.appName, ref),
      shell: platform,
      ...(platform === 'android' ? {android: androidClientConfig} : {}),
      ...(platform === 'ios' ? {apple: appleClientConfig} : {}),
      appInfo: {
        appDisplayName,
        refHead: ref,
        workspace: account.shortName,
        appName: app.appName,
        screenOrientation,
        statusBarVisible,
        naeBuildMode,
        bundleIdentifier: bundleId,
        versionName,
        resourceDir: '',
        ...(platform === 'ios'
          ? {
            permissions: naeInfo.permissions,
          }
          : {}),
      },
      appUuid: app.uuid,
      accountUuid: account.uuid,
      buildRequestTimestampMs: Date.now(),
      exportType,
      iconId: updatedNaeInfo.iconId,
      ...(platform === 'ios' ? {launchScreenIconId: updatedNaeInfo.launchScreenIconId} : {}),
      requestRef: activeRef,
      repoId,
    }
    const result = postNaeBuild(naeBuilderRequest)
    if (!result) {
      setHtmlShellToNaeBuildData(platform, {
        naeBuildStatus: 'noBuild',
        buildRequest: null,
        buildNotification: null,
      })
    } else {
      setHtmlShellToNaeBuildData(platform, {
        naeBuildStatus: 'building',
        buildRequest: naeBuilderRequest,
        buildNotification: null,
      })
    }
  }

  const getDefaultVersionName = () => {
    if (naeInfo?.lastBuiltVersionName) {
      return naeInfo.lastBuiltVersionName
    }
    return '1.0.0'
  }

  React.useEffect(() => {
    setAppDisplayName(getDefaultDisplayAppName())
    setBundleId(getDefaultBundleId())
    setVersionName(getDefaultVersionName())
  }, [naeInfo])

  useAbandonableEffect(async (executor) => {
    const iconId = getDefaultIconId()

    if (!naeInfo?.iconId) {
      // When the app is created, it will not have an iconId yet, so we set a default one. Here we
      // update the DB with this random iconId.
      await executor(updateNaeInfo({platform, appUuid: app.uuid, iconId}))
    }
    const previewAppIcon = getPreviewIconUrl(iconId)
    setIconPreview(previewAppIcon)
    // TODO(xiaokai): only default launch screen preview to app icon if it's not set in naeInfo
    if (platform === 'ios' && BuildIf.NAE_LAUNCH_SCREEN_UPLOAD_20250910) {
      if (!naeInfo?.launchScreenIconId) {
        await executor(updateNaeInfo({platform, appUuid: app.uuid, launchScreenIconId: iconId}))
      }
      const previewLaunchScreenIcon = getPreviewIconUrl(naeInfo?.launchScreenIconId)
      setLaunchScreenPreview(previewLaunchScreenIcon)
    }
  }, [naeInfo?.iconId, naeInfo?.launchScreenIconId])

  React.useEffect(() => {
    // If we started building, minimized, and then opened again, we need to set the state to the
    // currently building app configuration.
    if (currentNaeBuilderRequest) {
      setExportType(currentNaeBuilderRequest.exportType)
      setNaeBuildMode(currentNaeBuilderRequest.appInfo.naeBuildMode)
      setRef(currentNaeBuilderRequest.appInfo.refHead)
      // Even though this is set in app state, we want to make sure that when the user sees the
      // build progress UI, they see the correct version name for that build.
      setVersionName(currentNaeBuilderRequest.appInfo.versionName)
      return
    }

    // Otherwise, we should load this data based on the most recent historical builds. This is a
    // design decision we made rather than storing these fields in the app state.
    const mostRecentBuild = recentNaeBuilds?.[0]
    if (!mostRecentBuild) {
      return
    }

    if (validateExportType(mostRecentBuild.exportType)) {
      setExportType(mostRecentBuild.exportType)
    }

    if (validateBuildMode(mostRecentBuild.buildMode)) {
      setNaeBuildMode(mostRecentBuild.buildMode)
    }

    if (validateScreenOrientation(mostRecentBuild.screenOrientation)) {
      setScreenOrientation(mostRecentBuild.screenOrientation)
    }

    setRef(activeRef)
  }, [recentNaeBuilds])

  useAbandonableEffect(async (executor) => {
    const res = await executor(getRecentNaeBuilds(app.uuid, platform))
    setRecentNaeBuilds(res?.recentNaeBuilds || [])
  }, [app.uuid])

  const appDisplayNameValidationResult = validateAppDisplayName(appDisplayName, platform, t)
  const bundleIdValidationResult = validateBundleId(bundleId, platform, t)
  const versionNameValidationResult = validateVersionName(versionName, t)
  const hasIosSigningInfo = platform !== 'ios' ||
    // <Foo>SigningInfo will be undefined if there are no available certificates for that signing
    // type. But a certificate was uploaded for the account, but the user has not uploaded a
    // provisioning profile for the app, it will be defined. So here we check for certificateUuid,
    // which is only defined if a certificate and a provisioning profile were uploaded.
    (developmentSigningInfo || distributionSigningInfo)

  const continueDisabled = exportDisabled ||
    !appDisplayNameValidationResult.success ||
    !bundleIdValidationResult.success ||
    !hasIosSigningInfo

  const isBuilding = naeBuildData.naeBuildStatus === 'building'

  const publishHeadlineFirstKey =
    `editor_page.native_publish_modal.publish_headline_first.${platform}`

  if (currentStep === 'start' && !isBuilding) {
    return (
      <PublishPageWrapper
        headline={t(publishHeadlineFirstKey)}
        headlineType='mobile'
        displayText={exportDisabled &&
          <div className={classes.iconAndTextContainer}>
            <Icon stroke='info' color='danger' size={0.75} />
            <span className={combine(classes.displayText, classes.error)}>
              {t('editor_page.native_publish_modal.export_disabled_text')}
            </span>
          </div>}
        actionButton={(
          <PrimaryButton
            height='small'
            // eslint-disable-next-line local-rules/ui-component-styling
            className={classes.continueButton}
            disabled={continueDisabled}
            onClick={() => handleContinueInStart('configure')}
            a8='click;cloud-editor-export-flow;continue'
          >
            {t('button.continue', {ns: 'common'})}
          </PrimaryButton>
        )}
        actionButtonTooltip={!hasIosSigningInfo &&
          t('editor_page.export_modal.ios_signing.complete_apple_configuration')
        }
      >
        <div className={classes.columnsContent}>
          <div className={classes.leftCol}>
            <UploadPane
              uploadType='app-icon'
              appDisplayName={appDisplayName}
              iconPreview={iconPreview}
              setIconPreview={setIconPreview}
              setIconFile={setIconFile}
              platform={platform}
            />
            {platform === 'ios' && BuildIf.NAE_LAUNCH_SCREEN_UPLOAD_20250910 && (
              <UploadPane
                uploadType='launch-screen'
                iconPreview={launchScreenPreview}
                setIconPreview={setLaunchScreenPreview}
                setIconFile={setLaunchScreenFile}
                platform={platform}
              />
            )}
          </div>
          <div className={classes.rightCol}>
            <div className={classes.smallMonitorView}>
              <UploadPane
                uploadType='app-icon'
                appDisplayName={appDisplayName}
                iconPreview={iconPreview}
                setIconPreview={setIconPreview}
                setIconFile={setIconFile}
                platform={platform}
              />
              {platform === 'ios' && BuildIf.NAE_LAUNCH_SCREEN_UPLOAD_20250910 && (
                <UploadPane
                  uploadType='launch-screen'
                  iconPreview={launchScreenPreview}
                  setIconPreview={setLaunchScreenPreview}
                  setIconFile={setLaunchScreenFile}
                  platform={platform}
                />
              )}
            </div>
            <div className={classes.inputFields}>
              <div className={naeModalsStyles.inputGroup}>
                <RowTextField
                  id='appDisplayName'
                  label={t('editor_page.export_modal.app_display_name')}
                  placeholder={t('editor_page.export_modal.app_display_name_placeholder')}
                  value={appDisplayName}
                  onChange={e => setAppDisplayName(e.target.value)}
                  invalid={!appDisplayNameValidationResult.success}
                  onBlur={() => {
                    if (appDisplayName === '') {
                      setAppDisplayName(app.appName)
                    }
                  }}
                />
                {appDisplayNameValidationResult.uiResult &&
                  <TextNotification type={appDisplayNameValidationResult.uiResult.status}>
                    {appDisplayNameValidationResult.uiResult.message}
                  </TextNotification>
                }
                {platform !== 'ios' && (
                  <>
                    <RowTextField
                      id='bundleId'
                      label={t('editor_page.export_modal.bundle_id')}
                      placeholder='com.company.app_name'
                      value={bundleId}
                      onChange={e => setBundleId(e.target.value)}
                      invalid={!bundleIdValidationResult.success}
                      onBlur={() => {
                        if (bundleId === '') {
                          setBundleId(getDefaultBundleId())
                        }
                      }}
                    />
                    {bundleIdValidationResult.uiResult &&
                      <TextNotification type={bundleIdValidationResult.uiResult.status}>
                        {bundleIdValidationResult.uiResult.message}
                      </TextNotification>
                    }
                  </>
                )}
              </div>
            </div>
            {platform === 'ios' && (
              <div>
                <div className={classes.rowLabel}>
                  <StandardFieldLabel label={t('editor_page.export_modal.ios_signing.title')} />
                </div>
                <PublishTipBanner
                  content={(
                    <>
                      {t('editor_page.export_modal.ios_signing.content')}
                      <div className={classes.justifyBetween}>
                        <SecondaryButton
                          // eslint-disable-next-line local-rules/ui-component-styling
                          className={combine(classes.alignCenter, classes.gap0375)}
                          height='tiny'
                          onClick={() => handleContinueInStart('configure-ios-signing')}
                          a8='click;cloud-editor-export-flow;configure-ios-signing'
                        >
                          {t('editor_page.export_modal.ios_signing.complete_apple_configurations')}
                        </SecondaryButton>
                        <a
                          className={combine(
                            classes.alignCenter,
                            classes.gap05,
                            classes.textMuted,
                            classes.hoverTextMain
                          )}
                          href='https://developer.apple.com/account/resources/certificates/list'
                          target='_blank'
                          rel='noopener noreferrer'
                        >
                          <Icon
                            stroke='external'
                            size={0.75}
                          />
                          {t('editor_page.export_modal.ios_signing.visit_apple_developer')}
                        </a>
                      </div>
                    </>
                  )}
                />
              </div>
            )}
            {
              platform !== 'android' && (
                <div>
                  <div className={classes.rowLabel}>
                    <StandardFieldLabel label={t('editor_page.export_modal.permissions')} />
                  </div>
                  <PublishTipBanner
                    content={(
                      <>
                        {t('editor_page.export_modal.permissions.content')}
                        <div className={classes.justifyBetween}>
                          <SecondaryButton
                          // eslint-disable-next-line local-rules/ui-component-styling
                            className={combine(classes.alignCenter, classes.gap0375)}
                            height='tiny'
                            onClick={() => handleContinueInStart('permissions')}
                            a8='click;cloud-editor-export-flow;permissions'
                          >
                            {t('editor_page.export_modal.permissions.configure_permissions')}
                          </SecondaryButton>
                        </div>
                      </>
                    )}
                  />
                </div>
              )
            }
            <div>
              <div className={classes.rowLabel}>
                <StandardFieldLabel label={t('editor_page.export_modal.splash_screen.title')} />
              </div>
              {
                isActiveCommercialApp(app)
                  ? (
                    <PublishTipBanner
                      content={t(
                        'editor_page.export_modal.white_label_subscription.subscribed_msg'
                      )}
                      iconStroke='crown'
                      iconColor='success'
                    />
                  )
                  : (
                    <PublishTipBanner
                      url='https://8th.io/nae-white-label'
                      content={t(
                        'editor_page.export_modal.white_label_subscription.not_subscribed_msg'
                      )}
                      iconStroke='crown'
                      iconColor='gray4'
                      showLearnMore
                    />
                  )
              }
            </div>
            <div>
              <div className={classes.rowLabel}>
                <StandardFieldLabel
                  label={t('editor_page.native_publish_modal.upcoming_features.title')}
                />
              </div>
              <PublishTipBanner
                url='https://8th.io/nae-docs#requirements'
                content={(
                  <div className={classes.unsupportedExportList}>
                    <div className={classes.unsupportedExportsIntro}>
                      {t('editor_page.native_publish_modal.upcoming_features.intro')}
                    </div>
                    <div className={classes.exportItemGrid}>
                      {(platform === 'android'
                        ? UNSUPPORTED_FEATURES_ANDROID
                        : UNSUPPORTED_FEATURES_IOS).map((item) => {
                        const unsupportedItemName = t(item)
                        return (
                          <div className={classes.exportItem} key={unsupportedItemName}>
                            <span className={classes.exportItemText}>
                              {unsupportedItemName}
                            </span>
                          </div>
                        )
                      })}
                    </div>
                    <div className={classes.unsupportedExportLearnMoreLinkContainer}>
                      <LearnMoreText
                        a8='click;cloud-editor-export-flow;unsupported-features-learn-more'
                      />
                    </div>
                  </div>
                )}
              />
            </div>
          </div>
        </div>
      </PublishPageWrapper>
    )
  } else if (currentStep === 'permissions') {
    return (
      <PermissionsTable
        platform={platform}
        setCurrentStep={setCurrentStep}
      />
    )
  } else if (currentStep === 'configure-ios-signing') {
    return (
      <IosSigningPage
        setCurrentStep={setCurrentStep}
        bundleId={bundleId}
        setBundleId={setBundleId}
        getDefaultBundleId={getDefaultBundleId}
        existingSigningInfo={existingSigningInfo}
        setExistingSigningInfo={setExistingSigningInfo}
      />
    )
  } else if (currentStep === 'configure' || isBuilding) {
    const configureHeadlineKey =
      `editor_page.native_publish_modal.publish_headline_configure.${platform}`
    return (
      <PublishPageWrapper
        headline={t(configureHeadlineKey)}
        headlineType='mobile'
        showProgressBar={isBuilding}
        displayText={isBuilding &&
          <span className={classes.displayText}>
            {t('editor_page.native_publish_modal.building_text')}
          </span>
          }
        onBack={!isBuilding && (() => setCurrentStep('start'))}
        actionButton={(
          <BuildButton
            isBuilding={isBuilding}
            buildDisabled={exportDisabled || !versionNameValidationResult.success}
            onClose={onClose}
            handleExport={handleExport}
          />
        )}
      >
        <div className={classes.columnsContent}>
          {isBuilding && <div className={combine(classes.dimmer, classes.smallMonitorHidden)} />}
          <div className={classes.leftCol}>
            <UploadPane
              uploadType='app-icon'
              appDisplayName={appDisplayName}
              iconPreview={iconPreview}
              setIconPreview={setIconPreview}
              setIconFile={setIconFile}
              platform={platform}
            />
            {platform === 'ios' && BuildIf.NAE_LAUNCH_SCREEN_UPLOAD_20250910 && (
              <UploadPane
                uploadType='launch-screen'
                iconPreview={launchScreenPreview}
                setIconPreview={setLaunchScreenPreview}
                setIconFile={setLaunchScreenFile}
                platform={platform}
              />
            )}
          </div>
          <div className={classes.rightCol}>
            {isBuilding && <div className={combine(classes.dimmer, classes.smallMonitorVisible)} />}
            <div className={classes.smallMonitorView}>
              <UploadPane
                uploadType='app-icon'
                appDisplayName={appDisplayName}
                iconPreview={iconPreview}
                setIconPreview={setIconPreview}
                setIconFile={setIconFile}
                platform={platform}
              />
              {platform === 'ios' && BuildIf.NAE_LAUNCH_SCREEN_UPLOAD_20250910 && (
                <UploadPane
                  uploadType='launch-screen'
                  iconPreview={launchScreenPreview}
                  setIconPreview={setLaunchScreenPreview}
                  setIconFile={setLaunchScreenFile}
                  platform={platform}
                />
              )}
            </div>
            <div className={naeModalsStyles.inputGroup}>
              <RowTextField
                id='versionNumber'
                label={t('editor_page.export_modal.version_name')}
                value={versionName}
                onChange={e => setVersionName(e.target.value)}
                invalid={!versionNameValidationResult.success}
                onBlur={() => {
                  if (versionName === '') {
                    setVersionName(getDefaultVersionName())
                  }
                }}
              />
              {versionNameValidationResult.uiResult &&
                <TextNotification type={versionNameValidationResult.uiResult.status}>
                  {versionNameValidationResult.uiResult.message}
                </TextNotification>
              }
            </div>
            <div className={naeModalsStyles.inputGroup}>
              <RowSelectField
                id='screenOrientation'
                label={t('editor_page.export_modal.screen_orientation')}
                menuWrapperClassName={classes.exportType}
                options={getScreenOrientationOptions(t)}
                value={screenOrientation}
                onChange={newValue => setScreenOrientation(newValue)}
                renderExpandedOption={((option) => {
                  const isSelected = option.value === screenOrientation
                  return (
                  // TODO(lreyna): Name the dropdown style generic
                    <div className={classes.exportTypeDropdown}>
                      <div className={classes.exportTypeContent}>
                        {option.content}
                      </div>
                      {isSelected &&
                        <div className={classes.checkmark}>
                          <Icon stroke='checkmark' color='highlight' block />
                        </div>}
                    </div>
                  )
                })}
              />
              <RowJointToggleButton
                id='statusBarVisible'
                label={t('editor_page.export_modal.status_bar_visible')}
                options={[
                  {
                    content: t('option.yes', {ns: 'common'}),
                    value: 'true',
                  },
                  {
                    content: t('option.no', {ns: 'common'}),
                    value: 'false',
                  },
                ]}
                onChange={(selected) => { setStatusBarVisible(selected === 'true') }}
                value={statusBarVisible ? 'true' : 'false'}
              />
            </div>
            <div className={naeModalsStyles.inputGroup}>
              {platform === 'android' &&
                <RowSelectField
                  id='androidExportType'
                  label={t('editor_page.export_modal.build_type')}
                  menuWrapperClassName={classes.exportType}
                  options={[
                    {value: 'apk', content: 'APK (Android Package)'},
                    {value: 'aab', content: 'AAB (Android App Bundle)'},
                  ]}
                  value={exportType}
                  onChange={newValue => setExportType(newValue)}
                  renderExpandedOption={((option) => {
                    const isSelected = option.value === exportType
                    return (
                      <div className={classes.exportTypeDropdown}>
                        <div className={classes.exportTypeContent}>
                          {option.content}
                          <span className={classes.exportTypeDropdownDescription}>
                            {t(
                              `editor_page.export_modal.build_type.${
                                option.value
                              }_description`
                            )}
                          </span>
                        </div>
                        {isSelected &&
                          <div className={classes.checkmark}>
                            <Icon stroke='checkmark' color='highlight' block />
                          </div>}
                      </div>
                    )
                  })}
                />
              }
              <RowJointToggleButton
                id='buildMode'
                label={t('editor_page.export_modal.build_mode')}
                options={buildModeOptions}
                onChange={(selected) => { setNaeBuildMode(selected) }}
                value={naeBuildMode}
              />
              <RowJointToggleButton
                id='buildEnv'
                label={t('editor_page.export_modal.build_env')}
                options={buildEnvOptions}
                onChange={(selected) => { setRef(selected) }}
                value={ref}
              />
            </div>
            {platform === 'ios' &&
              <div className={naeModalsStyles.inputGroup}>
                <RowJointToggleButton
                  id='AppleSigningType'
                  label={t('editor_page.export_modal.ios_signing_type')}
                  options={appleSigningTypeOptions}
                  onChange={(selected) => { setAppleSigningType(selected) }}
                  value={appleSigningType}
                />
              </div>
            }
            <DebugShellVersion
              shellVersion={shellVersion}
              setShellVersion={setShellVersion}
              removeExistingShellVersion={removeExistingShellVersion}
              setRemoveExistingShellVersion={setRemoveExistingShellVersion}
            />
            {!isBuilding && (
              <PublishTipBanner
                url='https://8th.io/nae-docs#finalizing-build-settings'
                content={(
                  <div className={classes.configurePageHelperText}>
                    {t('editor_page.native_publish_modal.configure_page.helper_text')}
                    <div className={combine(classes.learnMoreText, 'learn-more-text')}>
                      <Icon
                        stroke='external'
                        size={0.75}
                      />
                      {t('button.learn_more', {ns: 'common'})}
                    </div>
                  </div>
                )}
              />
            )}
          </div>
        </div>
      </PublishPageWrapper>
    )
  } else if (currentStep === 'finished') {
    return (
      <BuildFinishedPage
        platform={platform}
        exportDisabled={exportDisabled}
        iconPreview={iconPreview}
        setCurrentStep={setCurrentStep}
      />
    )
  } else {
    return <div />
  }
}

export {ExportFlow, Steps}
