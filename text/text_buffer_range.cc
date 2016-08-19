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

#include "text/text_buffer_range.h"

#include <algorithm>

#include "text/text_selection.h"

namespace zi {

TextBufferRange::TextBufferRange() = default;

TextBufferRange::TextBufferRange(size_t start, size_t end)
    : start_(start), end_(end) {}

TextBufferRange::TextBufferRange(const TextRange& range)
    : TextBufferRange(range.start(), range.end()) {}

TextBufferRange::TextBufferRange(const TextSelection& selection)
    : TextBufferRange(selection.start_offset(), selection.end_offset()) {}

TextBufferRange::~TextBufferRange() = default;

void TextBufferRange::MarkDirty() {
  is_dirty_ = true;
}

void TextBufferRange::MarkClean() {
  is_dirty_ = false;
}

void TextBufferRange::PushFront(size_t count) {
  const size_t delta = std::min(count, start_);
  if (!delta)
    return;
  start_ -= delta;
  MarkDirty();
}

void TextBufferRange::PushBack(size_t count) {
  if (!count)
    return;
  end_ += count;
  MarkDirty();
}

void TextBufferRange::PopFront(size_t count) {
  if (!count)
    return;
  const bool was_empty = is_empty();
  start_ += count;
  end_ = std::max(start_, end_);
  if (!was_empty)
    MarkDirty();
}

void TextBufferRange::PopBack(size_t count) {
  const size_t delta = std::min(count, end_);
  if (!delta)
    return;
  const bool was_empty = is_empty();
  end_ -= delta;
  start_ = std::min(start_, end_);
  if (!was_empty)
    MarkDirty();
}

void TextBufferRange::ShiftForward(size_t count) {
  start_ += count;
  end_ += count;
}

void TextBufferRange::ShiftBackward(size_t count) {
  const size_t delta = std::min(count, start_);
  start_ -= delta;
  end_ -= delta;
}

bool TextBufferRange::DescendingByStart::operator()(
    const TextBufferRange* lhs,
    const TextBufferRange* rhs) const {
  return lhs->start() > rhs->start();
}

bool TextBufferRange::AscendingByEnd::operator()(
    const TextBufferRange* lhs,
    const TextBufferRange* rhs) const {
  return lhs->end() < rhs->end();
}

}  // namespace zi
