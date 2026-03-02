"""
  Compiler flags used across the different toolchains.
  https://clang.llvm.org/docs/CommandGuide/clang.html#code-generation-options
"""

# -c dbg (debug) compiler flags.
# Build with extra symbols and a _DEBUG macro.
DBG_COMPILER_FLAGS = ["-g2", "-O0", "-D_DEBUG"]

# -c fastbuild (default) compiler flags.
# Build with optimization and symbols.
FASTBUILD_COMPILER_FLAGS = ["-g2", "-O3", "-DNDEBUG"]

# -c opt (release) compiler flags.
# Build with LTO and other flags to optimize for size and performance.
# LTO needs to be in both compiling and linking flags.
_OPT_COMPILER_FLAGS = ["-O3", "-DNDEBUG", "-fdata-sections", "-ffunction-sections", "-fvisibility=hidden"]

OPT_WITH_SYMBOLS_COMPILER_FLAGS = ["-g2"] + _OPT_COMPILER_FLAGS
OPT_WITHOUT_SYMBOLS_COMPILER_FLAGS = ["-g0"] + _OPT_COMPILER_FLAGS
OPT_WITH_SYMBOLS_NO_LTO_COMPILER_FLAGS = ["-g2"] + _OPT_COMPILER_FLAGS
OPT_WITHOUT_SYMBOLS_NO_LTO_COMPILER_FLAGS = ["-g0"] + _OPT_COMPILER_FLAGS
