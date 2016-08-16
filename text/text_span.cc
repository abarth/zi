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

#include "text/text_span.h"

#include <algorithm>

namespace zi {

TextSpan::TextSpan() {}

TextSpan::TextSpan(size_t begin, size_t end) : begin_(begin), end_(end) {}

TextSpan::~TextSpan() {}

void TextSpan::MarkDirty() {
  is_dirty_ = true;
}

void TextSpan::MarkClean() {
  is_dirty_ = false;
}

void TextSpan::PushFront(size_t count) {
  begin_ -= std::min(count, begin_);
  MarkDirty();
}

void TextSpan::PushBack(size_t count) {
  end_ += count;
  MarkDirty();
}

void TextSpan::PopFront(size_t count) {
  begin_ += count;
  end_ = std::max(begin_, end_);
  MarkDirty();
}

void TextSpan::PopBack(size_t count) {
  end_ -= std::min(count, end_);
  begin_ = std::min(begin_, end_);
  MarkDirty();
}

void TextSpan::ShiftForward(size_t count) {
  begin_ += count;
  end_ += count;
}

void TextSpan::ShiftBackward(size_t count) {
  size_t delta = std::min(count, begin_);
  begin_ -= delta;
  end_ -= delta;
}

bool TextSpan::DescendingByBegin::operator()(const TextSpan* lhs,
                                             const TextSpan* rhs) const {
  return lhs->begin() > rhs->begin();
}

bool TextSpan::AscendingByEnd::operator()(const TextSpan* lhs,
                                          const TextSpan* rhs) const {
  return lhs->end() < rhs->end();
}

}  // namespace zi
