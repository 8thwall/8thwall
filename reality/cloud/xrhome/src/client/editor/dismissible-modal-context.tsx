import React from 'react'

interface IDismissibleModalContext {
  addModal: (onClose: () => void) => number
  removeModal: (id: number) => void
  dismissModals: () => void
}

type DismissibleModals = Record<number, () => void>

const DismissibleModalContext = React.createContext<IDismissibleModalContext>(null)

const DismissibleModalContextProvider: React.FC<React.PropsWithChildren> = ({children}) => {
  let modalIdCount = 1

  const modals = React.useRef<DismissibleModals>({})

  const addModal = (onClose: () => void) => {
    const id = modalIdCount++
    modals.current[id] = onClose
    return id
  }

  const removeModal = (id: number) => {
    delete modals.current[id]
  }

  const dismissModals = () => Object.values(modals.current).forEach(onClose => onClose())

  const value: IDismissibleModalContext = {
    addModal,
    removeModal,
    dismissModals,
  }

  return (
    <DismissibleModalContext.Provider value={value}>
      {children}
    </DismissibleModalContext.Provider>
  )
}

const useDismissibleModalContext = () => React.useContext(DismissibleModalContext)

const useDismissibleModal = (onClose: () => void) => {
  const context = useDismissibleModalContext()

  React.useEffect(() => {
    const id = context.addModal(onClose)
    return () => {
      context.removeModal(id)
    }
  }, [onClose])
}

export {
  DismissibleModalContextProvider,
  useDismissibleModalContext,
  useDismissibleModal,
}
