const errorAction = (msg: string) => ({
  type: 'ERROR',
  msg,
})

const lightshipErrorAction = (errorCode: string) => ({
  type: 'LIGHTSHIP_ERROR',
  errorCode,
})

const acknowledgeError = () => ({
  type: 'ACKNOWLEDGE_ERROR',
})

export {
  errorAction,
  lightshipErrorAction,
  acknowledgeError,
}
