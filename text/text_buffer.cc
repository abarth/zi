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

#include "text/text_buffer.h"

#include <string.h>

#include <algorithm>
#include <utility>

#ifndef NDEBUG
#include <iostream>
#endif

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

void TextBuffer::InsertText(size_t position, StringView text) {
  const size_t length = text.length();
  if (gap_begin_ + length > gap_end_)
    Expand(length);
  MoveInsertionPointTo(position);
  memcpy(&buffer_[gap_begin_], text.data(), length);
  gap_begin_ += length;
  DidInsert(length);
}

void TextBuffer::InsertText(size_t position, std::string text) {
  InsertText(position, StringView(text));
}

void TextBuffer::DeleteCharacterAfter(size_t position) {
  DeleteSpan(TextSpan(position, position + 1));
}

void TextBuffer::DeleteSpan(const TextSpan& span) {
  const size_t size = this->size();
  const size_t begin = std::min(span.begin(), size);
  const size_t end = std::min(span.end(), size);
  if (begin == end)
    return;
  const size_t count = end - begin;
  MoveInsertionPointTo(end);
  gap_begin_ -= count;
  DidDelete(count);
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
    result.replace(0, gap_begin_, data(), gap_begin_);
  if (gap_end_ < buffer_.size()) {
    const size_t tail_size = this->tail_size();
    result.replace(gap_begin_, tail_size, &buffer_[gap_end_], tail_size);
  }
  return result;
}

TextView TextBuffer::GetText() const {
  const char* data = buffer_.data();
  return TextView(StringView(data, data + gap_begin_),
                  StringView(data + gap_end_, data + buffer_.size()));
}

TextView TextBuffer::GetTextForSpan(TextSpan* span) const {
  if (span->end() <= gap_begin_) {
    const char* begin = &buffer_[span->begin()];
    return TextView(StringView(begin, begin + span->length()));
  } else if (span->begin() >= gap_begin_) {
    const char* begin = &buffer_[span->begin() + gap_size()];
    // TODO(abarth): Should we handle the case where the span extends beyond the
    // buffer?
    return TextView(StringView(begin, begin + span->length()));
  } else {
    // The span crosses the gap.
    const char* first_begin = &buffer_[span->begin()];
    const char* first_end = &buffer_[gap_begin_];
    const size_t first_length = first_end - first_begin;
    const char* second_begin = &buffer_[gap_end_];
    const char* second_end = second_begin + (span->length() - first_length);
    return TextView(StringView(first_begin, first_end),
                    StringView(second_begin, second_end));
  }
}

void TextBuffer::AddSpan(TextSpan* span) {
  if (span->end() <= gap_begin_)
    before_gap_.push(span);
  else if (span->begin() >= gap_begin_)
    after_gap_.push(span);
  else
    across_gap_.push_back(span);
}

#ifndef NDEBUG

static void DebugDumpTextSpanVector(const std::vector<TextSpan*>& spans) {
  for (auto& span : spans) {
    std::cout << "begin=" << span->begin() << " end=" << span->end()
              << std::endl;
  }
}

void TextBuffer::DebugDumpSpans() {
  std::cout << "gap_begin=" << gap_begin_ << " gap_end=" << gap_end_
            << " size=" << buffer_.size() << std::endl;
  std::cout << "== Before gap ==" << std::endl;
  DebugDumpTextSpanVector(before_gap_.debug_container());
  std::cout << "== Across gap ==" << std::endl;
  DebugDumpTextSpanVector(across_gap_);
  std::cout << "== After gap ==" << std::endl;
  DebugDumpTextSpanVector(after_gap_.debug_container());
}

#endif

void TextBuffer::Expand(size_t required_gap_size) {
  size_t existing_gap = gap_size();
  if (existing_gap >= required_gap_size)
    return;
  size_t min_size = buffer_.size() - existing_gap + required_gap_size;
  std::vector<char> new_buffer(min_size * 1.5 + 1);
  if (gap_begin_ > 0)
    memcpy(&new_buffer[0], data(), gap_begin_);
  if (gap_end_ < buffer_.size()) {
    const size_t tail_size = this->tail_size();
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
  std::vector<TextSpan*> displaced;
  while (!before_gap_.empty()) {
    TextSpan* span = before_gap_.top();
    if (span->end() <= gap_begin_)
      break;
    before_gap_.pop();
    displaced.push_back(span);
    span->PopBack(span->end() - gap_begin_);
  }
  AddSpans(displaced.begin(), displaced.end());
  for (auto& span : across_gap_)
    span->PopBack(count);
  after_gap_.ShiftBackward(count);
}

void TextBuffer::DidMoveInsertionPointForward() {
  std::vector<TextSpan*> displaced;
  across_gap_.swap(displaced);
  while (!after_gap_.empty()) {
    TextSpan* span = after_gap_.top();
    if (span->begin() >= gap_begin_)
      break;
    after_gap_.pop();
    displaced.push_back(span);
  }
  AddSpans(displaced.begin(), displaced.end());
}

void TextBuffer::DidMoveInsertionPointBackward() {
  std::vector<TextSpan*> displaced;
  across_gap_.swap(displaced);
  while (!before_gap_.empty()) {
    TextSpan* span = before_gap_.top();
    if (span->end() <= gap_begin_)
      break;
    before_gap_.pop();
    displaced.push_back(span);
  }
  AddSpans(displaced.begin(), displaced.end());
}

}  // namespace zi
