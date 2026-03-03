#!/bin/bash

DIR=$PWD
NODE_BIN="${DIR}/../../../${NODE:-node}"

# Run the capnp compiler plugin. This gets data about what to compile from stdin, and then writes
# files out.
${NODE_BIN} ../../../external/npm-capnpc-ts/node_modules/capnpc-ts/bin/capnpc-ts.js
