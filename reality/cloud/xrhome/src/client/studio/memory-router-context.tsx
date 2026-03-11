import React from 'react'

import {MemoryRouter} from 'react-router-dom'

// Wrapper for MemoryRouter to isolate children components from the main routing context.
// May be modified in future to pass through parent router hooks.

interface IMemoryRouterContext {
  children: React.ReactNode
  initialSpace?: string
}

const MemoryRouterContext: React.FC<IMemoryRouterContext> = ({children, initialSpace}) => (
  <MemoryRouter initialEntries={[`/studio?space=${initialSpace || ''}`]} initialIndex={0}>
    {children}
  </MemoryRouter>
)

export {
  MemoryRouterContext,
}
