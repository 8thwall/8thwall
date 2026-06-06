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

export {
  MACOS_SHORTCUTS_INDEX,
  WINDOWS_SHORTCUTS_INDEX,
  CLOUD_STUDIO_SHORTCUTS,
}
