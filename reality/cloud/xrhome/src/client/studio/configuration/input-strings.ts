import type {SubMenuCategory, SubMenuItem} from '../ui/submenu'

const GAMEPAD_FACE_BUTTON_INPUTS = [
  {
    content: 'input_strings.option.gamepad_face_buttons.south',
    value: 'gamepad:button:0',
    icon: 'faceButtonDown',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_face_buttons.east',
    value: 'gamepad:button:1',
    icon: 'faceButtonRight',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_face_buttons.west',
    value: 'gamepad:button:2',
    icon: 'faceButtonLeft',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_face_buttons.north',
    value: 'gamepad:button:3',
    icon: 'faceButtonUp',
    ns: 'cloud-studio-pages',
  },
] as const

const GAMEPAD_D_PAD_INPUTS = [
  {
    content: 'input_strings.option.gamepad_dpad_buttons.up',
    value: 'gamepad:button:12',
    icon: 'dPadUp',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_dpad_buttons.down',
    value: 'gamepad:button:13',
    icon: 'dPadDown',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_dpad_buttons.left',
    value: 'gamepad:button:14',
    icon: 'dPadLeft',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_dpad_buttons.right',
    value: 'gamepad:button:15',
    icon: 'dPadRight',
    ns: 'cloud-studio-pages',
  },
] as const

