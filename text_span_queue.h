// Copyright (c) 2016, Google Inc.
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <queue>
#include <vector>

#include "text_span.h"
#include "vector_extensions.h"

namespace zi {

template <class Compare>
class TextSpanQueue
    : public std::priority_queue<TextSpan*, std::vector<TextSpan*>, Compare> {
 public:
  void ShiftForward(size_t count) {
    for (auto& value : this->c)
      value->ShiftForward(count);
  }

  void ShiftBackward(size_t count) {
    for (auto& value : this->c)
      value->ShiftBackward(count);
  }

  template <typename Iterator>
  void Erase(Iterator begin, Iterator end) {
    if (EraseAllValues(this->c, begin, end))
      std::make_heap(this->c.begin(), this->c.end(), this->comp);
  }
};

}  // namespace zi
