import * as React from 'preact'

const DebugMessage = ({message}: {message: string}) => (
  <div id='debug-message' style='color:#eee; word-break: break-all; position:absolute; top:0'>
    {message}
  </div>
)

export {
  DebugMessage,
}
