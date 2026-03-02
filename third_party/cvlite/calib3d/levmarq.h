// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"

namespace c8cv {

class CV_EXPORTS LMSolver : public Algorithm {
public:
  class CV_EXPORTS Callback {
  public:
    virtual ~Callback() {}
    virtual bool compute(InputArray param, OutputArray err, OutputArray J) const = 0;
  };

  virtual void setCallback(const Ptr<LMSolver::Callback> &cb) = 0;
  virtual int run(InputOutputArray _param0) const = 0;
};

CV_EXPORTS Ptr<LMSolver> createLMSolver(const Ptr<LMSolver::Callback> &cb, int maxIters);

}  // namespace c8cv
