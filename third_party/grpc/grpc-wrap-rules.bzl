# @rules_proto_grpc project - https://rules-proto-grpc.com/en/latest/index.html
load("@rules_proto_grpc//cpp:defs.bzl", "cpp_grpc_compile", "cpp_grpc_library")

GRPC_VERSION = 'default'
# GRPC_VERSION = '1.0.0'

def warn(msg):
    print('{red}{msg}{nc}'.format(red='\033[1;33m', msg=msg, nc='\033[0m'))

def cpp_grpc_compile_versionable(name,
                                 deps, 
                                 visibility=None,
                                 linkstatic=False):
  if GRPC_VERSION != "default":
    fail("cpp_grpc_compile_versionable for now is supported only with the default gRPC version (>v1.26.0)")
  else:
    cpp_grpc_compile(
      name = name,
      protos = deps,
    )

def cpp_grpc_library_versionable(name,
                                 protos,
                                 deps = [],
                                 visibility=None,
                                 linkstatic=False):
  # NOTE: protobuf.bzl of 3.0.0 does not define a working "proto_library(_old)"
  #       but the rules to generate are part of cc_proto_library(_old)

  # For normal non-lite protobuf we prefer dynlib by default
  if GRPC_VERSION != "default":
    fail("cpp_grpc_library_versionable for now is supported only with the default gRPC version (>v1.26.0)")
  else:
    cpp_grpc_library(
      name = name,
      protos = protos,
      deps = deps,
    )
