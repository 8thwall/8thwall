import React from 'react'
import {
  autoUpdate, flip, FloatingFocusManager, FloatingPortal, offset, shift, size, useClick,
  useDismiss, useFloating, useInteractions, useRole, type Placement,
} from '@floating-ui/react'

import {createThemedStyles} from '../theme'
import type {ISliderInputField} from './slider-input-axis-field'
import {combine} from '../../common/styles'
import {
  cleanDragValue, useArrowKeyIncrement, useStyles as useRowStyles,
} from '../../studio/configuration/row-fields'
import {useStudioMenuStyles} from '../../studio/ui/studio-menu-styles'
import {Icon} from './icon'
import {useEphemeralEditState} from '../../studio/configuration/ephemeral-edit-state'
import {SliderInput} from '../../studio/ui/slider-input'

const useStyles = createThemedStyles(theme => ({
  outer: {
    position: 'relative',
    userSelect: 'none',
  },
  container: {
    display: 'flex',
    flexDirection: 'row',
    flex: 1,
    alignItems: 'center',
  },
  inputContainer: {
    display: 'flex',
    flexDirection: 'row',
    flex: 1,
    color: theme.fgMain,
    boxSizing: 'border-box',
    background: theme.sfcBackgroundDefault,
    borderRadius: '0.25rem',
    border: `1px solid ${theme.inputBorder}`,
  },
  icon: {
    display: 'flex',
    alignItems: 'center',
    padding: '0 0.25rem 0 0.5rem',
  },
  borderOverride: {
    borderTopRightRadius: '0',
    borderBottomRightRadius: '0',
  },
  noSlider: {
    '&::-webkit-outer-spin-button, &::-webkit-inner-spin-button': {
      '-webkit-appearance': 'none',
      'margin': 0,
    },
    '-moz-appearance': 'textfield',
  },
  focusBorder: {
    '&:focus-within': {
      'border': `1px solid ${theme.sfcBorderFocus}`,
    },
  },
  textInputDropdownButtonContainer: {
    color: theme.fgMuted,
    background: theme.sfcBackgroundDefault,
    borderRadius: '0',
    borderTopRightRadius: '0.25rem',
    borderBottomRightRadius: '0.25rem',
    height: '2rem',
    border: `1px solid ${theme.inputBorder}`,
    boxSizing: 'border-box',
  },
  disabled: {
    'background': theme.studioBgMain,
    '& input, svg': {
      color: theme.fgDisabled,
    },
  },
  textInputDropdownButton: {
    'display': 'flex',
    'padding': '0 0.5em',
    'alignItems': 'center',
    'justifyContent': 'center',
    'width': '100%',
    'height': '100%',
    'cursor': 'pointer',
    '&:disabled': {
      cursor: 'default',
    },
  },
  menuWrapper: {
    position: 'absolute',
    display: 'none',
    left: 0,
    right: 0,
    zIndex: 10,
    overflowY: 'auto',
  },
  menuOpen: {
    display: 'block',
  },
  centerAlignText: {
    textAlign: 'center',
  },
}))

interface IInputNumberTextboxDropdown extends Omit<ISliderInputField,
  'fullWidth' | 'mixed' | 'label' | 'id' | 'value' | 'onChange'> {
  icon?: React.ReactNode
  dropdownIcon?: ((isOpen: boolean) => React.ReactNode) | React.ReactNode
  placeholder?: string
  menuContent?: React.ReactNode | ((collapse: () => void) => React.ReactNode)
  menuPlacement?: Placement
  menuMatchContainerWidth?: boolean
  menuButtonDisabled?: boolean
  menuOffset?: number
  highlightOnFocus?: boolean
  ariaLabel: string
  supportPercentage?: boolean
  value: string | number
  defaultValue?: string | number
  onChange: (value: string | number) => void
}

