import {useEffect, useRef} from 'react'

type OnMsg = (e: MessageEvent) => void

export const useWindowMessageHandler = (onMsg: OnMsg, isActive: boolean = true) => {
  const functionRef = useRef<OnMsg>()
  functionRef.current = onMsg

  useEffect(() => {
    const handler = msg => functionRef.current(msg)

    if (isActive) {
      window.addEventListener('message', handler)
    }

    return () => {
      window.removeEventListener('message', handler)
    }
  }, [isActive])
}
