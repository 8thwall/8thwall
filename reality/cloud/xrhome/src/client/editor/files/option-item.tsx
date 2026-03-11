import React, {useState} from 'react'

import {
  FloatingPortal, autoUpdate, useFloating, shift, flip, useClick, useDismiss,
  useRole, useInteractions,
} from '@floating-ui/react'

import {useOptionsDropdownStyles} from './options-dropdown-styles'
import {combine} from '../../common/styles'

interface IOptionItem {
  onClick?: () => void
  options?: IOptionItem[]
  content: string
  externalLink?: string
  a8?: string
}

interface IOptionItemSubmenu {
  isOpen: boolean
  setIsOpen: (open: boolean) => void
  content: string
  options: IOptionItem[]
}

const OptionItemSubmenu: React.FunctionComponent<IOptionItemSubmenu> = ({
  isOpen, setIsOpen, content, options,
}) => {
  const menuClasses = useOptionsDropdownStyles()

  const {refs, floatingStyles, context} = useFloating({
    open: isOpen,
    onOpenChange: setIsOpen,
    placement: 'right-end',
    whileElementsMounted: autoUpdate,
    middleware: [
      shift(),
      flip(),
    ],
  })

  const click = useClick(context, {event: 'mousedown'})
  const dismiss = useDismiss(context)
  const role = useRole(context, {role: 'listbox'})

  const {getReferenceProps, getFloatingProps} = useInteractions([
    click,
    dismiss,
    role,
  ])
  return (
    <div>
      <div
        ref={refs.setReference}
        className={menuClasses.optionItem}
        {...getReferenceProps()}
      >{content}
      </div>
      {isOpen && (
        <FloatingPortal>
          <ul
            ref={refs.setFloating}
            style={{...floatingStyles}}
            className={menuClasses.optionsDropdown}
            onBlur={() => setIsOpen(false)}
            {...getFloatingProps()}
          >
            {options.map(props => (
              <li
                key={props.content}
                className={menuClasses.listItem}
              >
                {/* eslint-disable-next-line @typescript-eslint/no-use-before-define */}
                <OptionItem {...props} />
              </li>
            ))}
          </ul>
        </FloatingPortal>
      )}
    </div>
  )
}

const blurAndStopPropagation = handler => (e) => {
  e.stopPropagation()
  handler(e)
  ;(document.activeElement as HTMLElement)?.blur()
}

const OptionItem: React.FunctionComponent<IOptionItem> =
  ({onClick, content, options, externalLink, a8}) => {
    const [isOpen, setIsOpen] = useState(false)

    const menuClasses = useOptionsDropdownStyles()

    if (externalLink) {
      return (
        <a
          href={externalLink}
          rel='noopener noreferrer'
          target='_blank'
          className={combine('style-reset', menuClasses.optionItem)}
          a8={a8}
        >
          {content}
        </a>
      )
    } else if (options) {
      return (
        <OptionItemSubmenu
          isOpen={isOpen}
          setIsOpen={setIsOpen}
          content={content}
          options={options}
        />
      )
    } else {
      return (
        <button
          type='button'
          onClick={(e) => {
            blurAndStopPropagation(onClick)(e)
            setIsOpen(false)
          }}
          className={combine('style-reset', menuClasses.optionItem)}
          a8={a8}
        >
          {content}
        </button>
      )
    }
  }

export {OptionItem}

export type {IOptionItem}
