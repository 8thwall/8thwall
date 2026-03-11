import * as React from 'react'

import AccordionContent from './accordion-content'
import AccordionTitle from './accordion-title'
import {combine} from '../../common/styles'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  accordionRow: {
    'background': theme.sfcBackgroundDefault,
    'padding': '2em',
    'border': `1px solid ${theme.sfcBorderDefault}`,
    'borderRadius': theme.sfcBorderRadius,
    'backdropFilter': theme.sfcBackdropFilter,
    '& + &': {
      marginTop: '1.5em',
    },
    '& .row-checkbox': {
      display: 'none',
    },
    '& .row-title': {
      'display': 'flex',
      'justifyContent': 'space-between',
      'margin': '0 !important',
      'fontWeight': 'bold',
      'fontSize': '1.25em',
      '& + .row-content': {
        marginTop: '2em !important',
      },
    },
  },
  collapsable: {
    '& .row-content': {
      maxHeight: 0,
      transition: 'all 0.35s',
      display: 'none',
    },
    '& .row-title': {
      'cursor': 'pointer',
      '&::after': {
        content: '"\\276F"',
        width: '1em',
        height: '1em',
        textAlign: 'center',
        transition: 'all 0.35s',
        transform: 'rotate(90deg)',
      },
    },
    '& input:checked': {
      '& + .row-title': {
        '&::after': {
          transform: 'rotate(-90deg)',
          transformOrigin: 'center center',
        },
      },
      '& ~ .row-content': {
        display: 'block',
        maxHeight: '100%',
      },
    },
  },
}))

interface IAccordionProps {
  children: any
  collapsable?: boolean
  className?: string
}

interface IAccordion extends
  React.ForwardRefExoticComponent<IAccordionProps & React.RefAttributes<HTMLDivElement>> {
  Content: typeof AccordionContent
  Title: typeof AccordionTitle
}

/**
 * Custom accordion component build without Semantic UI. An example of how this is used would look
 * like:
 *
 * ```
 * <Accordion>
 *   <Accordion.Title>My Setting Name</Accordion.Title>
 *   <Accordion.Content>
 *     <p>Hello World! This is only visible when the accordion is expanded.</p>
 *   </Accordion.Content>
 * </Accordion>
 * ```
 */
const Accordion = React.forwardRef<HTMLDivElement, IAccordionProps>(
  ({children, collapsable = true, className = ''}, ref) => {
    const classes = useStyles()
    return (
      <div
        className={combine(classes.accordionRow, className, collapsable && classes.collapsable)}
        ref={ref}
      >
        {children}
      </div>
    )
  }
) as IAccordion
Accordion.Content = AccordionContent
Accordion.Title = AccordionTitle

export default Accordion
