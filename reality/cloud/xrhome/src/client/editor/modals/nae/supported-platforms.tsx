import React from 'react'
import {useTranslation} from 'react-i18next'

import {PublishTipBanner} from '../../publishing/publish-tip-banner'
import {useNaeStyles} from './nae-styles'
import {StandardFieldLabel} from '../../../ui/components/standard-field-label'
import {Icon} from '../../../ui/components/icon'
import {combine} from '../../../common/styles'

function SupportedPlatforms({platforms}: {platforms: {name: string, url: string}[]}) {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useNaeStyles()

  return (
    <div>
      <div className={classes.rowLabel}>
        <StandardFieldLabel
          label={t('editor_page.export_modal.html_export.supported_platforms.title')}
        />
      </div>
      <PublishTipBanner
        content={(
          <div className={classes.unsupportedExportList}>
            <div className={classes.unsupportedExportsIntro}>
              {t('editor_page.export_modal.html_export.supported_platforms.intro')}
            </div>
            <div className={classes.exportItemGrid}>
              {platforms.map(exportPlatform => (
                <div
                  key={exportPlatform.name}
                  className={classes.exportItem}
                >
                  <a
                    href={exportPlatform.url}
                    target='_blank'
                    rel='noopener noreferrer'
                    className={combine(
                      classes.alignCenter,
                      classes.gap05,
                      classes.textMuted,
                      classes.hoverTextMain
                    )}
                  >
                    <Icon stroke='external' size={0.75} />
                    {exportPlatform.name}
                  </a>
                </div>
              ))}
            </div>
          </div>
        )}
      />
    </div>
  )
}

export {SupportedPlatforms}
