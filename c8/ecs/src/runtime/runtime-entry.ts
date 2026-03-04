// @rule(js_binary)
// @name(runtime)
// @package(npm-ecs)
// @attr(esnext = 1)

import './set-three'

import ecs from './index'

// When loaded as a script tag, make the runtime available as window.ecs

import {application} from './application'
import {recordCurrentScriptBase} from '../shared/resources'

recordCurrentScriptBase()

Object.assign(window, {ecs})

Object.assign(ecs, {application})
