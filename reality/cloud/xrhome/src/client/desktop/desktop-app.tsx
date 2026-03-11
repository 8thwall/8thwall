import React from 'react'
import {Redirect, Route, Router, Switch, useHistory, useLocation} from 'react-router-dom'
import type {History} from 'history'

import {HelmetProvider} from 'react-helmet-async'

import '../i18n/i18n'
import {HomePage} from './home-page'
import {
  HOME_PATH, ROOT_PATH,
  LOCAL_STUDIO_PATH_FORMAT,
} from './desktop-paths'
import {CaughtErrorPage} from './caught-error-page'
import {ErrorBoundary} from '../common/error-boundary'
import {UiThemeProvider} from '../ui/theme'
import withTranslationLoaded from '../i18n/with-translations-loaded'
import {Loader} from '../ui/components/loader'
import {NotFoundPage} from './not-found-page'
import {DesktopWithTopBar} from './desktop-top-bar'
import {Brand8QaContextProvider} from '../brand8/brand8-qa-context'
import {useTheme} from '../user/use-theme'

const LocalStudioPage = React.lazy(() => import('./local-studio-page'))

interface IApp {
  history: History
}

const HistoryResumer = ({children}: {children: React.ReactElement}) => {
  const {pathname: currentPath} = useLocation()

  if (currentPath !== ROOT_PATH) {
    return children
  }

  return <Redirect to={HOME_PATH} />
}

const InnerApp: React.FC = () => {
  const history = useHistory()

  React.useEffect(() => {
    const unsubscribe = window.electron.onExternalNavigate((pathAndQuery) => {
      history.push(pathAndQuery)
    })
    return () => {
      unsubscribe()
    }
  }, [history])

  const theme = useTheme()

  return (
    <UiThemeProvider mode={theme}>
      <Brand8QaContextProvider>
        <DesktopWithTopBar windowTitle={undefined}>
          <ErrorBoundary fallback={CaughtErrorPage}>
            <HistoryResumer>
              <React.Suspense fallback={<Loader />}>
                <Switch>
                  <Route path={HOME_PATH} exact component={HomePage} />
                  <Route
                    path={LOCAL_STUDIO_PATH_FORMAT}
                    render={() => <LocalStudioPage />}
                  />
                  <Route component={NotFoundPage} />
                </Switch>
              </React.Suspense>
            </HistoryResumer>
          </ErrorBoundary>
        </DesktopWithTopBar>
      </Brand8QaContextProvider>
    </UiThemeProvider>
  )
}

const App: React.FC<IApp> = withTranslationLoaded(({history}) => (
  <HelmetProvider>
    <Router history={history}>
      <InnerApp />
    </Router>
  </HelmetProvider>
))

export default App
