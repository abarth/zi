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

namespace zi {

TextBuffer::TextBuffer() {}

TextBuffer::TextBuffer(std::vector<char> text) : buffer_(std::move(text)) {}

TextBuffer::~TextBuffer() {}

void TextBuffer::InsertCharacter(size_t position, char c) {
  if (gap_begin_ == gap_end_)
    Expand(1);
  MoveInsertionPointTo(position);
  buffer_[gap_begin_++] = c;
  DidInsert(1);
}

void TextBuffer::InsertText(size_t position, std::string text) {
  const size_t length = text.length();
  if (gap_begin_ + length > gap_end_)
    Expand(length);
  MoveInsertionPointTo(position);
  memcpy(&buffer_[gap_begin_], text.data(), length);
  gap_begin_ += length;
  DidInsert(length);
}

void TextBuffer::DeleteCharacter(size_t position) {
  MoveInsertionPointTo(position + 1);
  --gap_begin_;
  DidDelete(1);
  DidMoveInsertionPointBackward();
}

void TextBuffer::MoveInsertionPointForward(size_t offset) {
  size_t delta = std::min(offset, buffer_.size() - gap_end_);
  memmove(&buffer_[gap_begin_], &buffer_[gap_end_], delta);
  gap_begin_ += delta;
  gap_end_ += delta;
  DidMoveInsertionPointForward();
}

void TextBuffer::MoveInsertionPointBackward(size_t offset) {
  size_t delta = std::min(offset, gap_begin_);
  gap_begin_ -= delta;
  gap_end_ -= delta;
  memmove(&buffer_[gap_end_], &buffer_[gap_begin_], delta);
  DidMoveInsertionPointBackward();
}

void TextBuffer::MoveInsertionPointTo(size_t position) {
  if (position > gap_begin_)
    MoveInsertionPointForward(position - gap_begin_);
  else if (position < gap_begin_)
    MoveInsertionPointBackward(gap_begin_ - position);
}

size_t TextBuffer::Find(char c, size_t pos) {
  if (pos < gap_begin_) {
    char* ptr = static_cast<char*>(memchr(&buffer_[pos], c, gap_begin_ - pos));
    if (ptr)
      return ptr - data();
    pos = gap_begin_;
  }
  pos += gap_size();
  if (pos < buffer_.size()) {
    char* ptr =
        static_cast<char*>(memchr(&buffer_[pos], c, buffer_.size() - pos));
    if (ptr)
      return ptr - data() - gap_size();
  }
  return std::string::npos;
}

std::string TextBuffer::ToString() const {
  std::string result;
  result.resize(size());
  if (gap_begin_ > 0)
    memcpy(&result[0], data(), gap_begin_);
  if (gap_end_ < buffer_.size())
    memcpy(&result[gap_begin_], &buffer_[gap_end_], buffer_.size() - gap_end_);
  return result;
}

std::pair<StringView, StringView> TextBuffer::GetTextForSpan(
    TextSpan* span) const {
  if (span->end() <= gap_begin_) {
    const char* begin = &buffer_[span->begin()];
    return std::make_pair(StringView(begin, begin + span->length()),
                          StringView());
  } else if (span->begin() >= gap_begin_) {
    const char* begin = &buffer_[span->begin() + gap_size()];
    // TODO(abarth): Should we handle the case where the span extends beyond the
    // buffer?
    return std::make_pair(StringView(),
                          StringView(begin, begin + span->length()));
  } else {
    // The span crosses the gap.
    const char* first_begin = &buffer_[span->begin()];
    const char* first_end = &buffer_[gap_begin_];
    const size_t first_length = first_end - first_begin;
    const char* second_begin = &buffer_[gap_end_];
    const char* second_end = second_begin + (span->length() - first_length);
    return std::make_pair(StringView(first_begin, first_end),
                          StringView(second_begin, second_end));
  }
}

void TextBuffer::AddSpan(TextSpan* span) {
  if (span->end() < gap_begin_)
    before_gap_.push(span);
  else if (span->begin() >= gap_end_)
    after_gap_.push(span);
  else
    across_gap_.push_back(span);
}

void TextBuffer::Expand(size_t required_gap_size) {
  size_t existing_gap = gap_size();
  if (existing_gap >= required_gap_size)
    return;
  size_t min_size = buffer_.size() - existing_gap + required_gap_size;
  std::vector<char> new_buffer(min_size * 1.5 + 1);
  if (gap_begin_ > 0)
    memcpy(&new_buffer[0], data(), gap_begin_);
  if (gap_end_ < buffer_.size()) {
    size_t tail_size = buffer_.size() - gap_end_;
    memcpy(&new_buffer[new_buffer.size() - tail_size],
           &buffer_[buffer_.size() - tail_size], tail_size);
  }
  gap_end_ += new_buffer.size() - buffer_.size();
  buffer_.swap(new_buffer);
}

void TextBuffer::DidInsert(size_t count) {
  for (auto& span : across_gap_)
    span->PushBack(count);
  after_gap_.ShiftForward(count);
}

void TextBuffer::DidDelete(size_t count) {
  for (auto& span : across_gap_)
    span->PopBack(count);
  after_gap_.ShiftBackward(count);
}

void TextBuffer::DidMoveInsertionPointForward() {
  std::vector<TextSpan*> displaced;
  across_gap_.swap(displaced);
  while (!after_gap_.empty() && after_gap_.top()->begin() < gap_end_) {
    TextSpan* span = after_gap_.top();
    after_gap_.pop();
    displaced.push_back(span);
  }
  across_gap_.reserve(displaced.size());
  for (auto& span : displaced)
    AddSpan(span);
}

void TextBuffer::DidMoveInsertionPointBackward() {
  std::vector<TextSpan*> displaced;
  across_gap_.swap(displaced);
  while (!before_gap_.empty() && before_gap_.top()->end() >= gap_begin_) {
    TextSpan* span = before_gap_.top();
    before_gap_.pop();
    displaced.push_back(span);
  }
  across_gap_.reserve(displaced.size());
  for (auto& span : displaced)
    AddSpan(span);
}

}  // namespace zi
