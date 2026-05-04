import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {ErrorBoundary} from '../../common/error-boundary'
import {StaticBanner} from '../../ui/components/banner'
import {Loader} from '../../ui/components/loader'
import {RowContent} from './row-content'

const RuntimeVersionConfigurator: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <FloatingTraySection title={t('runtime_configurator.title')}>
      <React.Suspense fallback={<Loader centered size='small' inline />}>
        <ErrorBoundary
          fallback={({onReset}) => (
            <RowContent>
              <StaticBanner
                type='danger'
                message={t('runtime_configurator.load_error')}
                onClose={onReset}
              />
            </RowContent>
          )}
        >
          <p>TODO</p>
        </ErrorBoundary>
      </React.Suspense>
    </FloatingTraySection>
  )
}

export {
  RuntimeVersionConfigurator,
}
