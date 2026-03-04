// @rule(js_binary)
// @name(plugin)
// @package(npm-ecs)
// @attr(esnext = 1)

import {recordCurrentScriptBase} from '../shared/resources'
import ecs from './index'

recordCurrentScriptBase()

// When loaded as a script tag, make the runtime available as window.ecs
Object.assign(window, {ecs})
