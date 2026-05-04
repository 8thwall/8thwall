import React, {useRef} from 'react'

type AppPreviewWindowContextValue = {
  getInlinePreviewWindow: () => Window | null
  setInlinePreviewWindow: (inlinePreviewWindow: Window) => void
}

const AppPreviewWindowContext = React.createContext<AppPreviewWindowContextValue>(null)

const AppPreviewWindowContextProvider: React.FC<React.PropsWithChildren> = ({children}) => {
  const inlinePreviewWindowRef = useRef<Window | null>(null)

  /**
   * Check if the window has been closed, and update it. Otherwise just return the saved reference.
   * @param ref
   */
  const getAndMaybeUpdateWindowRef = (
    ref: React.MutableRefObject<Window | null>
  ): Window | null => {
    if (ref.current && !ref.current.closed) {
      return ref.current
    } else if (ref.current && ref.current.closed) {
      ref.current = null
    }

    return null
  }

  // eslint-disable-next-line max-len
  const getAndMaybeUpdateInlinePreviewWindow = (): Window => getAndMaybeUpdateWindowRef(inlinePreviewWindowRef)
  const ctxValue = {
    getInlinePreviewWindow: getAndMaybeUpdateInlinePreviewWindow,
    setInlinePreviewWindow: (inlinePreviewWindow: Window) => {
      inlinePreviewWindowRef.current = inlinePreviewWindow
    },
  }

  return (
    <AppPreviewWindowContext.Provider value={ctxValue}>
      {children}
    </AppPreviewWindowContext.Provider>
  )
}

const useAppPreviewWindow = () => React.useContext(AppPreviewWindowContext)

export {
  AppPreviewWindowContextProvider,
  useAppPreviewWindow,
}
