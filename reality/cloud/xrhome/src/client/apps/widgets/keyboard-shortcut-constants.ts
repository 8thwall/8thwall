import {isCloudStudioApp} from '../../../shared/app-utils'

const MACOS_SHORTCUTS_INDEX = 0
const WINDOWS_SHORTCUTS_INDEX = 1

const CLOUD_STUDIO_SHORTCUTS = {
  /* eslint-disable max-len, local-rules/hardcoded-copy */
  'project_settings_page.shortcut_binding.heading.cloud_studio': {
    'project_settings_page.shortcut_binding.action.handle_focus_object': ['F', 'F'],
    'project_settings_page.shortcut_binding.action.translate': ['W', 'W'],
    'project_settings_page.shortcut_binding.action.rotate': ['E', 'E'],
    'project_settings_page.shortcut_binding.action.scale': ['R', 'R'],
    'project_settings_page.shortcut_binding.action.transform_snap': ['⇧', '⇧'],
    'project_settings_page.shortcut_binding.action.deselect_current_entity': ['Esc', 'Esc'],
    'project_settings_page.shortcut_binding.action.show_hide_ui_layer': ['⌘-\\', 'Ctrl-\\'],
    'project_settings_page.shortcut_binding.action.delete_object': ['Delete', 'Delete'],
    'project_settings_page.shortcut_binding.action.duplicate': ['D', 'D'],
    'project_settings_page.shortcut_binding.action.copy_object': ['⌘-C', 'Ctrl-C'],
    'project_settings_page.shortcut_binding.action.paste_object': ['⌘-V', 'Ctrl-V'],
    'project_settings_page.shortcut_binding.action.undo': ['⌘-Z', 'Ctrl-Z'],
    'project_settings_page.shortcut_binding.action.redo': ['⌘-⇧-Z, ⌘-Y', 'Ctrl-⇧-Z, Ctrl-Y'],
    'project_settings_page.shortcut_binding.action.camera_orbit': ['⌥-Left Click+Drag', 'Alt-Left Click+Drag'],
    'project_settings_page.shortcut_binding.action.camera_pan': ['⌥-Right Click+Drag, Right Click+Drag, Middle Click+Drag', 'Alt-Right Click+Drag, Right Click+Drag, Middle Click+Drag'],
    'project_settings_page.shortcut_binding.action.camera_zoom': ['Scroll, ⌥-Scroll', 'Scroll, Alt-Scroll'],
  },
  /* eslint-enable max-len, local-rules/hardcoded-copy */
}

