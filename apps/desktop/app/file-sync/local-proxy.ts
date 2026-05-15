import {createServer} from 'node:http'
import {createProxyServer} from 'httpxy'

const startLocalProxy = (proxyPort: number, webpackPort: number) => {
  const proxy: any = createProxyServer({})

  proxy.on('proxyReq', (proxyReq) => {
    proxyReq.setHeader('Host', 'http://localhost')
    proxyReq.setHeader('Origin', 'http://localhost')
  })

  proxy.on('proxyReqWs', (...args) => {
    console.log('proxyReqWs')
  //   // proxyReq.setHeader('Host', 'http://localhost')
  //   // proxyReq.setHeader('Origin', 'http://localhost')
  })

  const server = createServer((req, res) => {
    // console.log(req.headers)
    req.headers.host = `localhost:${webpackPort}`
    return proxy.web(req, res, {target: `http://localhost:${webpackPort}`})
  })

  server.on('upgrade', (req, socket, head) => {
    console.log('upgrade request is being proxied', req.url)
    if (req.url === '/dev8') {
      return proxy.ws(req, socket, {target: `http://localhost:${62009}/incoming`}, head)
    }
    return proxy.ws(req, socket, {target: `http://localhost:${webpackPort}${req.url}`}, head)
  })

  server.listen(proxyPort, () => {
    console.log(`Proxy is listening on http://localhost:${proxyPort}`)
    console.log(`Base server is http://localhost:${webpackPort}`)
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
