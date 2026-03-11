import React from 'react'
import {useTranslation} from 'react-i18next'

import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import {useNaeStyles} from './nae-styles'
import CopyableCodeBlock from '../../../widgets/copyable-code-block'
import {RowJointToggleButton} from '../../../studio/configuration/row-joint-toggle-button'
import {useStyles as useNaeModalsStyles} from './nae-modals-styles'
import {EmbedType, getEmbedTypeOptions} from './utils'
import {IFRAME_AVAILABLE_PERMISSIONS} from '../../../../shared/nae/nae-constants'
import {SupportedPlatforms} from './supported-platforms'
import {RowTextField} from '../../../studio/configuration/row-text-field'

/* eslint-disable local-rules/hardcoded-copy */
const getIframeCode = (url: string, permissions: string[]) => `<iframe
  src="${url}"
  allowfullscreen
  allow="${permissions.join('; ')}"
></iframe>`

const getFullHtmlCode = (appName: string, url: string, permissions: string[]) => `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>${appName}</title>
    <style>
        html, body {
            margin: 0;
            padding: 0;
            height: 100%;
            overflow: hidden;
        }
        iframe {
            width: 100%;
            height: 100%;
            border: none;
        }
    </style>
</head>
<body>
${getIframeCode(url, permissions)}
</body>
</html>`
/* eslint-enable local-rules/hardcoded-copy */

const ExportFlowIframe: React.FC = () => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useNaeStyles()
  const naeModalsClasses = useNaeModalsStyles()

  const [productionUrl, setProductionUrl] = React.useState<string>('')
  const [embedType, setEmbedType] = React.useState<EmbedType>('iframe')

  return (
    <PublishPageWrapper
      headline={t('editor_page.native_publish_modal.publish_headline_first.iframe')}
      headlineType='web'
    >
      <div className={classes.rightCol}>
        <div className={naeModalsClasses.inputGroup}>
          <RowTextField
            label={t('editor_page.publish_modal.iframe.url')}
            value={productionUrl}
            placeholder='https://my-app.example'
            onChange={e => setProductionUrl(e.target.value)}
          />
          <RowJointToggleButton
            id='embed_type'
            label={t('editor_page.export_modal.iframe.embed_type')}
            options={getEmbedTypeOptions(t)}
            onChange={(selected) => { setEmbedType(selected) }}
            value={embedType}
          />
        </div>

        <CopyableCodeBlock
          code={embedType === 'iframe'
            ? getIframeCode(productionUrl, IFRAME_AVAILABLE_PERMISSIONS)
            // eslint-disable-next-line local-rules/hardcoded-copy
            : getFullHtmlCode('My App',
              productionUrl, IFRAME_AVAILABLE_PERMISSIONS)
            }
          downloadFileName={embedType === 'full-html' ? 'index.html' : undefined}
        />

        <SupportedPlatforms
          platforms={[
            /* eslint-disable local-rules/hardcoded-copy */
            {name: 'Itch.io', url: 'https://8th.io/nae-embed-itchio'},
            {name: 'Viverse', url: 'https://8th.io/nae-embed-viverse'},
            {name: 'Game Jolt', url: 'https://8th.io/nae-embed-game-jolt'},
            {name: 'GamePix', url: 'https://8th.io/nae-embed-gamepix'},
            {name: 'Newgrounds', url: 'https://8th.io/nae-embed-newgrounds'},
            {name: 'Y8', url: 'https://8th.io/nae-embed-y8'},
            {name: 'Poki', url: 'https://8th.io/nae-embed-poki'},
            {name: 'Kongregate', url: 'https://8th.io/nae-embed-kongregate'},
            {name: 'Armor Games', url: 'https://8th.io/nae-embed-armor-games'},
            {name: 'Addicting Games', url: 'https://8th.io/nae-embed-addicting-games'},
            {name: 'Lagged', url: 'https://8th.io/nae-embed-lagged'},
            /* eslint-enable local-rules/hardcoded-copy */
          ]}
        />
      </div>
    </PublishPageWrapper>
  )
}

export {ExportFlowIframe}
