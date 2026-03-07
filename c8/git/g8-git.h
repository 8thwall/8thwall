// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Git formula used in 8th Wall Studio.

#pragma once

#include "c8/git/g8-api.capnp.h"
#include "c8/io/capnp-messages.h"

namespace c8 {

// Perform actions on one or more g8 Changesets.
ConstRootMessage<G8ChangesetResponse> g8Changeset(G8ChangesetRequest::Reader request);

// Perform actions on one or more g8 Clients.
ConstRootMessage<G8ClientResponse> g8Client(G8ClientRequest::Reader request);

// Perform actions on a repository
ConstRootMessage<G8RepositoryResponse> g8Repository(G8RepositoryRequest::Reader request);

// Request a file merge.
ConstRootMessage<G8MergeFileResponse> g8MergeFile(G8MergeFileRequest::Reader request);

// Create a new git blob.
ConstRootMessage<G8CreateBlobResponse> g8CreateBlob(G8CreateBlobRequest::Reader request);

// Request data of a specific blob
ConstRootMessage<G8BlobResponse> g8GetBlob(G8BlobRequest::Reader request);

// Request a line-by-line diff between two blobs.
ConstRootMessage<G8DiffBlobsResponse> g8DiffBlobs(G8DiffBlobsRequest::Reader request);

// Perform action on files in client
ConstRootMessage<G8FileResponse> g8FileRequest(G8FileRequest::Reader request);

// Perform a diff on two points in time
ConstRootMessage<G8DiffResponse> g8Diff(G8DiffRequest::Reader request);

// Inspect files to get blobIds from a particular change point.
ConstRootMessage<G8InspectResponse> g8Inspect(G8InspectRequest::Reader request);

}  // namespace c8