const getShortcutBindings = app => (
  {
    /* eslint-disable max-len, local-rules/hardcoded-copy */
    ace: {
      ...isCloudStudioApp(app) && {
        ...CLOUD_STUDIO_SHORTCUTS,
      },
      'project_settings_page.shortcut_binding.heading.system': {
        'project_settings_page.shortcut_binding.action.code_search': ['⌘-⇧-F', 'Ctrl-⇧-F'],
        'project_settings_page.shortcut_binding.action.save_build': ['⌘-S', 'Ctrl-S'],
        'project_settings_page.shortcut_binding.action.format_file': ['⌥-⇧-F', 'Alt-⇧-F'],
        'project_settings_page.shortcut_binding.action.find_file': ['⌘-P', 'Ctrl-P'],
        'project_settings_page.shortcut_binding.action.next_tab': ['⌥-Equal', '⌥-Equal'],
        'project_settings_page.shortcut_binding.action.previous_tab': ['⌥-Minus', '⌥-Minus'],
      },
      'project_settings_page.shortcut_binding.heading.line_operations': {
        'project_settings_page.shortcut_binding.action.remove_line': ['⌘-D', 'Ctrl-D'],
        'project_settings_page.shortcut_binding.action.copy_lines_down': ['⌘-⌥-Down', 'Alt-⇧-Down'],
        'project_settings_page.shortcut_binding.action.copy_lines_up': ['⌘-⌥-Up', 'Alt-⇧-Up'],
        'project_settings_page.shortcut_binding.action.move_lines_down': ['⌥-Down', 'Alt-Down'],
        'project_settings_page.shortcut_binding.action.move_lines_up': ['⌥-Up', 'Alt-Up'],
        'project_settings_page.shortcut_binding.action.remove_to_line_end': ['Ctrl-K', 'Alt-Delete'],
        'project_settings_page.shortcut_binding.action.remove_to_line_start':
      ['⌘-Backspace', 'Alt-Backspace'],
        'project_settings_page.shortcut_binding.action.remove_word_left':
      ['⌥-Backspace, Ctrl-⌥-Backspace', 'Ctrl-Backspace'],
        'project_settings_page.shortcut_binding.action.remove_word_right':
      ['⌥-Delete', 'Ctrl-Delete'],
        'project_settings_page.shortcut_binding.action.split_line': ['Ctrl-O', null],
      },
      'project_settings_page.shortcut_binding.heading.selection': {
        'project_settings_page.shortcut_binding.action.select_all': ['⌘-A', 'Ctrl-A'],
        'project_settings_page.shortcut_binding.action.select_left': ['⇧-Left', '⇧-Left'],
        'project_settings_page.shortcut_binding.action.select_right': ['⇧-Right', '⇧-Right'],
        'project_settings_page.shortcut_binding.action.select_word_left': ['⌥-⇧-Left', 'Ctrl-⇧-Left'],
        'project_settings_page.shortcut_binding.action.select_word_right':
      ['⌥-⇧-Right', 'Ctrl-⇧-Right'],
        'project_settings_page.shortcut_binding.action.select_line_start': ['⇧-Home', '⇧-Home'],
        'project_settings_page.shortcut_binding.action.select_line_end': ['⇧-End', '⇧-End'],
        'project_settings_page.shortcut_binding.action.select_to_line_end':
        ['⌘-⇧-Right', 'Alt-⇧-Right'],
        'project_settings_page.shortcut_binding.action.select_to_line_start':
        ['⌘-⇧-Left', 'Alt-⇧-Left'],
        'project_settings_page.shortcut_binding.action.select_up': ['⇧-Up', '⇧-Up'],
        'project_settings_page.shortcut_binding.action.select_down': ['⇧-Down', '⇧-Down'],
        'project_settings_page.shortcut_binding.action.select_page_up': ['⇧-PageUp', '⇧-PageUp'],
        'project_settings_page.shortcut_binding.action.select_page_down':
        ['⇧-PageDown', '⇧-PageDown'],
        'project_settings_page.shortcut_binding.action.select_to_start': ['⌘-⇧-Up', 'Ctrl-⇧-Home'],
        'project_settings_page.shortcut_binding.action.select_to_end': ['⌘-⇧-Down', 'Ctrl-⇧-End'],
        'project_settings_page.shortcut_binding.action.duplicate_selection': ['⌘-⇧-D', 'Ctrl-⇧-D'],
        'project_settings_page.shortcut_binding.action.select_to_match_bracket': [null, 'Ctrl-⇧-P'],
      },
      'project_settings_page.shortcut_binding.heading.multicursor': {
        'project_settings_page.shortcut_binding.action.add_multicursor_above':
        ['Ctrl-⌥-Up', 'Ctrl-Alt-Up'],
        'project_settings_page.shortcut_binding.action.add_multicursor_below':
        ['Ctrl-⌥-Down', 'Ctrl-Alt-Down'],
        'project_settings_page.shortcut_binding.action.add_next_to_multiselect':
        ['Ctrl-⌥-Right', 'Ctrl-Alt-Right'],
        'project_settings_page.shortcut_binding.action.add_prev_to_multiselect':
        ['Ctrl-⌥-Left', 'Ctrl-Alt-Left'],
        'project_settings_page.shortcut_binding.action.move_cursor_to_line_above':
        ['Ctrl-⌥-⇧-Up', 'Ctrl-Alt-⇧-Up'],
        'project_settings_page.shortcut_binding.action.move_cursor_to_line_below':
        ['Ctrl-⌥-⇧-Down', 'Ctrl-Alt-⇧-Down'],
        'project_settings_page.shortcut_binding.action.remove_current_from_multi_to_next':
        ['Ctrl-⌥-⇧-Right', 'Ctrl-Alt-⇧-Right'],
        'project_settings_page.shortcut_binding.action.remove_current_from_multi_to_prev':
        ['Ctrl-⌥-⇧-Left', 'Ctrl-Alt-⇧-Left'],
        'project_settings_page.shortcut_binding.action.select_all_multiselect':
        ['Ctrl-⇧-L', 'Ctrl-⇧-L'],
      },
      'project_settings_page.shortcut_binding.heading.go_to': {
        'project_settings_page.shortcut_binding.action.go_to_left': ['Left, Ctrl-B', 'Left'],
        'project_settings_page.shortcut_binding.action.go_to_right': ['Right, Ctrl-F', 'Right'],
        'project_settings_page.shortcut_binding.action.go_word_left': ['⌥-Left', 'Ctrl-Left'],
        'project_settings_page.shortcut_binding.action.go_word_right': ['⌥-Right', 'Ctrl-Right'],
        'project_settings_page.shortcut_binding.action.go_line_up': ['Up, Ctrl-P', 'Up'],
        'project_settings_page.shortcut_binding.action.go_line_down': ['Down, Ctrl-N', 'Down'],
        'project_settings_page.shortcut_binding.action.go_line_start':
        ['⌘-Left, Home, Ctrl-A', 'Alt-Left, Home'],
        'project_settings_page.shortcut_binding.action.go_line_end':
        ['⌘-Right, End, Ctrl-E', 'Alt-Right, End'],
        'project_settings_page.shortcut_binding.action.go_page_up': ['⌥-PageUp', 'PageUp'],
        'project_settings_page.shortcut_binding.action.go_page_down':
        ['⌥-PageDown, Ctrl-V', 'PageDown'],
        'project_settings_page.shortcut_binding.action.go_to_start': ['⌘-Home, ⌘-Up', 'Ctrl-Home'],
        'project_settings_page.shortcut_binding.action.go_to_end': ['⌘-End, ⌘-Down', 'Ctrl-End'],
        'project_settings_page.shortcut_binding.action.scroll_line_down': ['⌘-Down', 'Ctrl-Down'],
        'project_settings_page.shortcut_binding.action.scroll_line_up': [null, 'Ctrl-Up'],
        'project_settings_page.shortcut_binding.action.go_matching_bracket': [null, 'Ctrl-P'],
        'project_settings_page.shortcut_binding.action.scroll_page_down': ['⌥-PageDown', null],
        'project_settings_page.shortcut_binding.action.scroll_page_up': ['⌥-PageUp', null],
      },
      'project_settings_page.shortcut_binding.heading.find_replace': {
        'project_settings_page.shortcut_binding.action.find': ['⌘-F', 'Ctrl-F'],
        'project_settings_page.shortcut_binding.action.replace': ['⌘-⌥-F', 'Ctrl-H'],
        'project_settings_page.shortcut_binding.action.find_next': ['⌘-G', 'Ctrl-K'],
        'project_settings_page.shortcut_binding.action.find_previous': ['⌘-⇧-G', 'Ctrl-⇧-K'],
      },
      'project_settings_page.shortcut_binding.heading.folding': {
        'project_settings_page.shortcut_binding.action.fold_selection':
        ['⌘-⌥-L, ⌘-F1', 'Alt-L, Ctrl-F1'],
        'project_settings_page.shortcut_binding.action.unfold':
        ['⌘-⌥-⇧-L, ⌘-⇧-F1', 'Alt-⇧-L, Ctrl-⇧-F1'],
        'project_settings_page.shortcut_binding.action.fold_all': ['⌘-⌥-0', 'Alt-0'],
        'project_settings_page.shortcut_binding.action.unfold_all': ['⌘-⌥-⇧-0', 'Alt-⇧-0'],
      },
      'project_settings_page.shortcut_binding.heading.other': {
        'project_settings_page.shortcut_binding.action.indent': ['Tab', 'Tab'],
        'project_settings_page.shortcut_binding.action.outdent': ['⇧-Tab', '⇧-Tab'],
        'project_settings_page.shortcut_binding.action.undo': ['⌘-Z', 'Ctrl-Z'],
        'project_settings_page.shortcut_binding.action.redo': ['⌘-⇧-Z, ⌘-Y', 'Ctrl-⇧-Z, Ctrl-Y'],
        'project_settings_page.shortcut_binding.action.toggle_comment': ['⌘-/', 'Ctrl-/'],
        'project_settings_page.shortcut_binding.action.transpose_letters': ['Ctrl-T', 'Ctrl-T'],
        'project_settings_page.shortcut_binding.action.enter_fullscreen': ['⌘-Enter', 'Ctrl-Enter'],
        'project_settings_page.shortcut_binding.action.change_lowercase': ['Ctrl-⇧-U', 'Ctrl-⇧-U'],
        'project_settings_page.shortcut_binding.action.change_uppercase': ['Ctrl-U', 'Ctrl-U'],
        'project_settings_page.shortcut_binding.action.overwrite': ['Insert', 'Insert'],
        'project_settings_page.shortcut_binding.action.macros_replay': ['⌘-⇧-E', 'Ctrl-⇧-E'],
        'project_settings_page.shortcut_binding.action.macros_recording': [null, 'Ctrl-Alt-E'],
        'project_settings_page.shortcut_binding.action.delete': [null, 'Delete'],
        'project_settings_page.shortcut_binding.action.center_select': ['Ctrl-L', null],
      },
    },
    vscode: {
      ...isCloudStudioApp(app) && {
        ...CLOUD_STUDIO_SHORTCUTS,
      },
      'project_settings_page.shortcut_binding.heading.system': {
        'project_settings_page.shortcut_binding.action.code_search': ['⌘-⇧-F', 'Ctrl-⇧-F'],
        'project_settings_page.shortcut_binding.action.save_build': ['⌘-S', 'Ctrl-S'],
        'project_settings_page.shortcut_binding.action.format_file': ['⌥-⇧-F', 'Alt-⇧-F'],
        'project_settings_page.shortcut_binding.action.find_file': ['⌘-P', 'Ctrl-P'],
        'project_settings_page.shortcut_binding.action.next_tab': ['⌥-Equal', 'Alt-Equal'],
        'project_settings_page.shortcut_binding.action.previous_tab': ['⌥-Minus', 'Alt-Minus'],
      },
      'project_settings_page.shortcut_binding.heading.line_operations': {
        'project_settings_page.shortcut_binding.action.remove_line': ['⌘-Shift-K', 'Ctrl-Shift-K'],
        'project_settings_page.shortcut_binding.action.copy_lines_down':
        ['⌥-Shift-Down', 'Alt-Shift-Down'],
        'project_settings_page.shortcut_binding.action.copy_lines_up': ['⌥-Shift-Up', 'Alt-Shift-Up'],
        'project_settings_page.shortcut_binding.action.move_lines_down': ['⌥-Down', 'Alt-Down'],
        'project_settings_page.shortcut_binding.action.move_lines_up': ['⌥-Up', 'Alt-Up'],
        'project_settings_page.shortcut_binding.action.remove_to_line_end':
        ['⌘-Backspace', 'Alt-Backspace'],
        'project_settings_page.shortcut_binding.action.remove_to_line_start':
        ['⌘-Delete', 'Alt-Delete'],
        'project_settings_page.shortcut_binding.action.remove_word_left':
        ['⌥-Backspace, Ctrl-⌥-Backspace', 'Ctrl-Backspace'],
        'project_settings_page.shortcut_binding.action.remove_word_right':
        ['⌥-Delete', 'Ctrl-Delete'],
        'project_settings_page.shortcut_binding.action.split_line': ['Ctrl-O', null],
      },
      'project_settings_page.shortcut_binding.heading.selection': {
        'project_settings_page.shortcut_binding.action.select_all': ['⌘-A', 'Ctrl-A'],
        'project_settings_page.shortcut_binding.action.select_left': ['⇧-Left', '⇧-Left'],
        'project_settings_page.shortcut_binding.action.select_right': ['⇧-Right', '⇧-Right'],
        'project_settings_page.shortcut_binding.action.select_word_left': ['⌥-⇧-Left', 'Ctrl-⇧-Left'],
        'project_settings_page.shortcut_binding.action.select_word_right':
        ['⌥-⇧-Right', 'Ctrl-⇧-Right'],
        'project_settings_page.shortcut_binding.action.select_line_start': ['⇧-Home', '⇧-Home'],
        'project_settings_page.shortcut_binding.action.select_line_end': ['⇧-End', '⇧-End'],
        'project_settings_page.shortcut_binding.action.select_to_line_end':
        ['⌘-⇧-Right', 'Alt-⇧-Right'],
        'project_settings_page.shortcut_binding.action.select_to_line_start':
        ['⌘-⇧-Left', 'Alt-⇧-Left'],
        'project_settings_page.shortcut_binding.action.select_up': ['⇧-Up', '⇧-Up'],
        'project_settings_page.shortcut_binding.action.select_down': ['⇧-Down', '⇧-Down'],
        'project_settings_page.shortcut_binding.action.select_page_up': ['⇧-PageUp', '⇧-PageUp'],
        'project_settings_page.shortcut_binding.action.select_page_down':
        ['⇧-PageDown', '⇧-PageDown'],
        'project_settings_page.shortcut_binding.action.select_to_start': ['⌘-⇧-Up', 'Ctrl-⇧-Home'],
        'project_settings_page.shortcut_binding.action.select_to_end': ['⌘-⇧-Down', 'Ctrl-⇧-End'],
      },
      'project_settings_page.shortcut_binding.heading.multicursor': {
        'project_settings_page.shortcut_binding.action.add_multicursor_above':
        ['⌥-⌘-Up', 'Ctrl-Enter'],
        'project_settings_page.shortcut_binding.action.add_multicursor_below':
        ['⌥-⌘-Down', 'Ctrl-Shift-Enter'],
        'project_settings_page.shortcut_binding.action.add_next_to_multiselect': ['⌘-D', 'Ctrl-D'],
      },
      'project_settings_page.shortcut_binding.heading.go_to': {
        'project_settings_page.shortcut_binding.action.go_to_left': ['Left, Ctrl-B', 'Left'],
        'project_settings_page.shortcut_binding.action.go_to_right': ['Right, Ctrl-F', 'Right'],
        'project_settings_page.shortcut_binding.action.go_word_left': ['⌥-Left', 'Ctrl-Left'],
        'project_settings_page.shortcut_binding.action.go_word_right': ['⌥-Right', 'Ctrl-Right'],
        'project_settings_page.shortcut_binding.action.go_line_up': ['Up, Ctrl-P', 'Up'],
        'project_settings_page.shortcut_binding.action.go_line_down': ['Down, Ctrl-N', 'Down'],
        'project_settings_page.shortcut_binding.action.go_line_start':
        ['⌘-Left, Home, Ctrl-A', 'Alt-Left, Home'],
        'project_settings_page.shortcut_binding.action.go_line_end':
        ['⌘-Right, End, Ctrl-E', 'Alt-Right, End'],
        'project_settings_page.shortcut_binding.action.go_page_up': ['⌥-PageUp', 'PageUp'],
        'project_settings_page.shortcut_binding.action.go_page_down':
        ['⌥-PageDown, Ctrl-V', 'PageDown'],
        'project_settings_page.shortcut_binding.action.go_to_start': ['⌘-Home, ⌘-Up', 'Ctrl-Home'],
        'project_settings_page.shortcut_binding.action.go_to_end': ['⌘-End, ⌘-Down', 'Ctrl-End'],
        'project_settings_page.shortcut_binding.action.go_to_line': ['⌘-L', 'Ctrl-L'],
        'project_settings_page.shortcut_binding.action.scroll_line_down': ['⌘-Down', 'Ctrl-Down'],
        'project_settings_page.shortcut_binding.action.scroll_line_up': [null, 'Ctrl-Up'],
        'project_settings_page.shortcut_binding.action.go_matching_bracket': [null, 'Ctrl-P'],
        'project_settings_page.shortcut_binding.action.scroll_page_down': ['⌥-PageDown', null],
        'project_settings_page.shortcut_binding.action.scroll_page_up': ['⌥-PageUp', null],
      },
      'project_settings_page.shortcut_binding.heading.find_replace': {
        'project_settings_page.shortcut_binding.action.find': ['⌘-F', 'Ctrl-F'],
        'project_settings_page.shortcut_binding.action.replace': ['⌘-⌥-F', 'Ctrl-H'],
        'project_settings_page.shortcut_binding.action.find_next': ['⌘-G', 'Ctrl-K'],
        'project_settings_page.shortcut_binding.action.find_previous': ['⌘-⇧-G', 'Ctrl-⇧-K'],
      },
      'project_settings_page.shortcut_binding.heading.folding': {
        'project_settings_page.shortcut_binding.action.fold_selection': ['⌥-⌘-[, Ctrl-Shift-['],
        'project_settings_page.shortcut_binding.action.unfold': ['⌥-⌘-], Ctrl-Shift-]'],
        'project_settings_page.shortcut_binding.action.fold_all': ['⌘-K ⌘-0', '⌘-K Ctrl-['],
        'project_settings_page.shortcut_binding.action.unfold_all': ['⌘-K ⌘-J', '⌘-K Ctrl-]'],
      },
      'project_settings_page.shortcut_binding.heading.other': {
        'project_settings_page.shortcut_binding.action.indent': ['Tab', 'Tab'],
        'project_settings_page.shortcut_binding.action.outdent': ['⇧-Tab', '⇧-Tab'],
        'project_settings_page.shortcut_binding.action.undo': ['⌘-Z', 'Ctrl-Z'],
        'project_settings_page.shortcut_binding.action.redo': ['⌘-⇧-Z, ⌘-Y', 'Ctrl-⇧-Z, Ctrl-Y'],
        'project_settings_page.shortcut_binding.action.toggle_comment': ['⌘-/', 'Ctrl-/'],
        'project_settings_page.shortcut_binding.action.transpose_letters': ['Ctrl-T', 'Ctrl-T'],
        'project_settings_page.shortcut_binding.action.enter_fullscreen': ['⌘-Enter', 'Ctrl-Enter'],
        'project_settings_page.shortcut_binding.action.overwrite': ['Insert', 'Insert'],
        'project_settings_page.shortcut_binding.action.macros_replay': ['⌘-⇧-E', 'Ctrl-⇧-E'],
        'project_settings_page.shortcut_binding.action.macros_recording': [null, 'Ctrl-Alt-E'],
        'project_settings_page.shortcut_binding.action.delete': ['Delete', 'Delete'],
      },
    },
    /* eslint-enable max-len, local-rules/hardcoded-copy */
  })

export {
  MACOS_SHORTCUTS_INDEX,
  WINDOWS_SHORTCUTS_INDEX,
  CLOUD_STUDIO_SHORTCUTS,
  getShortcutBindings,
}
