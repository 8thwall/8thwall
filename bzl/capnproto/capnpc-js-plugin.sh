#!/bin/bash

DIR=$PWD
NODE_BIN="${DIR}/../../../${NODE:-node}"

# Symlink the npm-capnpc-js node_modules directory so that when capnpc-js uses typescript to
# transpile the capnpc-ts output to js using typescript, the 'capnp-ts' module is contained
# in an enclosing node_modules directory.
ln -s ../../../external/npm-capnpc-js/node_modules node_modules

# Run the capnp compiler plugin. This gets data about what to compile from stdin, and then writes
# files out.
${NODE_BIN} ./node_modules/capnpc-js/bin/capnpc-js.js