const GAMEPAD_LEFT_STICK_INPUTS = [
  {
    content: 'input_strings.option.gamepad_left_joystick.up',
    value: 'gamepad:axis:left:up',
    icon: 'dPadUp',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_left_joystick.down',
    value: 'gamepad:axis:left:down',
    icon: 'dPadDown',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_left_joystick.left',
    value: 'gamepad:axis:left:left',
    icon: 'dPadLeft',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_left_joystick.right',
    value: 'gamepad:axis:left:right',
    icon: 'dPadRight',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_left_joystick.button',
    value: 'gamepad:button:10',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const GAMEPAD_RIGHT_STICK_INPUTS = [
  {
    content: 'input_strings.option.gamepad_right_joystick.up',
    value: 'gamepad:axis:right:up',
    icon: 'dPadUp',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_right_joystick.down',
    value: 'gamepad:axis:right:down',
    icon: 'dPadDown',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_right_joystick.left',
    value: 'gamepad:axis:right:left',
    icon: 'dPadLeft',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_right_joystick.right',
    value: 'gamepad:axis:right:right',
    icon: 'dPadRight',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_right_joystick.button',
    value: 'gamepad:button:11',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const GAMEPAD_NONE_INPUTS = [
  {
    content: 'input_strings.option.gamepad_left_shoulder',
    value: 'gamepad:button:4',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_right_shoulder',
    value: 'gamepad:button:5',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_left_trigger',
    value: 'gamepad:button:6',
    icon: 'triggerButton',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_right_trigger',
    value: 'gamepad:button:7',
    icon: 'triggerButton',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_select_button',
    value: 'gamepad:button:8',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_start_button',
    value: 'gamepad:button:9',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_home_button',
    value: 'gamepad:button:16',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.gamepad_aux_button',
    value: 'gamepad:button:17',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const KEYBOARD_LETTER_INPUTS = [
  {content: 'A', value: 'keyboard:KeyA', icon: 'keyPress'},
  {content: 'B', value: 'keyboard:KeyB', icon: 'keyPress'},
  {content: 'C', value: 'keyboard:KeyC', icon: 'keyPress'},
  {content: 'D', value: 'keyboard:KeyD', icon: 'keyPress'},
  {content: 'E', value: 'keyboard:KeyE', icon: 'keyPress'},
  {content: 'F', value: 'keyboard:KeyF', icon: 'keyPress'},
  {content: 'G', value: 'keyboard:KeyG', icon: 'keyPress'},
  {content: 'H', value: 'keyboard:KeyH', icon: 'keyPress'},
  {content: 'I', value: 'keyboard:KeyI', icon: 'keyPress'},
  {content: 'J', value: 'keyboard:KeyJ', icon: 'keyPress'},
  {content: 'K', value: 'keyboard:KeyK', icon: 'keyPress'},
  {content: 'L', value: 'keyboard:KeyL', icon: 'keyPress'},
  {content: 'M', value: 'keyboard:KeyM', icon: 'keyPress'},
  {content: 'N', value: 'keyboard:KeyN', icon: 'keyPress'},
  {content: 'O', value: 'keyboard:KeyO', icon: 'keyPress'},
  {content: 'P', value: 'keyboard:KeyP', icon: 'keyPress'},
  {content: 'Q', value: 'keyboard:KeyQ', icon: 'keyPress'},
  {content: 'R', value: 'keyboard:KeyR', icon: 'keyPress'},
  {content: 'S', value: 'keyboard:KeyS', icon: 'keyPress'},
  {content: 'T', value: 'keyboard:KeyT', icon: 'keyPress'},
  {content: 'U', value: 'keyboard:KeyU', icon: 'keyPress'},
  {content: 'V', value: 'keyboard:KeyV', icon: 'keyPress'},
  {content: 'W', value: 'keyboard:KeyW', icon: 'keyPress'},
  {content: 'X', value: 'keyboard:KeyX', icon: 'keyPress'},
  {content: 'Y', value: 'keyboard:KeyY', icon: 'keyPress'},
  {content: 'Z', value: 'keyboard:KeyZ', icon: 'keyPress'},
] as const

const KEYBOARD_NUMBER_INPUTS = [
  {content: '1', value: 'keyboard:Digit1', icon: 'keyPress'},
  {content: '2', value: 'keyboard:Digit2', icon: 'keyPress'},
  {content: '3', value: 'keyboard:Digit3', icon: 'keyPress'},
  {content: '4', value: 'keyboard:Digit4', icon: 'keyPress'},
  {content: '5', value: 'keyboard:Digit5', icon: 'keyPress'},
  {content: '6', value: 'keyboard:Digit6', icon: 'keyPress'},
  {content: '7', value: 'keyboard:Digit7', icon: 'keyPress'},
  {content: '8', value: 'keyboard:Digit8', icon: 'keyPress'},
  {content: '9', value: 'keyboard:Digit9', icon: 'keyPress'},
  {content: '0', value: 'keyboard:Digit0', icon: 'keyPress'},
] as const

const KEYBOARD_MODIFIER_INPUTS = [
  {
    content: 'input_strings.option.keyboard_modifier.left_cmd',
    value: 'keyboard:MetaLeft',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_modifier.right_cmd',
    value: 'keyboard:MetaRight',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_modifier.left_option',
    value: 'keyboard:AltLeft',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_modifier.right_option',
    value: 'keyboard:AltRight',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_modifier.left_shift',
    value: 'keyboard:ShiftLeft',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_modifier.right_shift',
    value: 'keyboard:ShiftRight',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_modifier.left_ctrl',
    value: 'keyboard:ControlLeft',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_modifier.right_ctrl',
    value: 'keyboard:ControlRight',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const KEYBOARD_SYMBOL_INPUTS = [
  {
    content: 'input_strings.option.keyboard_symbol.minus',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Minus',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.equal',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Equal',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.left_bracket',
    value: 'keyboard:BracketLeft',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.right_bracket',
    value: 'keyboard:BracketRight',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.backslash',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Backslash',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.slash',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Slash',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.semicolon',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Semicolon',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.apostrophe',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Quote',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.backquote',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Backquote',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.comma',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Comma',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_symbol.period',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Period',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const KEYBOARD_ARROW_INPUTS = [
  {
    content: 'input_strings.option.keyboard_arrow.up',
    value: 'keyboard:ArrowUp',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_arrow.down',
    value: 'keyboard:ArrowDown',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_arrow.right',
    value: 'keyboard:ArrowRight',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_arrow.left',
    value: 'keyboard:ArrowLeft',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const KEYBOARD_FUNCTION_INPUTS = [
  {content: 'F1', value: 'keyboard:F1', icon: 'keyPress'},
  {content: 'F2', value: 'keyboard:F2', icon: 'keyPress'},
  {content: 'F3', value: 'keyboard:F3', icon: 'keyPress'},
  {content: 'F4', value: 'keyboard:F4', icon: 'keyPress'},
  {content: 'F5', value: 'keyboard:F5', icon: 'keyPress'},
  {content: 'F6', value: 'keyboard:F6', icon: 'keyPress'},
  {content: 'F7', value: 'keyboard:F7', icon: 'keyPress'},
  {content: 'F8', value: 'keyboard:F8', icon: 'keyPress'},
  {content: 'F9', value: 'keyboard:F9', icon: 'keyPress'},
  {content: 'F10', value: 'keyboard:F10', icon: 'keyPress'},
  {content: 'F11', value: 'keyboard:F11', icon: 'keyPress'},
  {content: 'F12', value: 'keyboard:F12', icon: 'keyPress'},
] as const

const KEYBOARD_NONE_INPUTS = [
  {
    content: 'input_strings.option.keybaord_none.enter',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Enter',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_none.escape',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Escape',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_none.backspace',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Backspace',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_none.tab',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Tab',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.keyboard_none.space',
    /* eslint-disable-next-line local-rules/hardcoded-copy */
    value: 'keyboard:Space',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const MOUSE_INPUTS = [
  {
    content: 'input_strings.option.mouse.left_click',
    value: 'mouse:button:0',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.middle_click',
    value: 'mouse:button:1',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.right_click',
    value: 'mouse:button:2',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.back_click',
    value: 'mouse:button:3',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.forward_click',
    value: 'mouse:button:4',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.scroll_up',
    value: 'mouse:scroll:up',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.scroll_down',
    value: 'mouse:scroll:down',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.scroll_left',
    value: 'mouse:scroll:left',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.scroll_right',
    value: 'mouse:scroll:right',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.move_up',
    value: 'mouse:move:up',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.move_down',
    value: 'mouse:move:down',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.move_left',
    value: 'mouse:move:left',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_strings.option.mouse.move_right',
    value: 'mouse:move:right',
    icon: 'keyPress',
    ns: 'cloud-studio-pages',
  },
] as const

const TOUCHSCREEN_INPUTS = [
  {
    content: 'input_strings.option.touchscreen.touched',
    value: 'touch',
    icon: 'touch',
    ns: 'cloud-studio-pages',
  },
] as const

const ALL_INPUTS = [
  ...GAMEPAD_FACE_BUTTON_INPUTS,
  ...GAMEPAD_D_PAD_INPUTS,
  ...GAMEPAD_LEFT_STICK_INPUTS,
  ...GAMEPAD_RIGHT_STICK_INPUTS,
  ...GAMEPAD_NONE_INPUTS,
  ...KEYBOARD_LETTER_INPUTS,
  ...KEYBOARD_NUMBER_INPUTS,
  ...KEYBOARD_MODIFIER_INPUTS,
  ...KEYBOARD_SYMBOL_INPUTS,
  ...KEYBOARD_ARROW_INPUTS,
  ...KEYBOARD_FUNCTION_INPUTS,
  ...KEYBOARD_NONE_INPUTS,
  ...MOUSE_INPUTS,
  ...TOUCHSCREEN_INPUTS,
] as SubMenuItem[]

enum InputCategoryType {
  KEYBOARD = 'input_strings.category.keyboard',
  GAMEPAD = 'input_strings.category.gamepad',
  MOUSE = 'input_strings.category.mouse',
  GAMEPAD_DPAD = 'input_strings.category.gamepad_dpad',
  GAMEPAD_FACE = 'input_strings.category.gamepad_face',
  GAMEPAD_LEFT_STICK = 'input_strings.category.gamepad_left_stick',
  GAMEPAD_RIGHT_STICK = 'input_strings.category.gamepad_right_stick',
  KEYBOARD_LETTERS = 'input_strings.category.keyboard_letters',
  KEYBOARD_NUMBERS = 'input_strings.category.keyboard_numbers',
  KEYBOARD_MODIFIERS = 'input_strings.category.keyboard_modifiers',
  KEYBOARD_SYMBOLS = 'input_strings.category.keyboard_symbols',
  KEYBOARD_ARROWS = 'input_strings.category.keyboard_arrows',
  KEYBOARD_FUNCTIONS = 'input_strings.category.keyboard_functions',
  TOUCHSCREEN = 'input_strings.category.touchscreen',
}

const ALL_INPUT_CATEGORIES = [
  {
    value: InputCategoryType.GAMEPAD,
    parent: null,
    options: GAMEPAD_NONE_INPUTS,
  },
  {
    value: InputCategoryType.KEYBOARD,
    parent: null,
    options: KEYBOARD_NONE_INPUTS,
  },
  {
    value: InputCategoryType.MOUSE,
    parent: null,
    options: MOUSE_INPUTS,
  },
  {
    value: InputCategoryType.TOUCHSCREEN,
    parent: null,
    options: TOUCHSCREEN_INPUTS,
  },
  {
    value: InputCategoryType.GAMEPAD_DPAD,
    parent: InputCategoryType.GAMEPAD,
    options: GAMEPAD_D_PAD_INPUTS,
  },
  {
    value: InputCategoryType.GAMEPAD_FACE,
    parent: InputCategoryType.GAMEPAD,
    options: GAMEPAD_FACE_BUTTON_INPUTS,
  },
  {
    value: InputCategoryType.GAMEPAD_LEFT_STICK,
    parent: InputCategoryType.GAMEPAD,
    options: GAMEPAD_LEFT_STICK_INPUTS,
  },
  {
    value: InputCategoryType.GAMEPAD_RIGHT_STICK,
    parent: InputCategoryType.GAMEPAD,
    options: GAMEPAD_RIGHT_STICK_INPUTS,
  },
  {
    value: InputCategoryType.KEYBOARD_LETTERS,
    parent: InputCategoryType.KEYBOARD,
    options: KEYBOARD_LETTER_INPUTS,
  },
  {
    value: InputCategoryType.KEYBOARD_NUMBERS,
    parent: InputCategoryType.KEYBOARD,
    options: KEYBOARD_NUMBER_INPUTS,
  },
  {
    value: InputCategoryType.KEYBOARD_MODIFIERS,
    parent: InputCategoryType.KEYBOARD,
    options: KEYBOARD_MODIFIER_INPUTS,
  },
  {
    value: InputCategoryType.KEYBOARD_SYMBOLS,
    parent: InputCategoryType.KEYBOARD,
    options: KEYBOARD_SYMBOL_INPUTS,
  },
  {
    value: InputCategoryType.KEYBOARD_ARROWS,
    parent: InputCategoryType.KEYBOARD,
    options: KEYBOARD_ARROW_INPUTS,
  },
  {
    value: InputCategoryType.KEYBOARD_FUNCTIONS,
    parent: InputCategoryType.KEYBOARD,
    options: KEYBOARD_FUNCTION_INPUTS,
  },
] as SubMenuCategory[]

const validateInput = (input: string) => ALL_INPUTS.find(option => option.value === input)

export {
  GAMEPAD_FACE_BUTTON_INPUTS,
  GAMEPAD_D_PAD_INPUTS,
  GAMEPAD_LEFT_STICK_INPUTS,
  GAMEPAD_RIGHT_STICK_INPUTS,
  GAMEPAD_NONE_INPUTS,
  KEYBOARD_LETTER_INPUTS,
  KEYBOARD_NUMBER_INPUTS,
  KEYBOARD_MODIFIER_INPUTS,
  KEYBOARD_SYMBOL_INPUTS,
  KEYBOARD_ARROW_INPUTS,
  KEYBOARD_FUNCTION_INPUTS,
  KEYBOARD_NONE_INPUTS,
  MOUSE_INPUTS,
  ALL_INPUTS,
  ALL_INPUT_CATEGORIES,
  validateInput,
}
