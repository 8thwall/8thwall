import CssBaseline from '@material-ui/core/CssBaseline'
import {ThemeProvider} from '@material-ui/core/styles'
import * as React from 'react'  // satisifies eslint: react-in-jsx-scope.
import * as ReactDOM from 'react-dom'
import {Provider} from 'react-redux'
import {BrowserRouter, Redirect, Route} from 'react-router-dom'

import {LocalizedClockView} from 'bzl/examples/web/app/localized-clock-view'
import {getStore} from 'bzl/examples/web/app/redux-store'
import {theme} from 'bzl/examples/web/app/theme'

const store = getStore()

const InitialLoad = () => (
  <Redirect to={{pathname: '/'}} />
)

const ThisApp = () => {
  const routes = [
    {to: '/bzl/examples/web/app/index.html', key: 'InitialLoad', component: InitialLoad, exact: true},
    {to: '/', key: 'LocalizedClockView', component: LocalizedClockView, exact: true},
    {to: '/:tz', key: 'LocalizedClockViewTz', component: LocalizedClockView},
  ]

  return (
    <BrowserRouter>
      <ThemeProvider theme={theme}>
        {/* CssBaseline kickstart an elegant, consistent, and simple baseline to build upon. */}
        <CssBaseline />
        <Provider store={store}>
          {routes.map((r: any) => (
            <Route key={r.key} exact={r.exact} path={r.to} component={r.component} />
          ))}
        </Provider>
      </ThemeProvider>
    </BrowserRouter>
  )
}

window.onload = () => {
  ReactDOM.render(
    <ThisApp />,
    document.querySelector('#root')
  )
}
