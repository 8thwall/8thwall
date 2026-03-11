import React from 'react'

interface ScrollTo {
  line?: number
  column?: number
}

const useAceScroll = (aceRef, scrollTo: ScrollTo) => {
  React.useLayoutEffect(() => {
    const ace = aceRef.current
    if (ace && scrollTo) {
      const {line, column} = scrollTo
      if (line > 0) {
      // Change input to match ace's 0-based indexing.
        const l = line - 1
        const c = (column > 0) ? column - 1 : 0
        ace.editor.resize(true)
        ace.editor.scrollToLine(l, true, false, () => {})
        ace.editor.moveCursorTo(l, c)
        ace.editor.clearSelection()
      }
    }
  }, [scrollTo])
}

export type {
  ScrollTo,
}

export {
  useAceScroll,
}
