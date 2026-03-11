import * as React from 'react'

import type {IModelPreview} from './studio-model-preview'

const StudioThreeModelPreview = React.lazy(() => import('./studio-model-preview'))

const StudioModelPreviewWithLoader: React.FC<IModelPreview> = props => (
  <React.Suspense fallback={null}>
    <StudioThreeModelPreview {...props} />
  </React.Suspense>
)

export default StudioModelPreviewWithLoader
