import type {PostProcessorModule} from 'i18next'

const MIGRATION_DEBUG_STRING = 'FooBar8'

const I18nMigrationDebugPlugin: PostProcessorModule = {
  type: 'postProcessor',
  name: 'i18nMigrationDebug',
  process: () => MIGRATION_DEBUG_STRING,
}

export default I18nMigrationDebugPlugin
