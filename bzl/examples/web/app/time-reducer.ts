const initialState = {
  currentTime: new Date('2020/10/5').valueOf(),
  currentTz: 'America|New_York',
}

const timeReducer = (state = initialState, action) => {
  switch (action.type) {
    case 'TIME/SET':
      return {...state, currentTime: action.currentTime}
    case 'TIME/SET_TZ':
      return {...state, currentTz: action.tz}
    default:
      return state
  }
}

export {
  timeReducer,
}
