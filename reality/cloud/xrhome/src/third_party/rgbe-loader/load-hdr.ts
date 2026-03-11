/*
 Adapted from https://github.com/mrdoob/three.js/blob/master/examples/js/loaders/RGBELoader.js
 Adapted from http://www.graphics.cornell.edu/~bjw/rgbe.html

 THIS CODE CARRIES NO GUARANTEE OF USABILITY OR FITNESS FOR ANY PURPOSE.  WHILE THE AUTHORS HAVE
 TRIED TO ENSURE THE PROGRAM WORKS CORRECTLY, IT IS STRICTLY USE AT YOUR OWN RISK.

 This file contains code to read and write four byte rgbe file format developed by Greg Ward.  It
 handles the conversions between rgbe and pixels consisting of floats.  The data is assumed to be an
 array of floats. By default there are three floats per pixel in the order red, green, blue.
 (RGBE_DATA_??? values control this.)  Only the mimimal header reading is implemented.  Each routine
 does error checking and will return a status value as defined below.  This code is intended as a
 skeleton so feel free to modify it to suit your needs.

 (Place notice here if you modified the code.)
 posted to http://www.graphics.cornell.edu/~bjw/
 written by Bruce Walter  (bjw@graphics.cornell.edu)  5/26/95
 based on code written by Greg Ward
*/

const RGBE_RETURN_FAILURE = {width: 0, height: 0, image: null}
const NEWLINE = '\n'

const rgbe_error = (msg) => {
  console.error(`hdr error: ${msg}`)
  return RGBE_RETURN_FAILURE
}

const fgets = (buffer, lineLimit = 0, consume = true) => {
  const chunkSize = 128
  lineLimit = !lineLimit ? 1024 : lineLimit
  let p = buffer.pos
  let  i = -1
  let len = 0
  let s = ''
  let chunk = String.fromCharCode.apply( null, new Uint16Array(buffer.subarray(p, p + chunkSize)))

  while (( 0 > (i = chunk.indexOf(NEWLINE))) && (len < lineLimit) && (p < buffer.byteLength)) {
    s += chunk
    len += chunk.length
    p += chunkSize
    chunk += String.fromCharCode.apply(null, new Uint16Array(buffer.subarray(p, p + chunkSize )))
  }

  if (-1 < i) {
    if ( false !== consume) {
      buffer.pos += len + i + 1
    }
    return s + chunk.slice(0, i)

  }
  return false
}

const RGBE_ReadHeader = (buffer) => {
  let line
  let match

  // regexes to parse header info fields
  const magic_token_re = /^#\?(\S+)$/
  const gamma_re = /^\s*GAMMA\s*=\s*(\d+(\.\d+)?)\s*$/
  const exposure_re = /^\s*EXPOSURE\s*=\s*(\d+(\.\d+)?)\s*$/
  const format_re = /^\s*FORMAT=(\S+)\s*$/
  const dimensions_re = /^\s*\-Y\s+(\d+)\s+\+X\s+(\d+)\s*$/

  // RGBE format header struct
  const header = {
    validformat: false, // indicate which fields are valid
    validdimensions: false, //
    string: '', // the actual header string
    comments: '', // comments found in header
    programtype: 'RGBE', // listed at beginning of file to identify it after '#?'. defaults to 'RGBE'
    format: '', // RGBE format, default 32-bit_rle_rgbe
    gamma: 1.0, // image has already been gamma corrected with given gamma. defaults to 1.0 (no correction)
    exposure: 1.0, // a value of 1.0 in an image corresponds to <exposure> watts/steradian/m^2. defaults to 1.0
    width: 0, // image width
    height: 0, // image height
  }

  if (buffer.pos >= buffer.byteLength || !(line = fgets(buffer))) {
    return rgbe_error('no header found')
  }

  if (!(match = line.match(magic_token_re))) {
    return rgbe_error('bad initial token')
  }

  header.programtype = match[1]
  header.string += line + '\n'

  while (true) {
    line = fgets(buffer)
    if (false === line) {
      break
    }

    header.string += line + '\n'

    if ('#' === line.charAt(0)) {
      header.comments += line + '\n'
      continue // comment line
    }

    match = line.match(gamma_re)
    if (match) {
      header.gamma = parseFloat(match[1])
    }
    match = line.match(exposure_re)
    if (match) {
      header.exposure = parseFloat(match[1])
    }
    match = line.match(format_re)
    if (match) {
      header.validformat = true
      header.format = match[1]
    }
    match = line.match(dimensions_re)
    if (match) {
      header.validdimensions = true
      header.height = parseInt(match[1], 10)
      header.width = parseInt(match[2], 10)
    }

    if (header.validformat && header.validdimensions) {
      break
    }
  }

  if (!header.validformat) {
    return rgbe_error('missing format specifier')
  }
  if (!header.validdimensions) {
    return rgbe_error('missing image size specifier')
  }

  return header
}

