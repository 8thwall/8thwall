@0xe1690988ccae192f;

using RemoteRequest = import "remote-request.capnp";

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

interface RemoteService {
  # TODO(nb): move this into its own service.
  shutdown @0 ();

  # Main RPC
  log @1 (request :RemoteRequest.RemoteServiceRequest) -> (response :RemoteRequest.RemoteServiceResponse);
}
