import '../static/styles/index.scss'
import '../static/semantic/dist/semantic.min.css'
import '../static/styles/account-deep-link.scss'

import './styles.scss'

import * as React from 'react'
import {Provider} from 'react-redux'
import {JssProvider, createGenerateId, SheetsRegistry} from 'react-jss'

import {createRoot} from 'react-dom/client'
import {QueryClient, QueryClientProvider} from '@tanstack/react-query'

import {setResourceBase} from '@ecs/shared/resources'

import App from './desktop-app'

import {getHistory, getStore} from '../reducer'

const root = document.querySelector('#xrhome-desktop-root')
const appRoot = createRoot(root)

const sheets = new SheetsRegistry()
const generateId = createGenerateId()
const queryClient = new QueryClient()
queryClient.setDefaultOptions({
  queries: {
    refetchOnWindowFocus: false,
    networkMode: 'always',
  },
})

setResourceBase('desktop://dist/ecs-resources/')

const store = getStore()

appRoot.render(
  <QueryClientProvider client={queryClient}>
    <React.Suspense fallback={null}>
      <JssProvider registry={sheets} generateId={generateId}>
        <Provider store={store}>
          <App history={getHistory()} />
        </Provider>
      </JssProvider>
    </React.Suspense>
  </QueryClientProvider>
)
