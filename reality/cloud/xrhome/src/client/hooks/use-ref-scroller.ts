import React from 'react'

// This does collide Id globally. Ok for uuid, need an improved version for more usage.
// You can prefix your id, e.g. `file:assets/foobar.png` vs `librarySquare:12345`
const useRefScroller = <IdT>() => {
  const refMap = React.useRef(new Map<IdT, HTMLElement>())
  const targetScrollTo = React.useRef<IdT | null>(null)

  return {
    scrollTo(id: IdT) {
      const element = refMap.current.get(id)
      if (element) {
        element.scrollIntoView({behavior: 'smooth', block: 'nearest'})
      } else {
        // Might not be loaded yet
        targetScrollTo.current = id
      }
    },
    setRef(id: IdT, element: HTMLElement | null) {
      if (element) {
        refMap.current.set(id, element)
        if (targetScrollTo.current === id) {
          element.scrollIntoView({behavior: 'smooth', block: 'nearest'})
          targetScrollTo.current = null
        }
      } else {
        refMap.current.delete(id)
      }
    },
    getRef(id: IdT): HTMLElement | undefined {
      return refMap.current.get(id)
    },
    clearRefs() {
      refMap.current.clear()
      targetScrollTo.current = null
    },
    hasRef(id: IdT) {
      return refMap.current.has(id)
    },
    deleteRef(id: IdT) {
      refMap.current.delete(id)
    },
  }
}

type RefScroller<IdT> = ReturnType<typeof useRefScroller<IdT>>
export {
  useRefScroller,
}

export type {
  RefScroller,
}
