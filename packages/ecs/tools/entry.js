if (!window.ecs) {
  console.warn('ECS not present on window, missing runtime.js script tag?')
}

module.exports = window.ecs
