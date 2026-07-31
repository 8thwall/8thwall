import * as Preact from 'preact'
import * as React from 'preact'

import {defaultParameters as defaultScaleParameters} from '../src/absolute/parameters'

import * as absolute from '../src/absolute/render'
import {defaultParameters as defaultSkyParameters} from '../src/sky/parameters'
import * as sky from '../src/sky/render'

const Panel = () => (
  <aside id='panel'>
    <h1>Coaching Overlay Render Test</h1>
    <section>
      <h2>Absolute Scale</h2>
      <button
        type='button'
        onClick={() => absolute.showCoachingOverlay({
          ...defaultScaleParameters,
          trackingStatus: 'LIMITED',
          trackingReason: 'INITIALIZING',
        })}
      >Initializing
      </button>
      <button
        type='button'
        onClick={() => absolute.showCoachingOverlay({
          ...defaultScaleParameters,
          trackingStatus: 'NORMAL',
          trackingReason: 'UNDEFINED',
        })}
      >Calibrated
      </button>
      <button
        type='button'
        onClick={() => absolute.hideCoachingOverlay()}
      >Remove
      </button>
    </section>
    <section>
      <h2>Sky Effects</h2>
      <button
        type='button'
        onClick={() => sky.showCoachingOverlay({
          ...defaultSkyParameters,
          shouldShow: true,
        })}
      >Show
      </button>
      <button
        type='button'
        onClick={() => sky.showCoachingOverlay({
          ...defaultSkyParameters,
          shouldShow: false,
        })}
      >Fade out
      </button>
      <button
        type='button'
        onClick={() => sky.hideCoachingOverlay()}
      >Remove
      </button>
    </section>
  </aside>
)

const root = document.createElement('div')
document.body.appendChild(root)
Preact.render(Preact.h(Panel, {}), root)
