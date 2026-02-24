import sys
import os
import stat
import subprocess
import third_party.bazel.src.main.protobuf.extra_actions_base_pb2 as ea

def main(argv):
  extraActionsFile = argv[0]
  nodeExe = argv[1]
  emrunExe = argv[2]

  with open(extraActionsFile, 'rb') as f:
    extraActions = ea.ExtraActionInfo()
    extraActions.ParseFromString(f.read())

  if extraActions.mnemonic != 'CppLink':
    # Return if not a link action.
    return

  cppLinkInfo = extraActions.Extensions[ea.CppLinkInfo.cpp_link_info]

  if cppLinkInfo.link_target_type != "EXECUTABLE":
    # Return if not a binary.
    return

  htmlFile = cppLinkInfo.output_file + '.html'
  jsNodeFile = cppLinkInfo.output_file + '-node.js'

  command = cppLinkInfo.link_opt
  
  # Strip the -jsrun parameter.
  command = [x for x in command if x != "-jsrun"]

  outputIndex = command.index('-o') + 1

  # Create the output file.
  command[outputIndex] = htmlFile

  command.extend([
    "-O3",
    "--memory-init-file", "0",
    "-s",
    "TOTAL_MEMORY=134217728",
    "-s",
    "WASM=1",
    "-s",
    "FETCH=1",
  ])

  # Build once without NODERAWFS support for emrun.
  subprocess.check_call(command + ["--emrun"])

  # Build again with NODERAWFS support for node.
  command[outputIndex] = jsNodeFile
  subprocess.check_call(command + ["-s","NODERAWFS=1"])

  # Create the run script
  runFile = cppLinkInfo.output_file + '-run'
  with open(runFile, 'w') as f:
      f.write("\n".join([
      "#!/bin/bash",
      "ARGS=( \"$@\" )",
      "NUM_ARGS=$#",
      "ARGSPLIT=$#",
      "while [ $# -gt 0 ]",
      "do",
      "  case \"$1\" in",
      "    --)",
      "    ARGSPLIT=$(($NUM_ARGS - $#))",
      "    break",
      "    ;;",
      "  esac",
      "  shift",
      "done",
      "WORKSPACE=\"%s\"" % os.getcwd(),
      "NODE=\"%s\"" % nodeExe,
      "EMRUN=\"%s\"" % emrunExe,
      "BINBASE=\"%s\"" % os.path.splitext(os.path.basename(htmlFile))[0],
      "if [[ $ARGSPLIT -gt 0 ]];then",
      "  EXE=$WORKSPACE/$EMRUN",
      "  BIN=${BINBASE}.html",
      "else",
      "  EXE=$WORKSPACE/$NODE",
      "  BIN=${BINBASE}-node.js",
      "fi",
      "DIR=`dirname ${BASH_SOURCE[0]}`",
      "$EXE \"${ARGS[@]:0:${ARGSPLIT}}\" \"$DIR\"/${BIN} \"${ARGS[@]:$((${ARGSPLIT}+1))}\""]) + "\n")
      st = os.fstat(f.fileno())
      os.fchmod(f.fileno(), st.st_mode | stat.S_IEXEC | stat.S_IXGRP | stat.S_IXOTH)


if __name__ == '__main__':
  main(sys.argv[1:])


