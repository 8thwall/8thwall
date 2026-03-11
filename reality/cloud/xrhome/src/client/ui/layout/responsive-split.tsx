import React from 'react'
import {createUseStyles} from 'react-jss'

const useStyles = createUseStyles({
  responsiveSplit: {
    display: 'flex',
    flexWrap: 'wrap',
    marginRight: '-1rem',
    marginBottom: '-1rem',
  },
  responsiveChild: {
    flex: '1 1 15em',
    minWidth: '0',
    marginRight: '1rem',
    marginBottom: '1rem',
  },
})

const ResponsiveSplit: React.FunctionComponent<React.PropsWithChildren> = ({children}) => {
  const {responsiveSplit, responsiveChild} = useStyles()
  const childArray = Array.isArray(children) ? children : [children]
  return (

    <div className={responsiveSplit}>
      {childArray.map((child => (
        <div className={responsiveChild}>
          {child}
        </div>
      )))}
    </div>
  )
}

export default ResponsiveSplit
