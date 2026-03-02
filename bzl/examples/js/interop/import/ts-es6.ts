import {data as data7} from 'bzl/examples/js/interop/export/capnp-style'

import data3 from 'bzl/examples/js/interop/export/js-es6-default'

import data1 from '../export/commonjs-default'
import {data as data2} from '../export/commonjs-named'
import {data as data4} from '../export/js-es6-named'
import data5 from '../export/ts-es6-default'
import {data as data6} from '../export/ts-es6-named'

console.log('ts-es6-import', [data1, data2, data3, data4, data5, data6, data7].map(e => e.id))
