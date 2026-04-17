import React from 'react'
import {
  autoUpdate, useFloating, shift, flip, FloatingPortal, useInteractions, useClick,
  useDismiss, useRole, offset,
} from '@floating-ui/react'

import {useTranslation} from 'react-i18next'

import type {ProjectClientSide} from '../../shared/desktop/local-sync-types'
import {combine} from '../common/styles'
import {Icon} from '../ui/components/icon'
import {showProject} from '../studio/local-sync-api'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  dropdownItem: {
    'padding': '0.5rem 1rem',
    'cursor': 'pointer',
    'userSelect': 'none',
    'WebkitUserSelect': 'none',
    'MozUserSelect': 'none',
    'msUserSelect': 'none',
    '&:hover': {
      'background': theme.studioBtnHoverBg,
    },
    '&:focus-visible': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
  },
  optionsButton: {
    'cursor': 'pointer',
    'width': '32px',
    'height': '32px',
    'borderRadius': '0.5rem',
    'background': theme.secondaryBtnBg,
    'backdrop-filter': 'blur(5px)',
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    '&:hover': {
      'background': theme.studioBtnHoverBg,
    },
    '&:focus-visible': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
  },
  dropdown: {
    borderRadius: '0.5rem',
    background: theme.bgMain,
    boxShadow: theme.listBoxShadow,
    fontFamily: 'Geist Mono',
    overflow: 'hidden',
    zIndex: 2,
  },
}))

interface IMenuOption {
  label: string
  onClick: () => void
}

const MenuOption: React.FC<IMenuOption> = ({label, onClick}) => {
  const classes = useStyles()

  return (
    <div
      className={classes.dropdownItem}
      role='menuitem'
      tabIndex={0}
      onClick={onClick}
      onKeyDown={(e) => {
        // eslint-disable-next-line local-rules/hardcoded-copy
        if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault()
          onClick()
        }
      }}
    >
      {label}
    </div>
  )
}

interface IProjectListItemOptions {
  project: ProjectClientSide & {appKey: string}
  onDelete: () => void
  onMove: () => void
}

const ProjectListItemOptions: React.FC<IProjectListItemOptions> = ({
  project, onDelete, onMove,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['studio-desktop-pages'])
  const [isOpen, setIsOpen] = React.useState(false)
  const buttonRef = React.useRef<HTMLButtonElement>(null)

  React.useEffect(() => {
    if (!buttonRef.current) return undefined

    const observer = new IntersectionObserver(
      ([entry]) => {
        if (!entry.isIntersecting && isOpen) {
          setIsOpen(false)
        }
      }
    )

    observer.observe(buttonRef.current)
    return () => {
      observer.disconnect()
    }
  }, [isOpen])

  const {refs, floatingStyles, context} = useFloating({
    open: isOpen,
    onOpenChange: setIsOpen,
    placement: 'bottom-end',
    whileElementsMounted: autoUpdate,
    middleware: [
      shift(),
      flip(),
      offset(5),
    ],
  })

  const click = useClick(context)
  const dismiss = useDismiss(context)
  const role = useRole(context, {role: 'menu'})

  const {getReferenceProps, getFloatingProps} = useInteractions([
    click,
    dismiss,
    role,
  ])

  return (
    <button
      type='button'
      className={combine('style-reset', classes.optionsButton)}
      ref={(el) => {
        refs.setReference(el)
        buttonRef.current = el
      }}
      aria-label={t('project_list_item.button.options')}
      aria-expanded={isOpen}
      aria-haspopup='menu'
      {...getReferenceProps()}
    >
      <Icon stroke='kebab' />
      {isOpen && (
        <FloatingPortal>
          <div
            ref={refs.setFloating}
            style={floatingStyles}
            className={classes.dropdown}
            {...getFloatingProps()}
          >
            {project?.location && project.validLocation &&
              <MenuOption
                label={window.electron.os === 'mac'
                  ? t('project_list_item.menu.option.reveal_finder')
                  : t('project_list_item.menu.option.show_folder')}
                onClick={() => { showProject(project.appKey) }}
              />
            }
            {project?.location &&
              <MenuOption
                label={t('project_list_item.menu.option.remove_from_list')}
                onClick={() => { onDelete() }}
              />
            }
            {project?.location && project.validLocation &&
              <MenuOption
                label={t('project_list_item.menu.option.change_disk_location')}
                onClick={onMove}
              />
            }
          </div>
        </FloatingPortal>
      )}
    </button>
  )
}

export {
  ProjectListItemOptions,
}
