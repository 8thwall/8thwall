import React from 'react'

import {SliderInputProps, useSliderInput} from '../../ui/hooks/use-slider-input'
import {combine} from '../../common/styles'

interface ISliderInput extends SliderInputProps {
  className?: string
  children: React.ReactNode
}

const SliderInput: React.FC<ISliderInput> = ({
  className, children, ...rest
}) => {
  const draggableProps = useSliderInput(rest)

  return (
    <div
      {...draggableProps}
      className={combine(draggableProps.className, className)}
    >
      {children}
    </div>
  )
}

export {
  SliderInput,
}
