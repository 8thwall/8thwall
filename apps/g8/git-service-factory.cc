// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "git-service-factory.h",
  };
  deps = {
    ":github-service",
    ":gitlab-service",
  };
}
cc_end(0x2434bd4b);

#include "apps/g8/git-service-factory.h"
#include "apps/g8/github-service.h"
#include "apps/g8/gitlab-service.h"
#include "c8/c8-log.h"

namespace c8 {

std::unique_ptr<GitServiceInterface> GitServiceFactory::create(const GitServiceFactoryArgs args) {

  if (args.serviceType == GitServiceType::GITHUB) {
    return std::make_unique<GithubService>(args);
  }

  if (args.serviceType == GitServiceType::GITLAB) {
    return std::make_unique<GitlabService>(args);
  }

  C8Log("[WARNING] GitServiceFactory received unspecified git service type.");
  return nullptr;
}

}  // namespace c8
