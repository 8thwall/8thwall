// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2014, Advanced Micro Devices, Inc., all rights reserved.

#pragma once
#include "third_party/cvlite/core/bufferpool.hpp"

namespace c8cv {

class DummyBufferPoolController : public BufferPoolController
{
public:
    DummyBufferPoolController() { }
    virtual ~DummyBufferPoolController() { }

    virtual size_t getReservedSize() const { return (size_t)-1; }
    virtual size_t getMaxReservedSize() const { return (size_t)-1; }
    virtual void setMaxReservedSize(size_t size) { (void)size; }
    virtual void freeAllReservedBuffers() { }
};

} // namespace
