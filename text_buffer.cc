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

#include "text_buffer.h"

#include <string.h>

#include <algorithm>
#include <utility>

namespace zi {

TextBuffer::TextBuffer() {}

TextBuffer::~TextBuffer() {}

void TextBuffer::SetText(std::vector<char> text) {
  buffer_ = std::move(text);
  gap_begin_ = 0;
  gap_end_ = 0;
}

void TextBuffer::InsertCharacter(char c) {
  if (gap_begin_ == gap_end_)
    Expand(1);
  buffer_[gap_begin_++] = c;
}

void TextBuffer::InsertText(std::string text) {
  const size_t length = text.length();
  if (gap_begin_ + length > gap_end_)
    Expand(length);
  memcpy(&buffer_[gap_begin_], text.data(), length);
  gap_begin_ += length;
}

void TextBuffer::Backspace() {
  if (gap_begin_ > 0)
    --gap_begin_;
}

void TextBuffer::MoveCursorForward() {
  if (gap_end_ < buffer_.size())
    buffer_[gap_begin_++] = buffer_[gap_end_++];
}

void TextBuffer::MoveCursorBackward() {
  if (gap_begin_ > 0)
    buffer_[--gap_end_] = buffer_[--gap_begin_];
}

void TextBuffer::MoveCursorBy(int offset) {
  if (offset > 0) {
    size_t delta =
        std::min(static_cast<size_t>(offset), buffer_.size() - gap_end_);
    memcpy(&buffer_[gap_begin_], &buffer_[gap_end_], delta);
    gap_begin_ += delta;
    gap_end_ += delta;
  } else if (offset < 0) {
    size_t delta = std::min(static_cast<size_t>(-offset), gap_begin_);
    gap_begin_ -= delta;
    gap_end_ -= delta;
    memcpy(&buffer_[gap_end_], &buffer_[gap_begin_], delta);
  }
}

void TextBuffer::MoveCursorTo(size_t position) {
  if (position > cursor_position())
    MoveCursorBy(position - cursor_position());
  else if (position < cursor_position())
    MoveCursorBy(-static_cast<int>(cursor_position() - position));
}

void TextBuffer::Expand(size_t required_gap_size) {
  size_t existing_gap = gap_end_ - gap_begin_;
  if (existing_gap >= required_gap_size)
    return;
  size_t min_size = buffer_.size() - existing_gap + required_gap_size;
  std::vector<char> new_buffer(min_size * 1.5);
  size_t tail_size = buffer_.size() - gap_end_;
  if (gap_begin_ > 0)
    memcpy(&new_buffer[0], &buffer_[0], gap_begin_);
  if (tail_size > 0) {
    memcpy(&new_buffer[new_buffer.size() - tail_size],
           &buffer_[buffer_.size() - tail_size], tail_size);
  }
  buffer_.swap(new_buffer);
  gap_end_ = buffer_.size() - tail_size;
}

}  // namespace zi
