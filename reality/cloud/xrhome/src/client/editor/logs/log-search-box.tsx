import React from 'react'
import {useTranslation} from 'react-i18next'

import {Icon} from '../../ui/components/icon'
import {StandardTextInput} from '../../ui/components/standard-text-input'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  search: {
    'position': 'relative',
    'width': '180px',
    '& input': {
      paddingLeft: '2.67142857em',
    },
  },
  icon: {
    position: 'absolute',
    top: '50%',
    left: '0.75em',
    zIndex: 1,
    color: theme.fgMuted,
    pointerEvents: 'none',
    transform: 'translateY(-50%)',
  },
}))

interface ILogSearchBox {
  value: string
  onChange: (v: string) => void
}

const LogSearchBox: React.FunctionComponent<ILogSearchBox> = ({value, onChange}) => {
  const classes = useStyles()
  const {t} = useTranslation(['asset-lab'])
  const filterLabel = t('asset_lab.library.filter')

  return (
    <div className={classes.search}>
      <span className={classes.icon}>
        <Icon stroke='search' size={0.75} />
      </span>
      <StandardTextInput
        id='log-search-box'
        height='tiny'
        placeholder={`${filterLabel}...`}
        aria-label={filterLabel}
        value={value}
        onChange={e => onChange(e.target.value)}
      />
    </div>
  )
}

export default LogSearchBox
