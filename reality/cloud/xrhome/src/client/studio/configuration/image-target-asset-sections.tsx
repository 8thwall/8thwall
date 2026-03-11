import React, {useEffect} from 'react'
import {Trans, useTranslation} from 'react-i18next'

import type {IImageTarget} from '../../common/types/models'
import {createThemedStyles} from '../../ui/theme'
import {combine} from '../../common/styles'
import useActions from '../../common/use-actions'
import appsActions from '../../apps/apps-actions'
import {useSelector} from '../../hooks'
import {BasicQrCode} from '../../widgets/basic-qr-code'
import {Icon} from '../../ui/components/icon'
import {ImageTargetVisualizer, ImageTargetVisualizerFrame} from './image-target-visualizer'
import {ImageTargetModal} from '../image-target-modal'

type ImageTargetConfiguratorSection = 'configure' | 'test'

const useStyles = createThemedStyles(theme => ({
  sectionTitleContainer: {
    padding: '0.75em 1em 0.5em 1em',
    display: 'flex',
    fontWeight: 700,
    justifyContent: 'flex-start',
    gap: '1em',
    borderBottom: theme.studioSectionBorder,
    color: theme.fgMuted,
  },
  sectionTitle: {
    'cursor': 'pointer',
    'userSelect': 'none',
    '&:disabled': {
      cursor: 'default',
    },
  },
  sectionActive: {
    color: theme.fgMain,
  },
  testBody: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em',
    padding: '1em',
    flexGrow: 1,
    overflowY: 'scroll',
  },
  testInstructions: {
    color: theme.fgMuted,
  },
  qrCode: {
    margin: '0 3em',
    border: '0.5em solid white',
    background: 'white',
    borderRadius: '0.1em',
    width: '100px',
    height: '100px',
  },
  scanRow: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'center',
    gap: '0.25ch',
  },
  link: {
    color: theme.primaryBtnBg,
  },
  qrCodeContainer: {
    display: 'flex',
    justifyContent: 'center',
  },
}))

interface IImageTargetSections {
  section: ImageTargetConfiguratorSection
  setSection: (section: ImageTargetConfiguratorSection) => void
  saveChanges: () => Promise<boolean>
  hasChanges: boolean
  saveButtonDisabled: boolean
  saveButtonLoading: boolean
}

const ImageTargetSections: React.FC<IImageTargetSections> = ({
  section, setSection, saveChanges, hasChanges, saveButtonDisabled, saveButtonLoading,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()

  const testTargetButton = (
    <button
      type='button'
      onClick={hasChanges ? undefined : () => setSection('test')}
      className={combine('style-reset', classes.sectionTitle,
        section === 'test' && classes.sectionActive)}
      disabled={section === 'test'}
    >
      {t('asset_configurator.image_target_configurator.section.test')}
    </button>
  )

  const onUpdateClicked = async () => {
    const success = await saveChanges()
    if (success) {
      setSection('test')
    }
  }

  return (
    <div className={classes.sectionTitleContainer}>
      <button
        type='button'
        onClick={() => setSection('configure')}
        className={combine('style-reset', classes.sectionTitle,
          section === 'configure' && classes.sectionActive)}
        disabled={section === 'configure'}
      >
        {t('asset_configurator.image_target_configurator.section.configure')}
      </button>
      {hasChanges && (
        <ImageTargetModal
          trigger={testTargetButton}
          onSubmit={onUpdateClicked}
          heading={t('asset_configurator.image_target_configurator.test.modal.heading')}
          body={t('asset_configurator.image_target_configurator.test.modal.body')}
          submitLabel={t('asset_configurator.image_target_configurator.test.modal.update')}
          cancelLabel={t('asset_configurator.image_target_configurator.test.modal.cancel')}
          submitDisabled={saveButtonDisabled}
          submitLoading={saveButtonLoading}
        />
      )}
      {!hasChanges && testTargetButton}
    </div>
  )
}

interface IImageTargetTestSection {
  imageTarget: IImageTarget
}

const ImageTargetTestSection: React.FC<IImageTargetTestSection> = ({
  imageTarget,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()
  const imageTargetPreview = useSelector(s => s.common.imageTargetPreview)

  const {testImageTarget, testImageTargetClear} = useActions(appsActions)

  useEffect(() => {
    testImageTarget(imageTarget.AppUuid, imageTarget.uuid)
    return testImageTargetClear
  }, [])

  return (
    <div className={classes.testBody}>
      <ImageTargetVisualizerFrame>
        <ImageTargetVisualizer imageTarget={imageTarget} />
      </ImageTargetVisualizerFrame>
      <p className={classes.testInstructions}>
        {t('asset_configurator.image_target_configurator.test.instructions')}
      </p>
      {imageTargetPreview &&
        <>
          <div className={classes.qrCodeContainer}>
            <BasicQrCode
              className={classes.qrCode}
              url={imageTargetPreview}
              margin={0}
            />
          </div>
          <div className={classes.scanRow}>
            <div>
              <Trans
                t={t}
                i18nKey='asset_configurator.image_target_configurator.test.scan'
                components={[
                  <a href={imageTargetPreview} key='link' className={classes.link}> open link</a>,
                ]}
              />
            </div>
            <a href={imageTargetPreview} key='link'>
              <Icon inline stroke='external' color='muted' />
            </a>
          </div>
        </>
      }
    </div>
  )
}

export type {
  ImageTargetConfiguratorSection,
}

export {
  ImageTargetSections,
  ImageTargetTestSection,
}
