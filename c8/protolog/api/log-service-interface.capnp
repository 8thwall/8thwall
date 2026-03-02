@0xc6ddcb6e36ce40da;

using LogRequest = import "log-request.capnp";

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

interface LogService {
  # TODO(nb): move this into its own service.
  shutdown @0 ();

  # Main RPC
  log @1 (request :LogRequest.LogServiceRequest) -> (response :LogRequest.LogServiceResponse);
}
