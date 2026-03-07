// Copyright (c) 2022 Niantic, Inc.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#pragma once

#include "apps/g8/git-service-interface.h"

namespace c8 {

/**
 * @brief Creates an abstract git service endpoint based ont he supplied args.
 * @param creationArgs An immutable copy of the args used to create this service.
 *
 */
class GitServiceFactory {
public:
  static std::unique_ptr<GitServiceInterface> create(const GitServiceFactoryArgs);

  // Don't construct.
  GitServiceFactory() = delete;
  GitServiceFactory(GitServiceFactory &&) = delete;
  GitServiceFactory(const GitServiceFactory &) = delete;
  GitServiceFactory &operator=(GitServiceFactory &) = delete;
  GitServiceFactory &operator=(const GitServiceFactory &) = delete;
};

}  // namespace c8
