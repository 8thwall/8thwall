import React from 'react'
import {createUseStyles} from 'react-jss'

import {ProjectList} from './project-list'
import {SpaceBetween} from '../ui/layout/space-between'
import {SideNavBar} from './side-nav-bar'
import {FirstTimeSetupView} from './first-time-setup-view'
import {Loader} from '../ui/components/loader'

const useStyles = createUseStyles({
  homePage: {
    flex: '1 1 auto',
    overflowY: 'hidden',
    display: 'flex',
    flexDirection: 'row',
    padding: '0.5rem 1rem 0 1rem',
    gap: '1rem',
  },
})

const HomePage: React.FC = () => {
  const classes = useStyles()
  return (
    <>
      <div className={classes.homePage}>
        <SideNavBar />
        <SpaceBetween direction='vertical' grow>
          <SpaceBetween extraWide noWrap grow>
            <SpaceBetween extraWide direction='vertical' grow>
              <React.Suspense fallback={<Loader inline centered />}>
                <ProjectList />
              </React.Suspense>
            </SpaceBetween>
          </SpaceBetween>
        </SpaceBetween>
      </div>
      <FirstTimeSetupView />
    </>
  )
}

export {
  HomePage,
}
