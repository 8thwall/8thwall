// Used for directing splash screen logic in app8
// Should be similar to the AppBuildSettingsSplashScreen Database column
type SplashType =
  | 'none'
  | 'launch'
  | 'develop'
  | 'noncommercial'
  | 'educational'
  | 'app'
  | 'demo'

export type {
  SplashType,
}
