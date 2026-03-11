enum FeatureCategory {
  Account = 'Account',
  App = 'App',
}

enum FeatureBillingType {
  OneTime = 'OneTime',
  Recurring = 'Recurring',
}

enum FeatureEntityProps {
  Account = 'shortName',
  App = 'appName',
}

export {
  FeatureCategory,
  FeatureBillingType,
  FeatureEntityProps,
}
