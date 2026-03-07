// @package(npm-ecs)
// @attr(externalize_npm = 1)
import type {World} from './world'

// NOTE(christoph): This test ensures there are no undeclared dependencies within the world types.
// If it fails to build, it means that the world types are not self-contained.

const w: World | undefined = undefined

// eslint-disable-next-line no-console
console.log(w)
