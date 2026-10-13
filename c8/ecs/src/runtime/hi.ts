import * as ecs from '@8thwall/ecs'

ecs.registerComponent({
  name: 'space-selector',
  schema: {
    // @required
    activeSpaceElement: ecs.eid,
    // @required
    optionPrefab: ecs.eid,
    // @required
    labelPrefab: ecs.eid,
    // @required
    iconPrefab: ecs.eid,
    // @required
    containerElement: ecs.eid,
  },
  stateMachine: ({world, eid, schemaAttribute, defineState}) => {
    const {
      containerElement: containerElementEid,
      activeSpaceElement,
      optionPrefab,
      iconPrefab,
      labelPrefab,
    } = schemaAttribute.get(eid)

    const containerElement = world.getEntity(containerElementEid)

    // Clear placeholder elements
    containerElement.getChildren().forEach(c => c.delete())

    const options = world.spaces.listSpaces().filter(e => e.name !== 'Space Selector').map((space) => {
      // Spawn an option in the dropdown for each space
      const optionEid = world.createEntity(optionPrefab)
      const iconElement = world.getEntity(world.getInstanceEntity(optionEid, iconPrefab))
      ecs.Ui.set(world, world.getInstanceEntity(optionEid, labelPrefab), {text: space.name})
      containerElement.addChild(optionEid)

      return {
        space,
        stateName: space.id,
        optionEid,
        iconElement,
        trigger: ecs.defineTrigger(),
      }
    })

    containerElement.set(ecs.Ui, {height: String(options.length * 35)})

    const initialSpaceIndex = Math.max(
      0,
      options.findIndex(e => e.space.id == world.spaces.getActiveSpace().id)
    )

    options.forEach(({space, stateName, iconElement}, index) => {
      const state = defineState(stateName).onEnter(() => {
        if (world.spaces.getActiveSpace().id !== space.id) {
          world.spaces.loadSpace(space.id)
        }
        ecs.Ui.set(world, activeSpaceElement, {text: space.name})
        iconElement.show()
      })
        .onExit(() => {
          iconElement.hide()
        }).listen(world.events.globalId, ecs.CameraEvents.ACTIVE_CAMERA_CHANGE, () => {
          // After a space changes, this event will trigger a transition to the newly active space.
          const newActiveSpace = world.spaces.getActiveSpace().id
          options.find(e => e.space.id === newActiveSpace)?.trigger.trigger()
        })

      options.forEach((other, otherIndex) => {
        if (index !== otherIndex) {
          // Clicking any other option will transition to that state
          state.onEvent(ecs.input.UI_CLICK, other.stateName, {target: other.optionEid})
          state.onTrigger(other.trigger, other.stateName)
        }
      })

      if (initialSpaceIndex === index) {
        state.initial()
      }
    })
  },
})
