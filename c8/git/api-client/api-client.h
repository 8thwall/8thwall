#pragma once

#include "c8/git/api-common.h"
#include "c8/git/g8-api.capnp.h"

namespace c8 {
using namespace g8;

// Perform actions on one or more g8 Clients.
ConstRootMessage<G8ClientResponse> g8Client(G8ClientRequest::Reader request);

// List clients
ConstRootMessage<G8ClientResponse> g8ClientList(ClientContext &ctx);

// Create a client
ConstRootMessage<G8ClientResponse> g8ClientCreate(ClientContext &ctx);

// Save a client
ConstRootMessage<G8ClientResponse> g8ClientSave(ClientContext &ctx);

// Switch to another client
ConstRootMessage<G8ClientResponse> g8ClientSwitch(ClientContext &ctx);

// Delete a client
ConstRootMessage<G8ClientResponse> g8ClientDelete(ClientContext &ctx);

}  // namespace c8
