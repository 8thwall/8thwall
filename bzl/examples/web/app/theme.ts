import {red} from '@material-ui/core/colors'
import {createMuiTheme} from '@material-ui/core/styles'

// TODO(nb): Resolve this build error and switch to typescript:
// TS2307: Cannot find module '@material-ui/core/styles' or its corresponding type declarations.

// A custom theme for this app
const theme = createMuiTheme({
  palette: {
    primary: {
      main: '#556cd6',
    },
    secondary: {
      main: '#19857b',
    },
    error: {
      main: red.A400,
    },
    background: {
      default: '#fff',
    },
  },
})

export {
  theme,
}
