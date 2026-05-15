import {createServer} from 'http'
import {createProxyServer, proxyUpgrade} from 'httpxy'
import log from 'electron-log'

type LocalProxyOptions = {
  primaryPort: number
  buildPort: number
  dev8SocketPort: number
}

const startLocalProxy = ({primaryPort, buildPort, dev8SocketPort}: LocalProxyOptions) => {
  const proxy = createProxyServer({
    target: `http://localhost:${buildPort}`,
    changeOrigin: true,
  })

  const server = createServer((req, res) => proxy.web(req, res))

  server.on('upgrade', (req, socket, head) => {
    if (req.url?.startsWith('/dev8')) {
      proxyUpgrade(`http://localhost:${dev8SocketPort}`, req, socket, head)
      return
    }

    proxyUpgrade(`http://localhost:${buildPort}${req.url}`, req, socket, head, {
      headers: {Host: 'https://localhost', Origin: 'https://localhost'},
    })
  })

  server.listen(primaryPort, () => {
    log.info(`Proxy is listening on http://localhost:${primaryPort}`)
    log.info(`Base server is http://localhost:${buildPort}`)
  })

  return {
    stop: () => {
      server.close()
    },
  }
}

export {
  startLocalProxy,
}
