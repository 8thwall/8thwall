import React from 'react'
import {createUseStyles} from 'react-jss'
import {Input} from 'semantic-ui-react'

import {combine} from '../../common/styles'
import {brandBlack, gray4, gray2, brandWhite} from '../../static/styles/settings'
import {useTheme} from '../../user/use-theme'

const useStyles = createUseStyles({
  'logSearchBox': {
    '&.dark': {
      '& .icon': {
        'color': gray2,
      },
      '& input': {
        'border': 'none',
        'color': brandWhite,
        'background-color': brandBlack,
        '&:focus': {
          'background-color': brandBlack,
          'color': brandWhite,
        },
        '&::placeholder': {
          'color': gray4,
        },
      },
    },
  },
})

interface ILogSearchBox {
  value: string
  onChange: (v: string) => void
}

const LogSearchBox: React.FunctionComponent<ILogSearchBox> = ({value, onChange}) => {
  const classes = useStyles()
  const themeName = useTheme()
  return (
    <Input
      className={combine(classes.logSearchBox, themeName)}
      icon='search'
      iconPosition='left'
      placeholder='Filter...'
      aria-label='Filter'
      value={value}
      onChange={e => onChange(e.target.value)}
    />
  )
}

export default LogSearchBox
