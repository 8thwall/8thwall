import React from 'react'
import type {Meta} from '@storybook/react'

import {SpaceBetween} from '../layout/space-between'
import {useTypography} from '../typography'
import {combine} from '../../common/styles'
import AutoHeadingScope from '../../widgets/auto-heading-scope'
import AutoHeading from '../../widgets/auto-heading'
import {SecondaryButton} from '../components/secondary-button'
import {StaticBanner} from '../components/banner'
import type {HeadingLevel} from '../../widgets/generic-heading'

const SampleTypography: React.FC = () => {
  const typography = useTypography()

  const [autoLevel, setAutoLevel] = React.useState<HeadingLevel>(2)

  const renderLevelButton = (level: typeof autoLevel) => (
    <SecondaryButton
      height='tiny'
      disabled={autoLevel === level}
      aria-pressed={autoLevel === level}
      onClick={() => setAutoLevel(level)}
    >
      Level {level}
    </SecondaryButton>
  )

  return (
    <SpaceBetween direction='vertical' wide>
      <h1 className={typography.heading1}>Carrot cake jujubes<br />tiramisu cake.</h1>
      <h2 className={typography.heading2}>Carrot cake jujubes<br />tiramisu cake.</h2>
      <h3 className={typography.heading3}>Carrot cake jujubes<br />tiramisu cake.</h3>

      <p className={combine('style-reset', typography.subtitle)}>
        Carrot cake jujubes cotton candy  tiramisu cake.
      </p>

      <p className={combine('style-reset', typography.bodyMono)}>
        Carrot cake jujubes cotton candy  tiramisu cake.
      </p>

      <p className={combine('style-reset', typography.body)}>
        Carrot cake jujubes cotton candy croissant tiramisu<br />
        pastry oat cake marshmallow chocolate cake. <br />
        Macaroon dessert jelly sweet roll chupa chups tart <br />
        tart cookie icing.
      </p>

      <p className={combine('style-reset', typography.bodySmall)}>
        Carrot cake jujubes cotton candy croissant
      </p>

      <StaticBanner type='info'>
        Beyond the mock
      </StaticBanner>

      <p className={combine('style-reset', typography.bodyLarge)}>
        The AutoHeading component automatically chooses its heading level based on the current
        AutoHeadingScope. This allows for consistent heading levels without manually specifying.
      </p>

      <SpaceBetween direction='horizontal'>
        {renderLevelButton(1)}
        {renderLevelButton(2)}
        {renderLevelButton(3)}
        {renderLevelButton(4)}
      </SpaceBetween>

      <AutoHeadingScope level={autoLevel}>
        <AutoHeading className={typography.headingAuto}>
          Auto Heading Level {autoLevel}
        </AutoHeading>
        <AutoHeadingScope>
          <AutoHeading className={typography.headingAuto}>
            Auto Heading Level {autoLevel + 1}
          </AutoHeading>
        </AutoHeadingScope>
      </AutoHeadingScope>
    </SpaceBetween>
  )
}

export default {
  title: 'Views/Typography',
  component: SampleTypography,
} as Meta<typeof SampleTypography>

export const All = {
  args: {},
}
