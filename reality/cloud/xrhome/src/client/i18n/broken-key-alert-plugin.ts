/* eslint-disable local-rules/hardcoded-copy */
import type {PostProcessorModule} from 'i18next'

let activeAlert: HTMLDivElement | null = null

const BrokenKeyAlertPlugin: PostProcessorModule = {
  type: 'postProcessor',
  name: 'brokenKeyAlert',
  process: (value, key: string | string[], options) => {
    const isBrokenKey = value === key || (Array.isArray(key) && key[0] === value)
    if (isBrokenKey) {
      if (activeAlert) {
        activeAlert.remove()
      }
      const namespace = options?.ns || 'unknown'
      const message = `Malformed translation key: ${key} (${namespace}) `

      const alertBox = document.createElement('div')
      Object.assign(alertBox.style, {
        position: 'fixed',
        bottom: '20px',
        left: '50%',
        transform: 'translateX(-50%)',
        backgroundColor: 'rgba(255, 0, 0, 0.9)',
        color: 'white',
        padding: '15px',
        borderRadius: '5px',
        zIndex: 9999,
        fontSize: '14px',
        whiteSpace: 'pre-wrap',
      })
      alertBox.textContent = message
      document.body.appendChild(alertBox)
      activeAlert = alertBox

      const closeButton = document.createElement('button')
      closeButton.textContent = 'close'
      Object.assign(closeButton.style, {
        background: 'none',
        border: '1px solid white',
        color: 'white',
        fontSize: '16px',
        cursor: 'pointer',
      })

      const close = () => {
        activeAlert = null
        alertBox.remove()
      }
      closeButton.addEventListener('click', close)
      setTimeout(close, 10000)

      alertBox.appendChild(closeButton)
    }

    return value
  },
}

export default BrokenKeyAlertPlugin
