import {useLayoutEffect} from 'react'

import {createThemedStyles, UiTheme} from './theme'

const makeStyles = (theme: UiTheme) => ({
  '--bgMain': theme.bgMain,
  '--fgMain': theme.fgMain,
  '--bodyFontFamily': theme.bodyFontFamily,
  '--linkFg': theme.linkBtnFg,
  '--sfcBackgroundDefault': theme.sfcBackgroundDefault,
  '--sfcBackgroundError': theme.sfcBackgroundError,
  '--sfcBorderDefault': theme.sfcBorderDefault,
  '--sfcBorderFocus': theme.sfcBorderFocus,
  '--sfcBorderError': theme.sfcBorderError,
  '--sfcDisabledBackground': theme.sfcDisabledBackground,
})

const useThemedRootStyles = createThemedStyles(theme => ({
  themedRoot: makeStyles(theme),
}))

const useClientThemedGlobalStyles = () => {
  const classes = useThemedRootStyles()

  useLayoutEffect(() => {
    const themeClass = classes.themedRoot
    document.documentElement.classList.add(themeClass)

    return () => {
      document.documentElement.classList.remove(themeClass)
    }
  }, [classes.themedRoot])
}

const useSsrThemedRootStyles = createThemedStyles(theme => ({
  '@global': {
    // Need higher specificity to override ":root" in scss
    'html:root': makeStyles(theme),
  },
}))

// NOTE(christoph): The @global approach had some sort of issue on navigation where styles would
// not be cleaned up on page unmount - https://github.com/8thwall/code8/pull/4206
// However, useLayoutEffect does not run on the server, so we need to use different approaches
// based on if we're on the server or client.
const useThemedGlobalStyles = typeof window === 'undefined'
  ? useSsrThemedRootStyles
  : useClientThemedGlobalStyles

export {
  useThemedGlobalStyles,
}
