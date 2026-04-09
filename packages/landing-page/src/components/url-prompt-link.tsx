import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState} from 'preact/hooks'

interface IUrlPromptLink {
  url: string
}

const COPY_ICON_PATH = `
M 5 7 H 14 A 2 2 90 0 1 16 9 V 20 A 2 2 90 0 1 14 22 H 5 A 2 2 90 0 1 3 20 V 9 A 2 2 90 0 1 5 7 
M 8 4 V 4 A 2 2 90 0 1 10 2 H 19 A 2 2 90 0 1 21 4 V 15 A 2 2 90 0 1 19 17 H 19 L 19 17
`

const CHECK_ICON_PATH = 'M 6 16 L 8 18 L 13 13'

const UrlPromptLink: FC<IUrlPromptLink> = ({url}) => {
  const [didCopy, setDidCopy] = useState(false)

  const handleClick = (event) => {
    event.preventDefault()
    const dummy = document.createElement('input')
    document.body.appendChild(dummy)
    dummy.value = url
    dummy.select()
    document.execCommand('copy')
    document.body.removeChild(dummy)
    setDidCopy(true)
  }

  const text = url.replace(/^https:\/\//, '')

  return (
    <a onClick={handleClick} className='landing8-prompt-link' href={url}>
      <span className='landing8-prompt-link-placeholder'>{text}</span>
      <span className='landing8-prompt-link-overflow-container' aria-hidden='true'>
        <span className='landing8-prompt-link-overflow-text'>
          {text}
          <svg className='landing8-prompt-link-icon' width='24' height='24' viewBox='0 0 24 24'>
            <path
              d={COPY_ICON_PATH}
              fill='none'
              stroke='currentColor'
              stroke-width='2'  // eslint-disable-line react/no-unknown-property
              stroke-linecap='round'  // eslint-disable-line react/no-unknown-property
            />
            {didCopy &&
              <path
                d={CHECK_ICON_PATH}
                fill='none'
                stroke='#080'
                stroke-width='2'  // eslint-disable-line react/no-unknown-property
                stroke-linecap='round'  // eslint-disable-line react/no-unknown-property
                stroke-linejoin='round'  // eslint-disable-line react/no-unknown-property
              />
            }
          </svg>
        </span>
      </span>
    </a>
  )
}

export {
  UrlPromptLink,
}
