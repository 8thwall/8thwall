const loadScript = (url: string) => new Promise<void>((resolve, reject) => {
  const script = document.createElement('script')
  script.async = true
  script.crossOrigin = 'anonymous'
  script.onload = () => resolve()
  script.onerror = reject
  script.src = url
  document.head.appendChild(script)
})

export {
  loadScript,
}
