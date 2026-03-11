import React, {useState} from 'react'
import uuidv4 from 'uuid/v4'

interface IAccordionTitle {
  active: boolean
  onClick: () => void
  children: any
  className?: string
  a8?: string
}

const AccordionTitle: React.FunctionComponent<IAccordionTitle> = ({
  active,
  onClick,
  children,
  className = '',
  a8,
}) => {
  const [checkboxId, setCheckboxId] = useState('')

  React.useEffect(() => {
    // Lazily generate and set the checkbox id.
    setCheckboxId(uuidv4())
  }, [])

  return (
    <>
      <input
        type='checkbox'
        id={checkboxId}
        className='row-checkbox'
        onChange={() => onClick()}
        checked={active}
      />
      <label className='row-title' htmlFor={checkboxId} a8={a8}>
        <div className={className}>
          {children}
        </div>
      </label>
    </>
  )
}

export default AccordionTitle
