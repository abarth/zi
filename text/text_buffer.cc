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

TextBuffer::TextBuffer() = default;

TextBuffer::TextBuffer(std::vector<char> text) : buffer_(std::move(text)) {}

TextBuffer::~TextBuffer() = default;

void TextBuffer::InsertCharacter(const TextPosition& position, char c) {
  if (gap_start_ == gap_end_)
    Expand(1);
  MoveInsertionPointTo(position.offset());
  buffer_[gap_start_++] = c;
  // TODO(abarth): Consider the affinity when adjusting the TextBufferRanges.
  DidInsert(1);
}

void TextBuffer::InsertText(const TextPosition& position, StringView text) {
  const size_t length = text.length();
  if (gap_start_ + length > gap_end_)
    Expand(length);
  MoveInsertionPointTo(position.offset());
  memcpy(&buffer_[gap_start_], text.data(), length);
  gap_start_ += length;
  // TODO(abarth): Consider the affinity when adjusting the TextBufferRanges.
  DidInsert(length);
}

void TextBuffer::InsertText(const TextPosition& position,
                            const std::string& text) {
  InsertText(position, StringView(text));
}

void TextBuffer::DeleteCharacterAfter(size_t position) {
  DeleteRange(TextBufferRange(position, position + 1));
}

void TextBuffer::DeleteRange(const TextBufferRange& range) {
  const size_t size = this->size();
  const size_t begin = std::min(range.start(), size);
  const size_t end = std::min(range.end(), size);
  if (begin == end)
    return;
  const size_t count = end - begin;
  MoveInsertionPointTo(end);
  gap_start_ -= count;
  DidDelete(count);
  DidMoveInsertionPointBackward();
}

void TextBuffer::MoveInsertionPointForward(size_t offset) {
  size_t delta = std::min(offset, buffer_.size() - gap_end_);
  memmove(&buffer_[gap_start_], &buffer_[gap_end_], delta);
  gap_start_ += delta;
  gap_end_ += delta;
  DidMoveInsertionPointForward();
}

void TextBuffer::MoveInsertionPointBackward(size_t offset) {
  size_t delta = std::min(offset, gap_start_);
  gap_start_ -= delta;
  gap_end_ -= delta;
  memmove(&buffer_[gap_end_], &buffer_[gap_start_], delta);
  DidMoveInsertionPointBackward();
}

void TextBuffer::MoveInsertionPointTo(size_t position) {
  if (position > gap_start_)
    MoveInsertionPointForward(position - gap_start_);
  else if (position < gap_start_)
    MoveInsertionPointBackward(gap_start_ - position);
}

size_t TextBuffer::Find(char c, size_t pos) {
  if (pos < gap_start_) {
    char* ptr = static_cast<char*>(memchr(&buffer_[pos], c, gap_start_ - pos));
    if (ptr)
      return ptr - data();
    pos = gap_start_;
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
  if (gap_start_ > 0)
    result.replace(0, gap_start_, data(), gap_start_);
  if (gap_end_ < buffer_.size()) {
    const size_t tail_size = this->tail_size();
    result.replace(gap_start_, tail_size, &buffer_[gap_end_], tail_size);
  }
  return result;
}

TextView TextBuffer::GetText() const {
  const char* data = buffer_.data();
  return TextView(StringView(data, data + gap_start_),
                  StringView(data + gap_end_, data + buffer_.size()));
}

TextView TextBuffer::GetTextForRange(TextBufferRange* range) const {
  if (range->end() <= gap_start_) {
    const char* begin = &buffer_[range->start()];
    return TextView(StringView(begin, begin + range->length()));
  } else if (range->start() >= gap_start_) {
    const char* begin = &buffer_[range->start() + gap_size()];
    // TODO(abarth): Should we handle the case where the range extends beyond
    // the
    // buffer?
    return TextView(StringView(begin, begin + range->length()));
  } else {
    // The range crosses the gap.
    const char* first_begin = &buffer_[range->start()];
    const char* first_end = &buffer_[gap_start_];
    const size_t first_length = first_end - first_begin;
    const char* second_begin = &buffer_[gap_end_];
    const char* second_end = second_begin + (range->length() - first_length);
    return TextView(StringView(first_begin, first_end),
                    StringView(second_begin, second_end));
  }
}

void TextBuffer::AddRange(TextBufferRange* range) {
  if (range->end() <= gap_start_)
    before_gap_.push(range);
  else if (range->start() >= gap_start_)
    after_gap_.push(range);
  else
    across_gap_.push_back(range);
}

#ifndef NDEBUG

static void DebugDumpTextBufferRangeVector(const std::vector<TextBufferRange*>& ranges) {
  for (auto& range : ranges) {
    std::cout << "begin=" << range->start() << " end=" << range->end()
              << std::endl;
  }
}

void TextBuffer::DebugDumpRanges() {
  std::cout << "gap_begin=" << gap_start_ << " gap_end=" << gap_end_
            << " size=" << buffer_.size() << std::endl;
  std::cout << "== Before gap ==" << std::endl;
  DebugDumpTextBufferRangeVector(before_gap_.debug_container());
  std::cout << "== Across gap ==" << std::endl;
  DebugDumpTextBufferRangeVector(across_gap_);
  std::cout << "== After gap ==" << std::endl;
  DebugDumpTextBufferRangeVector(after_gap_.debug_container());
}

#endif

void TextBuffer::Expand(size_t required_gap_size) {
  size_t existing_gap = gap_size();
  if (existing_gap >= required_gap_size)
    return;
  size_t min_size = buffer_.size() - existing_gap + required_gap_size;
  std::vector<char> new_buffer(min_size * 1.5 + 1);
  if (gap_start_ > 0)
    memcpy(&new_buffer[0], data(), gap_start_);
  if (gap_end_ < buffer_.size()) {
    const size_t tail_size = this->tail_size();
    memcpy(&new_buffer[new_buffer.size() - tail_size],
           &buffer_[buffer_.size() - tail_size], tail_size);
  }
  gap_end_ += new_buffer.size() - buffer_.size();
  buffer_.swap(new_buffer);
}

void TextBuffer::DidInsert(size_t count) {
  for (auto& range : across_gap_)
    range->PushBack(count);
  after_gap_.ShiftForward(count);
}

void TextBuffer::DidDelete(size_t count) {
  std::vector<TextBufferRange*> displaced;
  while (!before_gap_.empty()) {
    TextBufferRange* range = before_gap_.top();
    if (range->end() <= gap_start_)
      break;
    before_gap_.pop();
    displaced.push_back(range);
    range->PopBack(range->end() - gap_start_);
  }
  AddRanges(displaced.begin(), displaced.end());
  for (auto& range : across_gap_)
    range->PopBack(count);
  after_gap_.ShiftBackward(count);
}

void TextBuffer::DidMoveInsertionPointForward() {
  std::vector<TextBufferRange*> displaced;
  across_gap_.swap(displaced);
  while (!after_gap_.empty()) {
    TextBufferRange* range = after_gap_.top();
    if (range->start() >= gap_start_)
      break;
    after_gap_.pop();
    displaced.push_back(range);
  }
  AddRanges(displaced.begin(), displaced.end());
}

void TextBuffer::DidMoveInsertionPointBackward() {
  std::vector<TextBufferRange*> displaced;
  across_gap_.swap(displaced);
  while (!before_gap_.empty()) {
    TextBufferRange* range = before_gap_.top();
    if (range->end() <= gap_start_)
      break;
    before_gap_.pop();
    displaced.push_back(range);
  }
  AddRanges(displaced.begin(), displaced.end());
}

}  // namespace zi
