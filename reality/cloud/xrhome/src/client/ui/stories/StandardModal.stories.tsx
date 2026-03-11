import React from 'react'
import type {Meta} from '@storybook/react'

import {ModalWidth, StandardModal} from '../components/standard-modal'
import {StandardModalHeader} from '../components/standard-modal-header'
import {StandardModalContent} from '../components/standard-modal-content'
import {StandardModalActions} from '../components/standard-modal-actions'
import {PrimaryButton} from '../components/primary-button'
import {SecondaryButton} from '../components/secondary-button'
import {StandardDropdownField} from '../components/standard-dropdown-field'
import {SpaceBetween} from '../layout/space-between'
import {StandardCheckboxField} from '../components/standard-checkbox-field'
import {useTypography} from '../typography'

const SampleControlledModal = () => {
  const typography = useTypography()
  const [visible, setVisible] = React.useState(false)
  return (
    <div>
      <SecondaryButton onClick={() => setVisible(true)}>
        Open Controlled Modal
      </SecondaryButton>
      {visible &&
        <StandardModal
          trigger='render'
          width='narrow'
          onOpenChange={setVisible}
        >
          {collapse => (
            <>
              <StandardModalHeader>
                <h2 className={typography.standardModalTitle}>Controlled Modal Header</h2>
              </StandardModalHeader>
              <StandardModalContent scroll>
                Hello there!
              </StandardModalContent>
              <StandardModalActions>
                <PrimaryButton
                  onClick={collapse}
                >
                  Button
                </PrimaryButton>
              </StandardModalActions>
            </>
          )}
        </StandardModal>
     }
    </div>
  )
}

const SampleStandardModal: React.FC = () => {
  const typography = useTypography()
  const [scroll, setScroll] = React.useState(false)
  const [contentLength, setContentLength] = React.useState<'short' | 'long'>('short')
  const [width, setWidth] = React.useState<ModalWidth>('narrow')

  return (
    <SpaceBetween direction='vertical'>
      <StandardModal
        width={width}
        trigger={(
          <PrimaryButton>
            Open Modal
          </PrimaryButton>
        )}
      >
        {collapse => (
          <div>
            <StandardModalHeader>
              <h2 className={typography.standardModalTitle}>Sample Modal Header</h2>
            </StandardModalHeader>
            <StandardModalContent scroll={scroll}>
              <SpaceBetween direction='vertical'>
                <StandardDropdownField
                  label='Modal Width'
                  value={width}
                  options={[
                    {value: 'narrow', content: 'Narrow'},
                    {value: 'unset', content: 'Unset'},
                  ]}
                  onChange={value => setWidth(value as ModalWidth)}
                />
                <StandardDropdownField
                  label='Content Length'
                  value={contentLength}
                  options={[
                    {value: 'short', content: 'Short'},
                    {value: 'long', content: 'Long'},
                  ]}
                  onChange={value => setContentLength(value as 'short' | 'long')}
                />
                <StandardCheckboxField
                  label='Scroll'
                  checked={scroll}
                  onChange={e => setScroll(e.target.checked)}
                />
                {contentLength === 'short'
                  ? (
                    <p>
                      Add more details here: Jelly donut jelly topping. Fruitcake soufflé pie
                      halvah toffee tiramisu gummies croissant and caramels.
                    </p>
                  )
                  : (
                    <>
                      <p>
                        Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque
                        maximus sed metus non suscipit. Praesent aliquet, sapien vitae viverra
                        elementum, orci risus convallis est, ac gravida ligula nisl nec lectus.
                        Pellentesque a diam cursus, aliquam nisl ac, commodo risus. Mauris et
                        tortor sit amet tortor feugiat aliquam ut sed neque. Etiam tempor facilisis
                        velit, sed dapibus sem blandit vitae. Nam a nibh euismod, ullamcorper mauris
                        venenatis, convallis enim. Phasellus at nisi ante. Vivamus vel faucibus dui.
                        Duis et vehicula nunc. In vestibulum nisl vel dui egestas ultricies. Aliquam
                        lacinia convallis dapibus.
                      </p>
                      <p>
                        Sed at nulla dictum, malesuada lorem eu, pulvinar magna. Nulla facilisi.
                        Phasellus at semper nisi. Aliquam tempor varius tortor et viverra. Integer
                        in sodales mauris. Sed nulla arcu, finibus et pulvinar in, cursus ac velit.
                        Phasellus dapibus luctus erat vitae venenatis. Vestibulum suscipit, est
                        maximus faucibus dignissim, nulla risus sodales enim, at aliquet justo
                        nunc vitae purus.
                      </p>
                      <p>
                        Nulla tellus lacus, viverra et ipsum nec, semper pretium neque. Proin
                        viverra augue ac turpis tristique porta. Sed tincidunt nisi id luctus
                        euismod. Ut vehicula nec dolor vel convallis. Fusce blandit elit eu lectus
                        iaculis, at dignissim est pharetra. Morbi vel mattis nibh, et tincidunt
                        velit. Ut lacinia odio augue, a aliquam nibh elementum vitae. Nunc ultricies
                        massa efficitur porttitor congue. Quisque ornare elementum sapien vitae
                        aliquet. Phasellus ac mauris non neque tristique laoreet. Mauris ullamcorper
                        sodales ipsum et ultricies. Praesent nunc metus, lacinia a commodo non,
                        laoreet id ex. Cras dapibus, odio et congue lobortis, velit ante
                        pellentesque nibh, pretium rutrum lorem lectus sit amet dolor. Nam et
                        dignissim lectus. Etiam tempus tortor eget hendrerit congue.
                      </p>
                      <p>
                        In laoreet turpis sed tempus dapibus. Fusce semper velit sapien. Integer
                        malesuada, metus eu facilisis pulvinar, diam nibh convallis est, vel
                        imperdiet nisl augue eu purus. Donec commodo porta tincidunt. Vestibulum
                        eget egestas orci. Curabitur sit amet ante pharetra, consequat lacus
                        vitae, ornare libero. Phasellus id tincidunt risus. Aliquam dignissim
                        cursus lectus, id ullamcorper nibh ullamcorper ac. Maecenas ultrices
                        gravida justo, vel posuere neque tristique et.
                      </p>
                      <p>
                        Etiam ac rhoncus nunc. Aenean quis tempor enim. Fusce vulputate ultrices
                        nisl sit amet tristique. Suspendisse potenti. Nunc pretium aliquam lacus
                        vitae lobortis. Donec euismod nunc felis, sed aliquet felis tincidunt quis.
                        Nullam in scelerisque justo. Vestibulum ac elit id neque lobortis
                        pellentesque. Ut nibh libero, lobortis at odio ac, tristique rhoncus nulla.
                        Donec sollicitudin congue sem, at sollicitudin risus tristique sed. In ut
                        facilisis ante, a porttitor ligula. Vestibulum consectetur commodo ante ac
                        sollicitudin. Sed eget dolor consectetur, pharetra nisl vitae, lobortis
                        augue. Phasellus maximus mauris erat, sit amet lacinia leo fermentum nec.
                      </p>
                    </>
                  )}
              </SpaceBetween>
            </StandardModalContent>
            <StandardModalActions>
              <SecondaryButton>
                Secondary
              </SecondaryButton>
              <PrimaryButton
                onClick={collapse}
              >
                Button
              </PrimaryButton>
            </StandardModalActions>
          </div>
        )}
      </StandardModal>
      <SampleControlledModal />
    </SpaceBetween>
  )
}

export default {
  title: 'Views/StandardModal',
  component: SampleStandardModal,
} as Meta<typeof SampleStandardModal>

export const All = {
  args: {},
}
