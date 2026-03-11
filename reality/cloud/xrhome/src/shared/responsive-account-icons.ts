import type {DeepReadonly} from 'ts-essentials'

const ACCOUNT_ICON_PREFIX = 'https://cdn.8thwall.com/web/accounts/icons/'

type ResponsiveAccountIcons = DeepReadonly<Record<'16' | '40' | '100' | '200' | '400', string>>

type AccountIcon = DeepReadonly<{icon?: string | ResponsiveAccountIcons | null}>

const responsiveAccountIcons = (a: AccountIcon): ResponsiveAccountIcons => {
  if (!a.icon) {
    return null
  }

  if (typeof a.icon === 'string') {
    return {
      '16': `${ACCOUNT_ICON_PREFIX}${a.icon}-16x16`,
      '40': `${ACCOUNT_ICON_PREFIX}${a.icon}-40x40`,
      '100': `${ACCOUNT_ICON_PREFIX}${a.icon}-100x100`,
      '200': `${ACCOUNT_ICON_PREFIX}${a.icon}-200x200`,
      '400': `${ACCOUNT_ICON_PREFIX}${a.icon}-400x400`,
    }
  }

  return a.icon
}

export {
  responsiveAccountIcons,
}

export type {
  ResponsiveAccountIcons,
}
