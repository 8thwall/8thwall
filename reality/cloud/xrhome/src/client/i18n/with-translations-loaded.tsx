import React from 'react'

import {Loader} from '../ui/components/loader'

// HOC which will display a loading spinner while any translations
// in the child Component are loading.
const withTranslationLoaded = <T extends {}>(Component: React.ComponentType<T>) => (props: T) => (
  <React.Suspense fallback={<Loader centered />}>
    <Component {...props} />
  </React.Suspense>
)

export default withTranslationLoaded
