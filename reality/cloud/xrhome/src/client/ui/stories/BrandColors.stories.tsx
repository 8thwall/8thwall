import React from 'react'
import type {Meta} from '@storybook/react'

import {
  accessibleHighlight,
  blueberry,
  brandBlack,
  brandHighlight,
  brandPurple,
  brandPurpleDark,
  brandWhite,
  cherry,
  cherryLight,
  darkBlue,
  darkBlueberry,
  darkMango,
  darkMint,
  gray0,
  gray1,
  gray2,
  gray3,
  gray4,
  gray5,
  mango,
  mint,
  moonlight,
} from '../../static/styles/settings'
import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(theme => ({
  swatch: {
    width: '3rem',
    height: '3rem',
    borderRadius: '1rem',
    boxShadow: '6px 6px 10px -4px #0004',
  },
  heroSwatch: {
    width: '4rem',
    height: '4rem',
  },
  colorColumn: {
    display: 'flex',
    flexDirection: 'column',
    gap: '2rem',
    minWidth: '6.5rem',
  },
  subtitle: {
    color: theme.fgMuted,
    display: 'block',
    marginTop: '1rem',
  },
  colorDisplay: {
    textTransform: 'uppercase',
  },
  colorGrid: {
    display: 'flex',
    flexWrap: 'wrap',
    justifyContent: 'flex-start',
    alignItems: 'flex-start',
    gap: '3rem',
  },
  colorRow: {
    display: 'flex',
    flexWrap: 'wrap',
    justifyContent: 'flex-start',
    alignItems: 'flex-start',
    gap: '2rem',
    marginBottom: '4rem',
  },
}))

const ColorColumn: React.FC<React.PropsWithChildren> = ({children}) => (
  <div className={useStyles().colorColumn}>{children}</div>
)

interface ISwatch {
  title?: React.ReactNode
  color: string
  hero?: boolean
  opacity?: number
}

const Swatch: React.FC<ISwatch> = ({title, color, hero, opacity = 1}) => {
  const classes = useStyles()
  const swatchClass = combine(hero && classes.heroSwatch, classes.swatch)
  return (
    <div>
      <div className={swatchClass} style={{background: color, opacity}} />
      <span className={classes.subtitle}>{title || (`${Math.round(100 * opacity)}%`)}
        {hero && <><br /><span className={classes.colorDisplay}>{color}</span></>}
      </span>
    </div>
  )
}

const BrandColorsPage: React.FC = () => {
  const classes = useStyles()

  return (
    <>
      <div className={classes.colorRow}>
        <Swatch color={brandWhite} title='White' hero />
        <Swatch color={moonlight} title='Moonlight' hero />
        <Swatch color={gray0} title='Gray 0' hero />
        <Swatch color={gray1} title='Gray 1' hero />
        <Swatch color={gray2} title='Gray 2' hero />
        <Swatch color={gray3} title='Gray 3' hero />
        <Swatch color={gray4} title='Gray 4' hero />
        <Swatch color={gray5} title='Gray 5' hero />
        <Swatch color={brandBlack} title='8W Black' hero />
        <Swatch color={darkBlue} title='Dark Black' hero />
      </div>
      <div className={classes.colorGrid}>
        <ColorColumn>
          <Swatch color={brandPurple} title='8W Purple' hero />
          <Swatch color={brandPurple} opacity={0.33} />
          <Swatch color={brandPurple} opacity={0.20} />
          <Swatch color={brandPurple} opacity={0.10} />
          <Swatch color={brandPurple} opacity={0.02} />
          <Swatch color={brandPurpleDark} title='Dark' />
        </ColorColumn>
        <ColorColumn>
          <Swatch color={brandHighlight} title='8W Highlight' hero />
          <Swatch color={brandHighlight} opacity={0.33} />
          <Swatch color={brandHighlight} opacity={0.20} />
          <Swatch color={brandHighlight} opacity={0.10} />
          <Swatch color={brandHighlight} opacity={0.02} />
          <Swatch color={accessibleHighlight} title='Light' />
        </ColorColumn>
        <ColorColumn>
          <Swatch color={blueberry} title='Blueberry' hero />
          <Swatch color={blueberry} opacity={0.33} />
          <Swatch color={blueberry} opacity={0.20} />
          <Swatch color={blueberry} opacity={0.10} />
          <Swatch color={blueberry} opacity={0.02} />
          <Swatch color={darkBlueberry} title='Dark' />
        </ColorColumn>
        <ColorColumn>
          <Swatch color={mint} title='Mint' hero />
          <Swatch color={mint} opacity={0.33} />
          <Swatch color={mint} opacity={0.20} />
          <Swatch color={mint} opacity={0.10} />
          <Swatch color={mint} opacity={0.02} />
          <Swatch color={darkMint} title='Dark' />
        </ColorColumn>
        <ColorColumn>
          <Swatch color={mango} title='Mango' hero />
          <Swatch color={mango} opacity={0.33} />
          <Swatch color={mango} opacity={0.20} />
          <Swatch color={mango} opacity={0.10} />
          <Swatch color={mango} opacity={0.02} />
          <Swatch color={darkMango} title='Dark' />
        </ColorColumn>
        <ColorColumn>
          <Swatch color={cherry} title='Cherry' hero />
          <Swatch color={cherry} opacity={0.33} />
          <Swatch color={cherry} opacity={0.20} />
          <Swatch color={cherry} opacity={0.10} />
          <Swatch color={cherry} opacity={0.02} />
          <Swatch color={cherryLight} title='Light' />
        </ColorColumn>
      </div>
    </>
  )
}

export default {
  title: 'Views/BrandColors',
  component: BrandColorsPage,
} as Meta<typeof BrandColorsPage>

export const All = {}
