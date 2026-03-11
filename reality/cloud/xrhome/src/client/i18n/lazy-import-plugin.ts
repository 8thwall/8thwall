import type {BackendModule} from 'i18next'

const LazyImportPlugin: BackendModule = {
  type: 'backend',
  init: () => {},
  read: async (language, namespace, callback) => {
    const translations = await import(`./${language}/${namespace}.json`)
    callback(null, translations)
  },
}

export default LazyImportPlugin
