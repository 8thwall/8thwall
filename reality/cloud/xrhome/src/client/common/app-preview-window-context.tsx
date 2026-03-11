import React, {useRef} from 'react'

type AppPreviewWindowContextValue = {
  getInlinePreviewWindow: () => Window | null
  setInlinePreviewWindow: (inlinePreviewWindow: Window) => void
  getFramedPreviewWindow: () => Window | null
  setFramedPreviewWindow: (framedPreviewWindow: Window) => void
  getPreviewWindow: () => Window | null
  setPreviewWindow: (previewWindow: Window) => void
}

const AppPreviewWindowContext = React.createContext<AppPreviewWindowContextValue>(null)

const AppPreviewWindowContextProvider: React.FC<React.PropsWithChildren> = ({children}) => {
  const inlinePreviewWindowRef = useRef<Window | null>(null)
  const framedPreviewWindowRef = useRef<Window | null>(null)
  const previewWindowRef = useRef<Window | null>(null)

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

  // eslint-disable-next-line max-len
  const getAndMaybeUpdateFramedPreviewWindow = (): Window => getAndMaybeUpdateWindowRef(framedPreviewWindowRef)

  const getAndMaybeUpdatePreviewWindow = (): Window => getAndMaybeUpdateWindowRef(previewWindowRef)

  const ctxValue = {
    getInlinePreviewWindow: getAndMaybeUpdateInlinePreviewWindow,
    setInlinePreviewWindow: (inlinePreviewWindow: Window) => {
      inlinePreviewWindowRef.current = inlinePreviewWindow
    },
    getFramedPreviewWindow: getAndMaybeUpdateFramedPreviewWindow,
    setFramedPreviewWindow: (framedPreviewWindow: Window) => {
      framedPreviewWindowRef.current = framedPreviewWindow
    },
    getPreviewWindow: getAndMaybeUpdatePreviewWindow,
    setPreviewWindow: (previewWindow: Window) => {
      previewWindowRef.current = previewWindow
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
