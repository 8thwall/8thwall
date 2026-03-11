import React from 'react'

import type {IApp, IAccount} from '../../common/types/models'
import FileLink from '../file-link'
import type {StackFrame} from './types'

interface IStackDisplay {
  stack: readonly StackFrame[]
  account: IAccount
  app: IApp
}

const StackDisplay: React.FC<IStackDisplay> = ({stack, account, app}) => (
  <div>
    {stack.map((stackFrame, i) => (
      // eslint-disable-next-line react/no-array-index-key
      <div key={i}>
        {'    at '}
        {stackFrame.function || '(anonymous)'}
        {' '}
        <FileLink account={account} app={app} {...stackFrame} />
      </div>
    ))}
  </div>
)

export default StackDisplay
