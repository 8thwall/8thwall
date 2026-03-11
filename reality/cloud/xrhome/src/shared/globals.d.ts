/* eslint-disable no-var, vars-on-top */
declare var Build8: import('./build8').Build8Replacements
declare var BuildIf: import('./buildif').BuildIfReplacements

interface Window {
  __REDUX_DEVTOOLS_EXTENSION_COMPOSE__?: unknown

  // SSR Data
  initialLanguage?: string
  __INITIAL_DATA__?: string  // Base 64
  initialI18nStore?: string  // Base 64
}
