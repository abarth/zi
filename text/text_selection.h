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

#include <algorithm>

#include "text/text_affinity.h"
#include "text/text_position.h"
#include "text/text_range.h"

namespace zi {

class TextSelection {
 public:
  TextSelection();
  TextSelection(size_t base_offset,
                size_t extent_offset,
                TextAffinity affinity = TextAffinity::Downstream);
  TextSelection(const TextRange& range,
                TextAffinity affinity = TextAffinity::Downstream);
  ~TextSelection();

  size_t base_offset() const { return base_offset_; }
  size_t extent_offset() const { return extent_offset_; }
  TextAffinity affinity() const { return affinity_; }

  TextPosition base() const { return TextPosition(base_offset_, affinity_); }
  TextPosition extent() const {
    return TextPosition(extent_offset_, affinity_);
  }

  size_t start_offset() const { return std::min(base_offset_, extent_offset_); }
  size_t end_offset() const { return std::max(base_offset_, extent_offset_); }
  TextRange range() const { return TextRange(start_offset(), end_offset()); }

  TextPosition start() const { return TextPosition(start_offset(), affinity_); }
  TextPosition end() const { return TextPosition(end_offset(), affinity_); }

 private:
  size_t base_offset_ = 0;
  size_t extent_offset_ = 0;
  TextAffinity affinity_ = TextAffinity::Downstream;
};

}  // namespace zi
