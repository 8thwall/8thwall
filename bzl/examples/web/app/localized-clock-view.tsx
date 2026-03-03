import {Box} from '@material-ui/core'
import Container from '@material-ui/core/Container'
import Typography from '@material-ui/core/Typography'
import * as React from 'react'
import {connect} from 'react-redux'
import {Link, useParams} from 'react-router-dom'

import {timeActions} from 'bzl/examples/web/app/time-actions'

// TODO(nb): Resolve this build error and switch to typescript:
// TS2307: Cannot find module '@material-ui/core' or its corresponding type declarations.

const TimeInTz = ({time, tz = 'America|New_York'}) => (
  <>{
      new Intl.DateTimeFormat(
        'en-US', {
          timeZone: tz.replace(/\|/g, '/'),
          year: 'numeric',
          month: 'numeric',
          day: 'numeric',
          hour: 'numeric',
          minute: 'numeric',
          second: 'numeric',
        }
      ).format(time)
    }
  </>
)

const SafeTimeInTz = ({time, tz}) => {
  try {
    return TimeInTz({time, tz})
  } catch (e) {
    return TimeInTz({time})
  }
}

const RawLocalizedClockView = ({currentTime, currentTz, setTime, setTimezone}) => {
  React.useEffect(() => {
    const myWorker = new Worker('bzl/examples/web/app/worker-min.js')
    myWorker.onmessage = (e) => {
      setTime(e.data)
    }

    const timeUpdate = setInterval(() => {
      myWorker.postMessage({action: 'getTime'})
    }, 1000)
    return () => {
      clearInterval(timeUpdate)
      myWorker.terminate()
    }
  }, [])

  const {tz} = useParams()
  React.useEffect(() => {
    setTimezone(tz)
  }, [tz])

  return (
    <Container maxWidth='sm'>
      <Box my={4}>
        <Typography variant='h4' component='h1' gutterBottom>
          The current time is<br /> <SafeTimeInTz time={currentTime} tz={currentTz} />
        </Typography>
        <ul>
          <li><Link to='/America|New_York'>America/New_York</Link></li>
          <li><Link to='/America|Los_Angeles'>America/Los_Angeles</Link></li>
          <li><Link to='/Asia|Taipei'>Asia/Taipei</Link></li>
        </ul>
      </Box>
    </Container>
  )
}

const LocalizedClockView = connect(state => ({
  currentTime: state.time.currentTime.toString(),
  currentTz: state.time.currentTz,
}), timeActions)(RawLocalizedClockView)

export {
  LocalizedClockView,
}
