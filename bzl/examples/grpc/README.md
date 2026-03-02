# Quick explanation

This is a simple example on how to create gRPC server/client with our current bazel rules

You can compile server and client by doing:

```
bazel build //bzl/examples/grpc:all
```

Then you can run the server by doing

```
./bazel-bin/bzl/examples/grpc/grpc_server
```

and then run the client by doing

```
./bazel-bin/bzl/examples/grpc/grpc_client
```

The server will kill itself gracefully after 10 seconds. You can change this by changing the global variable gServerLife in grpc_server.cpp