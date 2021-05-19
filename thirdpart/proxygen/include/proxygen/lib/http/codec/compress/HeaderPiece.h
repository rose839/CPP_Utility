/*
 *  Copyright (c) 2015-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include <deque>
#include <folly/Range.h>

namespace proxygen { namespace compress {

/**
 * in-place representation of a header name or value
 */
class HeaderPiece {
 public:
  /**
   * Construct a view around the data
   */
  HeaderPiece(const char* inData,
             uint32_t inLen,
             bool inOwner,
             bool inMultiValued)
      : str(inData, inLen),
        owner(inOwner),
        multiValued(inMultiValued) {}

  HeaderPiece(HeaderPiece&& goner) noexcept
      : str(goner.str),
        owner(goner.owner),
        multiValued(goner.multiValued){
    goner.owner = false;
  }

  ~HeaderPiece() {
    if (owner) {
      CHECK_NOTNULL(str.data());
      delete[] str.data();
    }
  }

  bool isMultiValued() const {
    return multiValued;
  }

  // should be const, but for one use in GzipHeaderCodec
  folly::StringPiece str;

 private:
  bool owner;
  bool multiValued;
};

using HeaderPieceList = std::deque<HeaderPiece>;

}}
