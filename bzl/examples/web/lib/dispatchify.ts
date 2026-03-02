const dispatchify = actions => dispatch => (
  Object.keys(actions).reduce((o, key) => {
    o[key] = (...args) => dispatch(actions[key](...args))
    return o
  }, {}))

export {
  dispatchify,
}
