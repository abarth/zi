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

#include <string>
#include <utility>
#include <vector>

#include "macros.h"
#include "string_view.h"
#include "text_span_queue.h"
#include "text_span.h"
#include "vector_extensions.h"

namespace zi {

class TextBuffer {
 public:
  TextBuffer();
  explicit TextBuffer(std::vector<char> text);
  ~TextBuffer();

  void InsertCharacter(char c);
  void InsertText(std::string text);
  void Backspace();

  void MoveCursorForward();
  void MoveCursorBackward();

  void MoveCursorBy(int offset);
  void MoveCursorTo(size_t position);

  // Returns std::string::npos if |c| is not found.
  size_t Find(char c, size_t pos = 0);

  size_t cursor_position() const { return gap_begin_; }

  bool is_empty() const { return size() == 0u; }
  size_t size() const { return buffer_.size() - gap_size(); }

  std::pair<StringView, StringView> GetTextForSpan(TextSpan* span) const;

  void AddSpan(TextSpan* span);

  template <typename Iterator>
  void RemoveSpans(Iterator begin, Iterator end) {
    before_gap_.Erase(begin, end);
    EraseAllValues(across_gap_, begin, end);
    after_gap_.Erase(begin, end);
  }

 private:
  void Expand(size_t required_gap_size);
  void DidInsert(size_t count);
  void DidDelete(size_t count);
  void DidMoveCursorForward();
  void DidMoveCursorBackward();

  const char* data() const { return &buffer_[0]; }
  size_t gap_size() const { return gap_end_ - gap_begin_; }

  size_t gap_begin_ = 0;
  size_t gap_end_ = 0;
  std::vector<char> buffer_;

  TextSpanQueue<TextSpan::AscendingByEnd> before_gap_;
  std::vector<TextSpan*> across_gap_;
  TextSpanQueue<TextSpan::DescendingByBegin> after_gap_;

  DISALLOW_COPY_AND_ASSIGN(TextBuffer);
};

}  // namespace zi
