import i18n from 'i18next'
import {initReactI18next} from 'react-i18next'

import LazyImportPlugin from './lazy-import-plugin'
import I18nMigrationDebugPlugin from './i18n-migration-debug-plugin'
import BrokenKeyAlertPlugin from './broken-key-alert-plugin'
import {
  FALLBACK_LOCALE,
  getSupportedLocales8w,
} from '../../shared/i18n/i18n-locales'
import {chooseDefaultLanguage} from './choose-default-language'

// react-i18next v11 owns the resource types. Type-only imports preserve lazy loading.
declare module 'react-i18next' {
  interface CustomTypeOptions {
    resources: {
      'account-pages': typeof import('./en-US/account-pages.json')
      'app-pages': typeof import('./en-US/app-pages.json')
      'asset-lab': typeof import('./en-US/asset-lab.json')
      'caught-error-page': typeof import('./en-US/caught-error-page.json')
      'cloud-editor-pages': typeof import('./en-US/cloud-editor-pages.json')
      'cloud-studio-pages': typeof import('./en-US/cloud-studio-pages.json')
      common: typeof import('./en-US/common.json')
      'public-featured-pages': typeof import('./en-US/public-featured-pages.json')
      'studio-desktop-pages': typeof import('./en-US/studio-desktop-pages.json')
      'studio-tooltips': typeof import('./en-US/studio-tooltips.json')
    }
  }
}

const I18N_DEBUG = false

if (BuildIf.LOCAL) {
  i18n.use(BrokenKeyAlertPlugin)
}

i18n
  .use(initReactI18next)
  .use(LazyImportPlugin)
  .use(I18nMigrationDebugPlugin)
  .init({
    lng: chooseDefaultLanguage(),
    fallbackLng: FALLBACK_LOCALE,
    supportedLngs: getSupportedLocales8w(),
    ns: [],
    interpolation: {
      escapeValue: false,
    },
    postProcess: [
      ...(I18N_DEBUG ? ['i18nMigrationDebug'] : []),
      ...(BuildIf.LOCAL ? ['brokenKeyAlert'] : []),
    ],
  })
