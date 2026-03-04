const DEFAULT_ACTION_MAPS = {
  'fly-controller': [
    {
      'name': 'lookRight',
      'bindings': [
        {
          'input': 'gamepad:axis:right:right',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:right',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'lookLeft',
      'bindings': [
        {
          'input': 'gamepad:axis:right:left',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:left',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'lookUp',
      'bindings': [
        {
          'input': 'gamepad:axis:right:up',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:up',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'lookDown',
      'bindings': [
        {
          'input': 'gamepad:axis:right:down',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:down',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'up',
      'bindings': [
        {
          'input': 'keyboard:KeyQ',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:0',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'down',
      'bindings': [
        {
          'input': 'keyboard:KeyE',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:1',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'backward',
      'bindings': [
        {
          'input': 'keyboard:KeyS',
          'modifiers': [],
        },
        {
          'input': 'gamepad:axis:left:down',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:13',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowDown',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'left',
      'bindings': [
        {
          'input': 'keyboard:KeyA',
          'modifiers': [],
        },
        {
          'input': 'gamepad:axis:left:left',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:14',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowLeft',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'right',
      'bindings': [
        {
          'input': 'keyboard:KeyD',
          'modifiers': [],
        },
        {
          'input': 'gamepad:axis:left:right',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:15',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowRight',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'forward',
      'bindings': [
        {
          'input': 'keyboard:KeyW',
          'modifiers': [],
        },
        {
          'input': 'gamepad:axis:left:up',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:12',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowUp',
          'modifiers': [],
        },
      ],
    },
  ],
  'orbit-controls': [
    {
      'name': 'lookUp',
      'bindings': [
        {
          'input': 'gamepad:axis:left:up',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:12',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowUp',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:up',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'lookRight',
      'bindings': [
        {
          'input': 'gamepad:axis:left:right',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:15',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowRight',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:right',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'lookLeft',
      'bindings': [
        {
          'input': 'gamepad:axis:left:left',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:14',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowLeft',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:left',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'lookDown',
      'bindings': [
        {
          'input': 'gamepad:axis:left:down',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:13',
          'modifiers': [],
        },
        {
          'input': 'keyboard:ArrowDown',
          'modifiers': [],
        },
        {
          'input': 'mouse:move:down',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'zoomOut',
      'bindings': [
        {
          'input': 'gamepad:axis:right:down',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:5',
          'modifiers': [],
        },
        {
          'input': 'mouse:scroll:down',
          'modifiers': [],
        },
      ],
    },
    {
      'name': 'zoomIn',
      'bindings': [
        {
          'input': 'gamepad:axis:right:up',
          'modifiers': [],
        },
        {
          'input': 'gamepad:button:7',
          'modifiers': [],
        },
        {
          'input': 'mouse:scroll:up',
          'modifiers': [],
        },
      ],
    },
  ],
}

const DEFAULT_ACTION_MAP = 'default'

export {
  DEFAULT_ACTION_MAPS,
  DEFAULT_ACTION_MAP,
}
