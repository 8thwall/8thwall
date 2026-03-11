import {assert} from 'chai'

import {
  getSupportedLocales8w,
  getSupportedLocale8wOptions,
  EXPORTS_FOR_TESTS,
} from '../src/shared/i18n/i18n-locales'
import {disableFlag, enableFlag} from './buildif-mock'

const {PROD_SUPPORTED_LOCALES_8W, DEV_SUPPORTED_LOCALES_8W} = EXPORTS_FOR_TESTS

describe('8w i18n Locales - Prod and Dev Locales Test', () => {
  describe('when BuildIf.EXPERIMENTAL is disabled', () => {
    beforeEach(() => {
      disableFlag('EXPERIMENTAL')
    })

    it('should be using the production locales', () => {
      const locales = getSupportedLocales8w()
      assert.sameOrderedMembers(locales, PROD_SUPPORTED_LOCALES_8W)
    })

    it('should only include production locales in the locale options', () => {
      const localeOptions = getSupportedLocale8wOptions()
      localeOptions.forEach((option) => {
        const locale = option.value
        assert.include(PROD_SUPPORTED_LOCALES_8W, locale)
      })
    })
  })

  describe('when BuildIf.EXPERIMENTAL is enabled', () => {
    beforeEach(() => {
      enableFlag('EXPERIMENTAL')
    })

    it('should be using the dev locales', () => {
      const locales = getSupportedLocales8w()
      assert.sameOrderedMembers(locales, DEV_SUPPORTED_LOCALES_8W)
    })

    it('should only include dev locales in the locale options', () => {
      const localeOptions = getSupportedLocale8wOptions()
      localeOptions.forEach((option) => {
        const locale = option.value
        assert.include(DEV_SUPPORTED_LOCALES_8W, locale)
      })
    })
  })
})
