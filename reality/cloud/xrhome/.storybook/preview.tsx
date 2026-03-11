import React from 'react'
import '../src/client/static/styles/index.scss'
import '../src/client/static/styles/account-deep-link.scss'
import '../src/client/static/styles/code-editor.scss'
import {useGlobals} from '@storybook/addons'

import {UiThemeProvider} from '../src/client/ui/theme'
import {moonlight, darkBlue, brandWhite} from '../src/client/static/styles/settings'
import {brand8Black} from '../src/client/ui/colors'
import {useThemedGlobalStyles} from '../src/client/ui/globals'

const ThemedStory: React.FC<{story: React.ComponentType}> = ({story:Story}) => {
  useThemedGlobalStyles()
  return (
    <Story />
  )
}

const decorators = [
  (Story: React.ComponentType) => {
    const [globals] = useGlobals()

    const theme = React.useMemo(() => {
      const color: string = globals.backgrounds?.value

      if (!color || color === 'transparent') {
        return 'light'
      }

      if (color === brandWhite) {
        return 'brand8light'
      }

      if (color === brand8Black) {
        return 'brand8dark'
      }

      // Assuming hex, look at the red channel and see if it is lighter or darker
      return parseInt(color.substring(1, 3), 16) > 128 ? 'light' : 'dark'
    }, [globals.backgrounds])

    return (
      <div id='xrhome-root' style={{position: "static"}}>
        <div>
          <UiThemeProvider mode={theme} skipNormalize>
            <ThemedStory story={Story} />
          </UiThemeProvider>
        </div>
      </div>
    )
  },
]

const parameters = {
  actions: {argTypesRegex: '^on[A-Z].*'},
  controls: {
    matchers: {
      color: /(background|color)$/i,
      date: /Date$/,
    },
  },
  backgrounds: {
    values: [
      {name: 'Dark', value: darkBlue},
      {name: 'Light', value: moonlight},
      {name: 'Brand8 - Light', value: brandWhite},
      {name: 'Brand8 - Dark', value: brand8Black},
    ],
  },
}

const preview = {
  decorators,
  parameters,
}

export default preview
