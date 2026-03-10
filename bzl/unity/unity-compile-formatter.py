#!/usr/bin/env python3

"""Format Unity Build output into Clang-like errors and warnings."""

__author__ = 'Erik Murphy-Chutorian'
__email__ = 'mc@8thwall.com'

import sys
import os
import copy
import shutil
import re
import textwrap
import linecache
from enum import Enum


class Output(Enum):
  NONE = 0
  STDOUT = 1
  STDERR = 2


compilerRe = re.compile(r"^(\S+)\((\d+),(\d+)\): (\w+) (\w+): (.*)$")
decolor = re.compile(r'\x1b[^m]*m')


def formatCompilerLine(line, wrapper, oneLineWrapper, projectRoot):
  """Format one compiler error or warning."""

  def makeBold(output):
    return "\033[1m%s\033[0m" % output

  def colorWarnError(output):
    output = re.sub(': warning:', ': \033[35mwarning:\033[39m', output, 1)
    output = re.sub(': error:', ': \033[91merror:\033[39m', output, 1)
    return output

  try:
    m = compilerRe.findall(line)[0]
    fileName = m[0]
    lineNumber = int(m[1])
    charNumber = int(m[2])
    level = m[3]
    code = m[4]
    message = m[5]
    result = "{fileName}:{lineNumber}:{charNumber}: {level}: {message}{code}".format(
      fileName=fileName,
      lineNumber=lineNumber,
      charNumber=charNumber,
      level=level,
      message=message,
      code=' [%s]' % code if level == 'warning' else ''
    )
    output = makeBold(colorWarnError(wrapper.fill(result)))
    if charNumber == 0 and lineNumber > 0:
      # Print the previous line if charNumber is zero.
      lineNumber -= 1
      charNumber = 1

    fullFileName = os.path.join(projectRoot, fileName)
    codeLine = linecache.getline(fullFileName, lineNumber).rstrip()
    shift = 0
    if charNumber > oneLineWrapper.width:
      shift = oneLineWrapper.width - 1

    prefix = '' if shift == 0 else '  ...'

    output += '\n' + oneLineWrapper.fill(prefix + codeLine[shift:])
    output += makeBold(
      '\n' + ''.join([' '] * (charNumber - 1 - shift + len(prefix))) +
      '\033[92m^\033[39m'
    )
    return output
  except IndexError:
    return makeBold(wrapper.fill(line))

def formatUnityExceptionLine(outout):

  def makeBold(output):
    return "\033[1m%s\033[0m" % output

  def colorError(output):
    return "\033[91mERROR: %s\033[39m" % output

  return makeBold(colorError(outout))

def main(argv):
  projectRoot = os.environ.get('UNITY_PROJECT_ROOT', '.')
  stdout = []
  stderr = []
  terminalSize = shutil.get_terminal_size((80, 20))
  textWrapper = textwrap.TextWrapper(
    initial_indent='',
    width=terminalSize.columns,
    subsequent_indent="      ",
    break_long_words=False,
    break_on_hyphens=False,
    replace_whitespace=False
  )
  oneLineWrapper = copy.deepcopy(textWrapper)
  oneLineWrapper.max_lines = 1
  oneLineWrapper.placeholder = '...'

  output = Output.NONE
  for line in sys.stdin:
    if line.startswith("-----CompilerOutput:-stdout") or line.startswith("##### Output"):
      output = Output.STDOUT
      continue
    if line.startswith("-----CompilerOutput:-stderr"):
      output = Output.STDERR
      continue
    if line.startswith("-----EndCompilerOutput") or line.startswith("##### CommandLine"):
      output = Output.NONE
      continue
    if (line.startswith("UnityException") or
      line.startswith("CommandInvokationFailure") or
      line.startswith("Unable to locate Android SDK.") or
      line.startswith("Cancelling") or
      line.startswith("XmlException") or
      ": error:" in line or
      "Fatal Error!" in line):
      stderr.append(formatUnityExceptionLine(line.rstrip()))
      continue

    if output == Output.STDOUT:
      stdout.append(line.rstrip())
    elif output == Output.STDERR:
      stderr.append(
        formatCompilerLine(
          line.rstrip(), wrapper=textWrapper, oneLineWrapper=oneLineWrapper,
          projectRoot=projectRoot)
      )

  if stderr:
    print('\n'.join(stderr), file=sys.stderr)
  if stdout:
    print('\n'.join(stdout), file=sys.stderr)


if __name__ == '__main__':
  main(sys.argv)
