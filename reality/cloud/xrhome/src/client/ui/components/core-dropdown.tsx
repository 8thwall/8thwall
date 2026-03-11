import React from 'react'
import type {DeepReadonly as RO} from 'ts-essentials'

import {combine} from '../../common/styles'
// Reference implementation:
//   https://w3c.github.io/aria-practices/examples/combobox/combobox-select-only.html

const SEARCH_RESET_DELAY_MS = 750

const MOVEMENT_BY_KEY = {
  Home: -Infinity,
  PageUp: -10,
  ArrowUp: -1,
  ArrowDown: 1,
  PageDown: 10,
  End: Infinity,
} as const

type DropdownOption<T = string> = {
  value: T
  content: React.ReactNode
  section?: string
  shouldNotCollapse?: boolean
  extra?: any  // additional data that you can use with formatVisibleContent
}

type DropdownSection = {
  key: string
  label?: React.ReactNode
  loadMoreNode?: React.ReactNode
}

const makeOptionId = (baseId: string, option: DropdownOption) => `${baseId}-option-${option.value}`

const matchOptionElement = (search: string, element: HTMLElement) => (
  element.innerText.toLowerCase().startsWith(search)
)

const sortOptionsBySections = (
  options: RO<DropdownOption[]>,
  sections: RO<DropdownSection[]>
) => [...options].sort((a, b) => {
  const aSectionIndex = sections.findIndex(s => s.key === a.section)
  const bSectionIndex = sections.findIndex(s => s.key === b.section)
  return aSectionIndex - bSectionIndex
})

interface ICoreDropdown {
  id: string
  labelId: string
  value: string
  options: RO<DropdownOption[]>
  placeholder?: React.ReactNode
  disabled?: boolean
  onChange: (newValue: string) => void
  renderChevron?: (menuOpen: boolean) => React.ReactNode
  formatVisibleContent?: (option: DropdownOption) => React.ReactNode
  targetClassName?: string
  menuClassName?: string
  menuTopClassName?: string
  menuBottomClassName?: string
  menuOpenClassName?: string
  optionClassName?: string
  optionGroupClassName?: string
  optionGroupLabelClassName?: string
  focusedOptionClassName?: string
  sections?: RO<DropdownSection[]>
  maxHeight?: number
  maxWidth?: number
  shouldReposition?: boolean
  positionAbove?: boolean
}

