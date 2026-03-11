import {brand8MonospaceFontFamily} from './fonts'
import {createThemedStyles} from './theme'

const useTypography = createThemedStyles(theme => ({
  baseHeading: {
    margin: 0,
    fontWeight: 700,
    fontFamily: theme.headingFontFamily,
    lineHeight: '1.5',  // Fallback for h5+
  },
  headingAuto: {
    'extend': 'baseHeading',  // Fallback for h5+
    '&:is(h1)': {
      extend: 'heading1',
    },
    '&:is(h2)': {
      extend: 'heading2',
    },
    '&:is(h3)': {
      extend: 'heading3',
    },
    '&:is(h4)': {
      extend: 'heading4',
    },
  },
  heading1: {
    extend: 'baseHeading',
    fontSize: '80px',
    lineHeight: '80px',
  },
  heading2: {
    extend: 'baseHeading',
    fontSize: '56px',
    lineHeight: '60px',
  },
  heading3: {
    extend: 'baseHeading',
    fontSize: '32px',
    lineHeight: '38px',
  },
  heading4: {
    extend: 'baseHeading',
    fontSize: '20px',
    lineHeight: '30px',
  },
  subtitle: {
    fontFamily: theme.subHeadingFontFamily,
    fontWeight: 700,
    fontSize: '20px',
    lineHeight: '28px',
  },
  bodyMono: {
    fontFamily: brand8MonospaceFontFamily,
    fontWeight: 400,
    fontSize: '14px',
    lineHeight: '20px',
  },
  bodyLarge: {
    fontSize: '16px',
    lineHeight: '24px',
    fontFamily: theme.bodyFontFamily,
  },
  body: {
    fontSize: '14px',
    lineHeight: '20px',
    fontFamily: theme.bodyFontFamily,
  },
  bodySmall: {
    fontSize: '12px',
    lineHeight: '18px',
    fontFamily: theme.bodyFontFamily,
  },
  standardModalTitle: {
    fontFamily: brand8MonospaceFontFamily,
    fontSize: '16px',
    lineHeight: '24px',
    margin: 0,
  },
}))

export {
  useTypography,
}
