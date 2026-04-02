import type {DeepReadonly} from 'ts-essentials'

// Defined in:
// https://github.com/bazelbuild/buildtools/blob/master/tables/tables.go#L177
const NAME_PRIORITY: DeepReadonly<Record<string, number>> = {
  'name': -99,
  'archive_override.module_name': -99,
  'git_override.module_name': -99,
  'local_path_override.module_name': -99,
  'multiple_version_override.module_name': -99,
  'single_version_override.module_name': -99,
  'bazel_dep.version': -98,
  'module.version': -98,
  'gwt_name': -98,
  'package_name': -97,
  'visible_node_name': -96,
  'size': -95,
  'timeout': -94,
  'testonly': -93,
  'src': -92,
  'srcdir': -91,
  'srcs': -90,
  'out': -89,
  'outs': -88,
  'hdrs': -87,
  'has_services': -86,
  'include': -85,
  'of': -84,
  'baseline': -83,
  // All others sort here, at 0.
  'destdir': 1,
  'exports': 2,
  'runtime_deps': 3,
  'deps': 4,
  'implementation': 5,
  'implements': 6,
  'alwayslink': 7,
}

export {
  NAME_PRIORITY,
}
