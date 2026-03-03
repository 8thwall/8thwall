#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>

#include "bzl/examples/grpc/proto/test_service.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using test_service_package::IntRequest;
using test_service_package::IntReply;
using test_service_package::Decompositor;

class DecompositorClient {
 public:
  DecompositorClient(std::shared_ptr<Channel> channel)
      : stub_(Decompositor::NewStub(channel)) {}
  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string Decompose(const int& question_int) {
    // Data we are sending to the server.
    IntRequest request;
    request.set_value(question_int);
    // Container for the data we expect from the server.
    IntReply reply;
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    // The actual RPC.
    Status status = stub_->Decompose(&context, request, &reply);
    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }
 private:
  std::unique_ptr<Decompositor::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  DecompositorClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  int value = 42;
  std::string reply = greeter.Decompose(value);
  std::cout << "Decompositor received: " << reply << std::endl;
  return 0;
}