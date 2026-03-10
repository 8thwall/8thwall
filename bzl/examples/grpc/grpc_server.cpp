#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <vector>
#include <sstream>

#include <grpc++/grpc++.h>

#include "bzl/examples/grpc/proto/test_service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using test_service_package::IntRequest;
using test_service_package::IntReply;
using test_service_package::Decompositor;

using namespace std::chrono_literals;

namespace 
{
  std::unique_ptr<Server> gGlobalServer;
  auto gServerLife = 10s;
}

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Decompositor::Service 
{
  
  Status Decompose(ServerContext* context, 
                   const IntRequest* request,
                   IntReply* reply) override {

    auto question_int = request->value();

    std::vector<int> values;

    while (question_int != 0)
    {
      std::random_device rd;  //Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd());

      std::uniform_int_distribution<> distrib(1, question_int);

      auto value = distrib(gen);

      values.push_back(value);
      question_int -= value;
    }
    
    std::string prefix;

    for(auto v : values)
    {
      prefix += std::to_string(v) + " ";
    }
    
    prefix += " = ";
    
    reply->set_message(prefix + std::to_string(request->value()));
    
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  // std::unique_ptr<Server> server(builder.BuildAndStart());
  gGlobalServer = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;
  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  // server->Wait();
  gGlobalServer->Wait();
}

void grpc_server_killer()
{
  
  std::this_thread::sleep_for(gServerLife);
  std::cout << "Killing the server after " << gServerLife.count() << "s" << std::endl;
  gGlobalServer->Shutdown(); 
}

int main(int argc, char** argv) {
  
  std::thread t1(grpc_server_killer);

  RunServer();

  t1.join();

  return 0;
}