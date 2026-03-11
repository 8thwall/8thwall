import React from 'react'

import {UniversalPublishModal} from './modals/universal-publish-modal'

type IPublishButton = {
  renderButton: (props: {disabled: boolean}) => React.ReactElement
}

const PublishButton: React.FC<IPublishButton> = ({
  renderButton,
}) => (
  <UniversalPublishModal
    trigger={renderButton({disabled: false})}
  />
)

export {
  PublishButton,
}