const fillScaleTable = (table) => {
  table.length = 256
  table.fill(0)
  table.forEach((v, i) => table[i] = Math.pow(2.0, i - 128.0))
  return table
}

const SCALE_LUT = []

// Builds a histogram equalized mapping from exponential values to values.
const buildRgbeHist = src => {
  if (!SCALE_LUT.length) {
    fillScaleTable(SCALE_LUT)
  }

  // Create a map from scale * value to pixel counts at that (scale, value). This map has at
  // most 65536 (2^16) elements, although actually somewhat less because some scale*value
  // combinations map to the same output, and in practice many less because an image rarely covers
  // the usable space of values.
  const valMap = new Map()
  for (let idx = 0; idx < src.length; idx += 4) {
    const scale = SCALE_LUT[src[idx + 3]]
    let v = 0
    v = src[idx + 0] * scale
    valMap.set(v, valMap.get(v) + 1 || 1)
    v = src[idx + 1] * scale
    valMap.set(v, valMap.get(v) + 1 || 1)
    v = src[idx + 2] * scale
    valMap.set(v, valMap.get(v) + 1 || 1)
  }

  // Sort the observed scale * value by cardinality.
  const sortedVals = Array.from(valMap.keys()).sort((a, b) => a - b)

  // Compute the ideal number of pixel values at each intensity for an equalized histogram. This
  // is the total number r + g + b pixels split across 256 buckets (0-255).
  const bucketSize = src.length * 3 / 4 / 256
  const arr = []
  arr.length = 256
  const percentiles = arr.fill(0).map((v, i) => (i + 1) * bucketSize)

  // Compute the scale * value cutoffs for each intensity level.
  const cutoffs = sortedVals.reduce((r, v) => {
    r.sum += valMap.get(v)
    while (r.pidx < percentiles.length && percentiles[r.pidx] < r.sum) {
      r.hist.push(r.lastv)
      r.pidx++
    }
    r.lastv = v
    return r
  }, {hist: [0], sum: 0, pidx: 0, lastv: 0}).hist

  // Create a lookup table from [scale][value] to intensity level.
  const veLutOuter = []
  veLutOuter.length = 256
  const veLut = veLutOuter.fill(0).map((z1, i) => {
    const scale = SCALE_LUT[i]
    const veLutInner = []
    veLutInner.length = 256
    return veLutInner.fill(0).map((z2, j) => {
      // The value for this table entry
      const val = j * scale

      // Find the target values that is just greater than this level, and use one-before-it as the
      // pixel value. Currently using brute force search.
      // TODO(nb): there some memoization opportunity on val, since some scale * value pairs are
      //           identical.
      // TODO(nb): binary search would be faster than brute force search.
      for (let hidx = 1; hidx <cutoffs.length; ++hidx) {
        if (val <= cutoffs[hidx]) {
          return hidx - 1
        }
      }
      return 255
    })
  })

  return veLut
}

// RGBE is a high dynamic range space. To render it, we need some transform from a high precision
// space to a low precision space. We do a non-linear historgram equalizing transform to capture as
// much range as possible.
const RGBEBufToRGBABuf = src => {
  // Build a lookup table (size 256x256) of scale x value -> intensity based on the source image
  // historgam.
  const veLut = buildRgbeHist(src)

  // Set each image pixel value based on scale x value.
  const dst = new Uint8ClampedArray(src.length)
  for (let idx = 0; idx < src.length; idx += 4) {
    // Histogram equalizing transform.
    const scale = veLut[src[idx + 3]]
    dst[idx + 0] = scale[src[idx + 0]]
    dst[idx + 1] = scale[src[idx + 1]]
    dst[idx + 2] = scale[src[idx + 2]]
    dst[idx + 3] = 255

    /*
    // Linear transform:
    const scale = SCALE_LUT[src[idx + 3]]
    // Since dst is Uint8ClampedArray, we don't need to do extra clamping or flooring here.
    dst[idx + 0] = src[idx + 0] * scale
    dst[idx + 1] = src[idx + 1] * scale
    dst[idx + 2] = src[idx + 2] * scale
    dst[idx + 3] = 255
    */
  }
  return dst
}