const InputNumberTextboxDropdown: React.FC<IInputNumberTextboxDropdown> = ({
  icon, dropdownIcon, value, disabled, placeholder, highlightOnFocus, menuContent, menuPlacement,
  menuMatchContainerWidth, menuButtonDisabled = false, menuOffset = 8, ariaLabel, onChange,
  normalizer, min, max, fixed, step, onDrag, supportPercentage, defaultValue,
}) => {
  const rowClasses = useRowStyles()
  const studioMenuStyles = useStudioMenuStyles()
  const classes = useStyles()
  const [menuOpen, setMenuOpen] = React.useState(false)
  const handleKeydown = useArrowKeyIncrement(value, onChange, min, max, step)

  const {editValue, setEditValue, clear} = useEphemeralEditState({
    value,
    deriveEditValue: (v: string | number) => (
      typeof v === 'number' ? Number(v.toFixed(fixed ?? 3)).toString() : String(v)
    ),
    parseEditValue: (v: string | number) => {
      if (!v) {
        if (defaultValue !== undefined) {
          return [true, defaultValue] as const
        }
        return [false]
      }

      const newValue =
        v ? Number(supportPercentage && typeof v === 'string' ? v.replace(/%$/, '') : v) : NaN

      if (Number.isNaN(newValue)) {
        return [false]
      }

      const normalizedValue = normalizer ? normalizer(newValue) : newValue
      if (min !== undefined && normalizedValue < min) {
        return [false]
      }

      if (max !== undefined && normalizedValue > max) {
        return [false]
      }

      if (typeof v === 'string' && v.endsWith('%')) {
        return supportPercentage ? [true, `${normalizedValue}%`] as const : [false]
      }

      return [true, newValue] as const
    },
    onChange,
  })

  const collapse = () => setMenuOpen(false)
  const {refs, floatingStyles, context} = useFloating({
    open: menuOpen,
    onOpenChange: isOpen => !disabled && setMenuOpen(isOpen),
    placement: menuPlacement ?? 'bottom-start',
    whileElementsMounted: autoUpdate,
    middleware: [
      size({
        apply({rects, elements}) {
          Object.assign(elements.floating.style, {
            width: menuMatchContainerWidth ? `${rects.reference.width}px` : undefined,
            minWidth: menuMatchContainerWidth ? `${rects.reference.width}px` : undefined,
          })
        },
        padding: 10,
      }),
      offset(menuOffset),
      shift(),
      flip(),
    ],
  })

  const click = useClick(context)
  const dismiss = useDismiss(context)
  const role = useRole(context)
  const {getReferenceProps, getFloatingProps} = useInteractions([click, dismiss, role])

  return (
    <div className={classes.outer}>
      <div className={classes.container} ref={refs.setReference}>
        <div
          className={combine(
            classes.inputContainer, highlightOnFocus && classes.focusBorder,
            menuContent && classes.borderOverride, disabled && classes.disabled
          )}
        >
          {icon && (
            <SliderInput
              className={classes.icon}
              value={typeof value === 'string' ? parseFloat(value) : value}
              min={min}
              max={max}
              step={step}
              onChange={(v: number) => {
                const isPercentage = typeof value === 'string' && value.endsWith('%')
                setEditValue(cleanDragValue(v, isPercentage, fixed))
              }}
              disabled={disabled}
              normalizer={normalizer}
              onDrag={onDrag}
            >
              {icon}
            </SliderInput>
          )}
          <input
            type={supportPercentage ? 'text' : 'number'}
            className={combine(rowClasses.input, classes.noSlider, classes.centerAlignText)}
            value={editValue}
            disabled={disabled}
            placeholder={placeholder}
            onChange={e => setEditValue(e.target.value)}
            onBlur={() => clear()}
            aria-label={ariaLabel}
            onFocus={e => e.target.select()}
            onKeyDown={handleKeydown}
          />
        </div>
        {menuContent && (
          <div
            className={combine(
              classes.textInputDropdownButtonContainer, highlightOnFocus && classes.focusBorder,
              menuButtonDisabled && classes.disabled
            )}
            {...getReferenceProps()}
          >
            <button
              type='button'
              className={combine(
                'style-reset', classes.textInputDropdownButton,
                menuOpen && !dropdownIcon && studioMenuStyles.openIcon
              )}
              disabled={menuButtonDisabled}
            >
              {(typeof dropdownIcon === 'function' ? dropdownIcon(menuOpen) : dropdownIcon) ?? (
                <Icon stroke='chevronDown' size={0.5} />
              )}
            </button>
          </div>
        )}
      </div>
      <FloatingPortal>
        <FloatingFocusManager context={context} modal={false} initialFocus={refs.floating}>
          <div
            role='dialog'
            ref={refs.setFloating}
            className={combine(
              studioMenuStyles.studioMenu, rowClasses.selectMenuContainer, classes.menuWrapper,
              menuOpen && classes.menuOpen
            )}
            style={floatingStyles}
            {...getFloatingProps()}
          >
            {typeof menuContent === 'function' ? menuContent(collapse) : menuContent}
          </div>
        </FloatingFocusManager>
      </FloatingPortal>
    </div>
  )
}

export {
  InputNumberTextboxDropdown,
}
