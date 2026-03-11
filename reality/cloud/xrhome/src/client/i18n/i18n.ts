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
