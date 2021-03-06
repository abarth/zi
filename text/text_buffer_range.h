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

#include <stddef.h>

#include "text/text_range.h"
#include "zen/macros.h"

namespace zi {
class TextSelection;

class TextBufferRange {
 public:
  TextBufferRange();
  TextBufferRange(size_t begin, size_t end);
  explicit TextBufferRange(const TextRange& range);
  explicit TextBufferRange(const TextSelection& selection);
  ~TextBufferRange();

  void MarkDirty();
  void MarkClean();

  void PushFront(size_t count);
  void PushBack(size_t count);

  void PopFront(size_t count);
  void PopBack(size_t count);

  void ShiftForward(size_t count);
  void ShiftBackward(size_t count);

  bool is_dirty() const { return is_dirty_; }

  size_t start() const { return start_; }
  size_t end() const { return end_; }

  TextRange range() const { return TextRange(start(), end()); }

  bool is_empty() const { return start_ == end_; }
  size_t length() const { return end_ - start_; }

  struct DescendingByStart {
    bool operator()(const TextBufferRange* lhs,
                    const TextBufferRange* rhs) const;
  };

  struct AscendingByEnd {
    bool operator()(const TextBufferRange* lhs,
                    const TextBufferRange* rhs) const;
  };

 private:
  bool is_dirty_ = false;
  size_t start_ = 0;
  size_t end_ = 0;

  DISALLOW_COPY_AND_ASSIGN(TextBufferRange);
};

}  // namespace zi
