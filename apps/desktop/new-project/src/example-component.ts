import * as ecs from '@8thwall/ecs'

ecs.registerComponent({
  name: 'example-component',
  add: () => {
    console.log('Component attached.')
  },
})
