import React from 'react'
import type {DeepReadonly} from 'ts-essentials'

import type {IAccount} from '../common/types/models'

const EnclosedAccountContext = React.createContext<DeepReadonly<IAccount>>(null)

const useEnclosedAccount = () => React.useContext(EnclosedAccountContext)

const EnclosedAccountProvider = EnclosedAccountContext.Provider

export {
  useEnclosedAccount,
  EnclosedAccountProvider,
}