const RGBE_ReadPixels_RLE = (buffer, w, h) => {
  const scanline_width = w
  let num_scanlines = h

  if (
    // run length encoding is not allowed so read flat
    ((scanline_width < 8) || (scanline_width > 0x7fff)) ||
    // this file is not run length encoded
    ((2 !== buffer[0]) || (2 !== buffer[1] ) || (buffer[2] & 0x80))) {
    return new Uint8Array(buffer)  // return the flat buffer
  }

  if (scanline_width !== ((buffer[2] << 8) | buffer[3])) {
    return rgbe_error('wrong scanline width')
  }

  if ((w * h) > (1 << 28)) {
    return rgbe_error('unable to allocate buffer space')
  }

  const ptr_end = 4 * scanline_width
  const data_rgba = new Uint8Array(4 * w * h)
  const scanline_buffer = new Uint8Array(ptr_end)

  // read in each successive scanline
  let pos = 0
  let offset = 0
  while ((num_scanlines > 0) && (pos < buffer.byteLength)) {
    if ( pos + 4 > buffer.byteLength ) {
      return rgbe_error('incomplete scanline')
    }

    const scanHead1 = buffer[pos + 0]
    const scanHead2 = buffer[pos + 1]
    const scanW = (buffer[pos + 2] << 8) | buffer[pos + 3]
    pos += 4

    if ((2 !== scanHead1) || (2 !== scanHead2) || (scanW != scanline_width)) {
      return rgbe_error('bad rgbe scanline format')
    }

    // read each of the four channels for the scanline into the buffer
    let ptr = 0
    while ((ptr < ptr_end) && (pos < buffer.byteLength)) {
      let count = buffer[pos++]
      const isEncodedRun = count > 128
      if (isEncodedRun) {
        count -= 128
      }

      if ((0 === count) || (ptr + count > ptr_end)) {
        return rgbe_error('bad scanline data')
      }

      if (isEncodedRun) {
        const byteValue = buffer[pos++]  // a (encoded) run of the same value
        for (let i = 0; i < count; i++) {
          scanline_buffer[ptr++] = byteValue
        }
      } else {
        scanline_buffer.set(buffer.subarray(pos, pos + count), ptr)  // a literal-run
        ptr += count
        pos += count
      }
    }

    // now convert data from buffer into rgba
    const roff = 0
    const goff = roff + scanline_width
    const boff = goff + scanline_width
    const eoff = boff + scanline_width

    for (let i = 0; i < scanline_width; i++) {
      data_rgba[offset + 0] = scanline_buffer[i + roff]
      data_rgba[offset + 1] = scanline_buffer[i + goff]
      data_rgba[offset + 2] = scanline_buffer[i + boff]
      data_rgba[offset + 3] = scanline_buffer[i + eoff]
      offset += 4
    }

    num_scanlines--
  }

  return data_rgba
}

const parse = (buffer) => {
  const byteArray : any = new Uint8Array(buffer)  // disable type checks because we decorate.
  byteArray.pos = 0
  const rgbe_header_info = RGBE_ReadHeader(byteArray)

  if (RGBE_RETURN_FAILURE === rgbe_header_info) {
    return RGBE_RETURN_FAILURE
  }

  const {width, height} = rgbe_header_info
  const image_rgbe_data = RGBE_ReadPixels_RLE(byteArray.subarray(byteArray.pos), width, height)

  if (RGBE_RETURN_FAILURE === image_rgbe_data) {
    return RGBE_RETURN_FAILURE
  }
  const pix = RGBEBufToRGBABuf(image_rgbe_data)
  const im = new ImageData(pix, width, height)
  return {
    width,
    height,
    buffer: pix,
    image: im,
    header: rgbe_header_info.string,
    gamma: rgbe_header_info.gamma,
    exposure: rgbe_header_info.exposure,
  }
}

const loadHdr = (hdrAsset) => fetch(hdrAsset, {mode: 'cors'})
  .then(r => r.arrayBuffer())
  .then(r => parse(r))

export {loadHdr}

