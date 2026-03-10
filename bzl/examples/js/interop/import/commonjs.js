const {data: data7} = require('bzl/examples/js/interop/export/capnp-style')

const data1 = require('../export/commonjs-default')
const {data: data2} = require('../export/commonjs-named')
const {default: data3} = require('../export/js-es6-default')
const {data: data4} = require('../export/js-es6-named')
const {default: data5} = require('../export/ts-es6-default')
const {data: data6} = require('../export/ts-es6-named')

console.log('commonjs-import', [data1, data2, data3, data4, data5, data6, data7].map(e => e.id))
