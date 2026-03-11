import i18n from 'i18next'

import {
  LOCALE_COOKIE_NAME,
} from '../../shared/i18n/i18n-constants'

export const useLocaleChange = (): [string, (newLocale: string) => void] => {
  const setLanguage = (lng: string) => {
    localStorage.setItem(LOCALE_COOKIE_NAME, lng)

    i18n.changeLanguage(lng)
  }

  return [i18n.language, setLanguage]
}