const CoreDropdown: React.FC<ICoreDropdown> = ({
  id, labelId, value, options: unsortedOptions, placeholder, disabled, onChange, renderChevron,
  formatVisibleContent = option => option.content,
  targetClassName, menuClassName, menuTopClassName, menuBottomClassName, menuOpenClassName,
  optionClassName, optionGroupClassName, optionGroupLabelClassName, focusedOptionClassName,
  sections, maxHeight, maxWidth, shouldReposition = true, positionAbove = false,
}) => {
  const [menuOpen, setMenuOpen] = React.useState(false)
  const [focusedValue, setFocusedValue] = React.useState(null)
  const [menuAbove, setMenuAbove] = React.useState(positionAbove)

  const placeholderId = `${id}-placeholder`
  const listboxId = `${id}-listbox`

  const options = sections ? sortOptionsBySections(unsortedOptions, sections) : unsortedOptions

  const selectedIndex = options.findIndex(e => e.value === value)

  const focusedIndex = options.findIndex(e => e.value === focusedValue)
  const focusedId = focusedIndex === -1
    ? placeholderId
    : makeOptionId(id, options[focusedIndex])

  const visibleContents = selectedIndex === -1
    ? placeholder
    : formatVisibleContent(options[selectedIndex])

  const expectedBlur = React.useRef(false)

  const searchResetTimeoutRef = React.useRef<number>()
  const searchStringRef = React.useRef<string>('')
  const previousSearchMatchRef = React.useRef<number>(0)

  React.useEffect(() => () => clearTimeout(searchResetTimeoutRef.current), [])

  const menuRef = React.useRef<HTMLDivElement>()

  React.useLayoutEffect(() => {
    if (menuRef.current) {
      menuRef.current.querySelector(`.${focusedOptionClassName}`)
        ?.scrollIntoView({block: 'nearest'})

      if (shouldReposition) {
        const rem = parseInt(getComputedStyle(document.documentElement).fontSize, 10) / 4
        // NOTE(Dale): We want height, not how far it can scroll here
        const {top, height: menuHeight} = menuRef.current.getBoundingClientRect()
        setMenuAbove((window.innerHeight - top) < (menuHeight + rem))
      }
    }
  }, [menuOpen, focusedValue, shouldReposition])

  const doInitialOpen = () => {
    if (disabled) {
      return
    }
    setFocusedValue(value)
    setMenuOpen(true)
  }

  const resetSearch = () => {
    searchStringRef.current = ''
    previousSearchMatchRef.current = 0
    clearTimeout(searchResetTimeoutRef.current)
  }

  const collapse = () => {
    resetSearch()
    setFocusedValue(null)
    setMenuOpen(false)
  }

  const handleOptionClick = (o: DropdownOption) => {
    setMenuOpen(o.shouldNotCollapse ?? false)
    setFocusedValue(null)
    onChange(o.value)
  }

  // This fixes an expected blur on the field when a menu item is clicked.
  const handlePreOptionClick = () => {
    expectedBlur.current = true
  }

  const applySearch = () => {
    clearTimeout(searchResetTimeoutRef.current)
    searchResetTimeoutRef.current = window.setTimeout(resetSearch, SEARCH_RESET_DELAY_MS)

    if (!menuRef.current) {
      return
    }

    const children = Array.from(menuRef.current.querySelectorAll<HTMLElement>('[role=option]'))

    if (!searchStringRef.current) {
      return
    }

    if (!menuOpen) {
      setMenuOpen(true)
    }

    const isCycle = searchStringRef.current.split('').every(l => l === searchStringRef.current[0])

    // When typing the same character multiple times, cycle between each option that
    // starts with that character.
    if (isCycle) {
      const matchedIndices: number[] = []
      children.forEach((o, i) => {
        if (matchOptionElement(searchStringRef.current[0], o)) {
          matchedIndices.push(i)
        }
      })

      let selected: number
      if (matchedIndices.length > 1) {
        previousSearchMatchRef.current %= matchedIndices.length
        selected = matchedIndices[previousSearchMatchRef.current++]
      } else if (matchedIndices.length === 1) {
        [selected] = matchedIndices
      }

      if (selected !== undefined) {
        setFocusedValue(options[selected].value)
      }
    } else {
      const searchedIndex = children.findIndex(o => matchOptionElement(searchStringRef.current, o))
      if (searchedIndex !== -1) {
        setFocusedValue(options[searchedIndex].value)
      } else {
        resetSearch()
      }
    }
  }

  const handleKeyDown = (e: React.KeyboardEvent<HTMLDivElement>) => {
    const {key, altKey, ctrlKey, metaKey} = e

    // eslint-disable-next-line local-rules/hardcoded-copy
    if (!menuOpen && ['ArrowDown', 'ArrowUp', 'Enter', ' '].includes(key)) {
      e.preventDefault()
      doInitialOpen()
      return
    }

    if (menuOpen && MOVEMENT_BY_KEY[key]) {
      e.preventDefault()
      const currentIndex = focusedIndex === -1 ? selectedIndex : focusedIndex
      const goalIndex = currentIndex + MOVEMENT_BY_KEY[key]
      const newIndex = Math.min(options.length - 1, Math.max(0, goalIndex))
      setFocusedValue(options[newIndex]?.value)
      setMenuOpen(true)
      return
    }

    // eslint-disable-next-line local-rules/hardcoded-copy
    if (menuOpen && key === 'Escape') {
      collapse()
      return
    }

    // eslint-disable-next-line local-rules/hardcoded-copy
    if (menuOpen && (key === 'Enter' || key === ' ')) {
      e.preventDefault()
      onChange(focusedValue)
      collapse()
      return
    }

    // eslint-disable-next-line local-rules/hardcoded-copy
    if (key === 'Backspace' || key === 'Clear') {
      e.preventDefault()
      resetSearch()
      return
    }

    if (key.length === 1 && key !== ' ' && !altKey && !ctrlKey && !metaKey) {
      searchStringRef.current += key.toLowerCase()
      applySearch()
    }
  }

  const handleTargetClick = () => {
    if (menuOpen) {
      collapse()
    } else {
      doInitialOpen()
    }
  }

  const handleTargetBlur = () => {
    if (expectedBlur.current) {
      expectedBlur.current = false
    } else {
      collapse()
    }
  }

  const dropdownOption = (option: DropdownOption) => (
    // NOTE(christoph): Disabling these rules here because the keyboard/focus interactivity
    // is managed on the combobox as opposed to individual option elements
    /* eslint-disable jsx-a11y/click-events-have-key-events */
    /* eslint-disable jsx-a11y/interactive-supports-focus */
    <div
      key={option.value}
      role='option'
      id={makeOptionId(id, option)}
      className={combine(
        optionClassName,
        focusedValue === option.value && focusedOptionClassName
      )}
      aria-selected={value === option.value ? 'true' : undefined}
      onClick={(e: React.MouseEvent) => {
        e.stopPropagation()
        handleOptionClick(option)
      }}
    >
      {option.content}
    </div>
  )

  const dropdownSection = (section: DropdownSection) => (
    <div key={section.key} role='group' className={optionGroupClassName}>
      {section.label && <span className={optionGroupLabelClassName}>{section.label}</span>}
      {options
        .filter(option => option.section === section.key)
        .map(dropdownOption)}
      {React.isValidElement(section.loadMoreNode) && section.loadMoreNode}
    </div>
  )

  return (
    <div>
      <div
        id={id}
        className={targetClassName}
        role='combobox'
        aria-controls={listboxId}
        aria-expanded={menuOpen}
        aria-haspopup='listbox'
        aria-labelledby={labelId}
        aria-disabled={disabled}
        tabIndex={0}
        aria-activedescendant={focusedId}
        onBlur={handleTargetBlur}
        onKeyDown={handleKeyDown}
        onClick={handleTargetClick}
      >
        {visibleContents}
        {renderChevron && renderChevron(menuOpen)}
      </div>
      <div
        ref={menuRef}
        role='listbox'
        className={combine(menuClassName, menuOpen && menuOpenClassName,
          menuAbove ? menuTopClassName : menuBottomClassName)}
        style={{
          maxHeight: maxHeight ? `${maxHeight}px` : undefined,
          maxWidth: maxWidth ? `${maxWidth}px` : undefined,
        }}
        id={listboxId}
        aria-labelledby={labelId}
        tabIndex={-1}
        onMouseDown={handlePreOptionClick}
      >
        {sections
          ? sections.map(dropdownSection)
          : options.map(dropdownOption)}
      </div>
    </div>
  )
}

export {
  CoreDropdown,
}

export type {
  DropdownOption,
  DropdownSection,
}
