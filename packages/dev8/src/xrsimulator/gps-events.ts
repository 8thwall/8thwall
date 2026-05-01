import type {GpsData} from './frame-data'

const makeGpsDataCursor = (data: GpsData[]) => {
  let currentIdx_ = data.length === 0 ? -1 : 0
  return {
    seek: (timeNanos: number): GpsData | null => {
      if (currentIdx_ < 0 || currentIdx_ >= data.length) {
        // No data
        return null
      }

      if (timeNanos < data[currentIdx_].timeNanos) {
        // No data available yet
        return null
      }

      // Collect all data that is available at this time
      const gpsDataToSend: GpsData[] = []
      while (currentIdx_ < data.length && timeNanos >= data[currentIdx_].timeNanos) {
        gpsDataToSend.push(data[currentIdx_])
        currentIdx_++
      }
      if (gpsDataToSend.length > 0) {
        return gpsDataToSend[gpsDataToSend.length - 1]
      }
      return null
    },
    reset: () => {
      currentIdx_ = 0
    },
  }
}

type GpsDataCursor = ReturnType<typeof makeGpsDataCursor>

export {
  makeGpsDataCursor,
  GpsDataCursor,
}
