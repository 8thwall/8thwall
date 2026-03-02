import data1 from '@nia/bzl/examples/js/interop/export/commonjs-default'
import {data as data2} from '@nia/bzl/examples/js/interop/export/commonjs-named'
import data3 from '@nia/bzl/examples/js/interop/export/js-es6-default'
import {data as data4} from '@nia/bzl/examples/js/interop/export/js-es6-named'
import data5 from '@nia/bzl/examples/js/interop/export/ts-es6-default'
import {data as data6} from '@nia/bzl/examples/js/interop/export/ts-es6-named'
import {data as data7} from '@nia/bzl/examples/js/interop/export/capnp-style'

console.log('ts-es6-nia', [data1, data2, data3, data4, data5, data6, data7].map(e => e.id))
