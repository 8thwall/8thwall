import * as React from 'react'

interface IAccordionContent {
  children: any
  className?: string
}

const AccordionContent: React.FunctionComponent<IAccordionContent> = ({
  children,
  className = '',
}) => (
  <div className='row-content'>
    <div className={className}>
      {children}
    </div>
  </div>
)

export default AccordionContent
