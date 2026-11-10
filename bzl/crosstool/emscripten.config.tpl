import os
emsdk_path = os.path.dirname(os.environ.get('EM_CONFIG')).replace('\\', '/')
NODE_JS = '{node_toolchain}'
if os.name == 'nt':
    # @nodejs_host points at an extensionless Unix launcher path. On Windows,
    # use the native executable from the platform repository directly; the
    # host repository's generated node.exe symlink has an MSYS target that
    # native subprocess creation cannot follow.
    external_root = os.path.dirname(os.path.dirname(os.path.dirname(NODE_JS)))
    NODE_JS = os.path.join(external_root, 'nodejs_windows_amd64', 'bin', 'nodejs', 'node.exe')
PYTHON = '{python_toolchain}'
LLVM_ROOT = emsdk_path + '/upstream/bin'
BINARYEN_ROOT = emsdk_path + '/upstream'
EMSCRIPTEN_ROOT = emsdk_path + '/emscripten'
#LLVM_ROOT = '{llvm_toolchain}'
#BINARYEN_ROOT = emsdk_path + '/binaryen/main_64bit_binaryen'
#EMSCRIPTEN_ROOT = emsdk_path + '/emscripten/main'
COMPILER_ENGINE = NODE_JS
JS_ENGINES = [NODE_JS]
