import * as os from 'os'

// Try to figure out the most likely IP address a developer should use to hit this server.
const guessIp = (): string => {
  const ips = Object.entries(os.networkInterfaces())
    .map(([name, interfaces]) => interfaces.map(val => ({...val, name})))
    .flat()
    .sort((a, b) => {
      // Prefer IPv4 addresses
      if (a.family !== b.family) {
        return a.family === 'IPv4' ? -1 : 1
      }
      // Prefer non-internal addresses
      if (a.internal !== b.internal) {
        return a.internal ? 1 : -1
      }
      // Prefer interfaces like 'en0' to ones like 'eth0' or 'utun3'
      if (a.name.startsWith('en') !== b.name.startsWith('en')) {
        return a.name.startsWith('en') ? -1 : 1
      }
      // Otherwise, prefer alphabetically lower interface names.
      return a.name.localeCompare(b.name)
    })
  return ips.length ? ips[0].address : 'localhost'
}

export {guessIp}
