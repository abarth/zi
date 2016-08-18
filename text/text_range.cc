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

#include "text/text_range.h"

#include <algorithm>

#include "text/text_selection.h"

namespace zi {

TextRange::TextRange() = default;

TextRange::TextRange(size_t begin, size_t end) : begin_(begin), end_(end) {}

TextRange::TextRange(const TextSelection& selection)
    : TextRange(selection.start_offset(), selection.end_offset()) {}

TextRange::~TextRange() = default;

void TextRange::MarkDirty() {
  is_dirty_ = true;
}

void TextRange::MarkClean() {
  is_dirty_ = false;
}

void TextRange::PushFront(size_t count) {
  const size_t delta = std::min(count, begin_);
  if (!delta)
    return;
  begin_ -= delta;
  MarkDirty();
}

void TextRange::PushBack(size_t count) {
  if (!count)
    return;
  end_ += count;
  MarkDirty();
}

void TextRange::PopFront(size_t count) {
  if (!count)
    return;
  const bool was_empty = is_empty();
  begin_ += count;
  end_ = std::max(begin_, end_);
  if (!was_empty)
    MarkDirty();
}

void TextRange::PopBack(size_t count) {
  const size_t delta = std::min(count, end_);
  if (!delta)
    return;
  const bool was_empty = is_empty();
  end_ -= delta;
  begin_ = std::min(begin_, end_);
  if (!was_empty)
    MarkDirty();
}

void TextRange::ShiftForward(size_t count) {
  begin_ += count;
  end_ += count;
}

void TextRange::ShiftBackward(size_t count) {
  const size_t delta = std::min(count, begin_);
  begin_ -= delta;
  end_ -= delta;
}

bool TextRange::DescendingByBegin::operator()(const TextRange* lhs,
                                              const TextRange* rhs) const {
  return lhs->begin() > rhs->begin();
}

bool TextRange::AscendingByEnd::operator()(const TextRange* lhs,
                                           const TextRange* rhs) const {
  return lhs->end() < rhs->end();
}

}  // namespace zi
