import os
emsdk_path = os.path.dirname(os.environ.get('EM_CONFIG')).replace('\\', '/')
NODE_JS = '{node_toolchain}'
PYTHON = '{python_toolchain}'
LLVM_ROOT = emsdk_path + '/upstream/bin'
BINARYEN_ROOT = emsdk_path + '/upstream'
EMSCRIPTEN_ROOT = emsdk_path + '/emscripten'
#LLVM_ROOT = '{llvm_toolchain}'
#BINARYEN_ROOT = emsdk_path + '/binaryen/main_64bit_binaryen'
#EMSCRIPTEN_ROOT = emsdk_path + '/emscripten/main'
COMPILER_ENGINE = NODE_JS
JS_ENGINES = [NODE_JS]
