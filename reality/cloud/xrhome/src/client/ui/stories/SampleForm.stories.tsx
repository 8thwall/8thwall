import React from 'react'
import type {Meta} from '@storybook/react'
import {capitalize} from 'lodash'

import {StandardTextField} from '../components/standard-text-field'
import SpaceBelow from '../layout/space-below'
import {createThemedStyles} from '../theme'
import {StandardSelectField} from '../components/standard-select-field'
import {StandardToggleField} from '../components/standard-toggle-field'
import {StandardRadioButton, StandardRadioGroup} from '../components/standard-radio-group'
import {StandardDropdownField} from '../components/standard-dropdown-field'
import {PrimaryButton} from '../components/primary-button'

const useStyles = createThemedStyles(theme => ({
  form: {
    padding: '1em',
    width: 'fit-content',
  },
  link: {
    color: theme.fgPrimary,
  },
  blue: {color: theme.fgBlue},
  error: {color: theme.fgError},
  muted: {color: theme.fgMuted},
}))

const ICE_CREAM_OPTIONS = ['vanilla', 'chocolate', 'strawberry'] as const

const SampleFormView: React.FC = () => {
  const [text, setText] = React.useState('')
  const [toggle, setToggle] = React.useState(false)
  const [scoop, setScoop] = React.useState<(typeof ICE_CREAM_OPTIONS)[number]>('vanilla')
  const [topping, setTopping] = React.useState<(typeof TOPPINGS_OPTIONS)[number]['value']>('none')

  const textProps = {
    value: text,
    onChange: (e: React.ChangeEvent<HTMLInputElement>) => setText(e.target.value),
  }

  const toggleProps = {
    checked: toggle,
    onChange: setToggle,
  }
  const classes = useStyles()

  const TOPPINGS_OPTIONS = [
    {value: 'none', content: <>None</>},
    {
      value: 'sprinkles',
      content: (
        <b className={classes.muted} style={{textDecoration: 'underline'}}>
          Sprinkles
        </b>
      ),
    },
    {value: 'fudge', content: <i className={classes.blue}>Hot Fudge</i>},
    {value: 'marshmallows', content: 'Marshmallows'},
    {
      value: 'marshmallows-mini',
      content: <small className={classes.error}>Mini Marshmallows</small>,
    },
    {
      value: 'syrup',
      content: (
        <>
          <b style={{outline: '1px solid purple'}}>Hot Fudge Syrup</b>
          <span style={{background: 'black', color: 'white'}}>
            Limited Time only!
          </span>
        </>),
    },
  ] as const

  return (
    <form className={classes.form} onSubmit={e => e.preventDefault()}>
      <SpaceBelow>
        <StandardRadioGroup label='Favorite flavor'>
          {ICE_CREAM_OPTIONS.map(flavor => (
            <StandardRadioButton
              id={`flavor-${flavor}`}
              label={capitalize(flavor)}
              key={flavor}
              checked={scoop === flavor}
              onChange={() => setScoop(flavor)}
            />
          ))}
        </StandardRadioGroup>
      </SpaceBelow>
      <SpaceBelow>
        <StandardDropdownField
          id='dropdown-1'
          label='Choose a topping'
          value={topping}
          onChange={(v: any) => setTopping(v)}
          options={TOPPINGS_OPTIONS}
        />
        <p>Current Topping: {topping}</p>
      </SpaceBelow>
      <SpaceBelow>
        <StandardTextField
          {...textProps}
          id='1'
          label='Base Field'
          placeholder='(placeholder)'
        />
      </SpaceBelow>
      <SpaceBelow>
        <StandardTextField
          {...textProps}
          id='2'
          label='Do not type any vowels'
          errorMessage={text.match(/[aeiou]/gi) ? 'No vowels allowed.' : ''}
        />
      </SpaceBelow>
      <SpaceBelow>
        <StandardTextField {...textProps} id='4' label='Small' height='small' />
      </SpaceBelow>
      <SpaceBelow>
        <StandardTextField {...textProps} id='4' label='Tiny' height='tiny' />
      </SpaceBelow>
      <SpaceBelow>
        <div
          style={{
            display: 'grid',
            gridTemplateColumns: 'repeat(3, 1fr)',
            gap: '1rem',
          }}
        >
          <StandardTextField
            {...textProps}
            id='username'
            label='Username'
          />
          <StandardTextField
            {...textProps}
            id='password'
            label='Password'
          />
          <StandardSelectField
            id='hosting'
            label='Favorite fruit'
          >
            <option>Apple</option>
            <option>Banana</option>
            <option>Orange</option>
            <option>Pear</option>
            <option>Pineapple</option>
          </StandardSelectField>
        </div>
      </SpaceBelow>
      <SpaceBelow>
        <StandardSelectField id='Commit' label='Commit'>
          <option value='latest'>Latest</option>
          <optgroup label='Commits'>
            <option value='abc'>ABC</option>
            <option value='def'>DEF</option>
            <option value='ghi'>GHI</option>
          </optgroup>
        </StandardSelectField>
      </SpaceBelow>
      <SpaceBelow>
        <StandardToggleField id='slider1' label='Toggle' {...toggleProps} />
      </SpaceBelow>
      <SpaceBelow>
        <StandardToggleField id='slider4' label='Label with status' {...toggleProps} showStatus />
      </SpaceBelow>
      <PrimaryButton type='submit' spacing='wide'>
        Submit
      </PrimaryButton>
    </form>
  )
}

export default {
  title: 'Views/SampleForm',
  component: SampleFormView,
} as Meta<typeof SampleFormView>

export const All = {
  args: {},
}
