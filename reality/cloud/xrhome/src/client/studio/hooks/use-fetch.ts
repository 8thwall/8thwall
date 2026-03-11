import React from 'react'

type UseFetchOptions = {
  type: 'json' | 'text' | 'blob' | 'buffer'
}

type DataForOptions<T extends UseFetchOptions> = T['type'] extends 'json'
  ? unknown
  : T['type'] extends 'text'
    ? string
    :T['type'] extends 'blob'
      ? Blob
      :T['type'] extends 'buffer'
        ? ArrayBuffer
        :never

type LoadedData<T extends UseFetchOptions> = {
  url: string
  data: DataForOptions<T>
  error: unknown
}

const createUseFetch = <T extends UseFetchOptions>(options: T) => (url: string) => {
  const [loadedData, setLoadedData] = React.useState<LoadedData<T>>({
    url: null,
    data: null,
    error: null,
  })

  React.useEffect(() => {
    setLoadedData({url: null, data: null, error: null})
    if (!url) {
      return undefined
    }
    const controller = new AbortController()
    const {signal} = controller

    const fetchData = async () => {
      try {
        const res = await fetch(url, {...options, signal})

        let newData: typeof loadedData.data

        if (!res.ok) {
          throw new Error(`Error fetching ${url}`)
        }

        switch (options.type) {
          case 'json':
            newData = await res.json()
            break
          case 'text':
            newData = await res.text() as DataForOptions<T>
            break
          case 'blob':
            newData = await res.blob() as DataForOptions<T>
            break
          case 'buffer':
            newData = await res.arrayBuffer() as DataForOptions<T>
            break
          default:
            throw new Error('Invalid type')
        }

        if (!signal.aborted) {
          setLoadedData({url, data: newData, error: null})
        } else {
          const json = await res.json()
          if (!signal.aborted) {
            setLoadedData({url, data: json, error: null})
          }
        }
      } catch (err) {
        if (!signal.aborted) {
          setLoadedData({url, data: null, error: err})
        }
      }
    }

    if (url) {
      fetchData()
    }

    return () => {
      controller.abort()
    }
  }, [url])

  return loadedData.url === url ? loadedData : {loadedFor: null, data: null, error: null}
}

const useRawFetch = createUseFetch({type: 'blob'})
const useJsonFetch = createUseFetch({type: 'json'})
const useArrayBufferFetch = createUseFetch({type: 'buffer'})

export {
  createUseFetch,
  useRawFetch,
  useJsonFetch,
  useArrayBufferFetch,
}
