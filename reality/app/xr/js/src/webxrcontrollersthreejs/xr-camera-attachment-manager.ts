const CameraAttachmentManagerFactory = ({controllers, camera}) => {
  const controllers_ = controllers
  const camera_ = camera
  const oldAttachmentGroup_ = camera_.el ? camera_.parent : camera_
  let newAttachmentGroup_ = null

  const getCamAttachedChildren = () => newAttachmentGroup_

  const moveToController = () => {
    if (!newAttachmentGroup_) {
      newAttachmentGroup_ = new window.THREE.Group()
    }
    oldAttachmentGroup_.children.forEach((c) => {
      if (c.uuid !== camera_.uuid) {
        newAttachmentGroup_.add(c)
      }
    })
    controllers_.left.controller.add(newAttachmentGroup_)
  }

  const reset = () => {
    if (newAttachmentGroup_) {
      if (newAttachmentGroup_.parent === controllers_.left.controller) {
        newAttachmentGroup_.children.forEach((c) => { oldAttachmentGroup_.add(c) })
        controllers_.left.controller.remove(newAttachmentGroup_)
        newAttachmentGroup_ = null
      }
    }
  }

  return {
    moveToController,
    reset,
    getCamAttachedChildren,
  }
}

export {
  CameraAttachmentManagerFactory,
}
