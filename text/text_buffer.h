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
#include <vector>

#include "zen/string_view.h"
#include "text/text_range_queue.h"
#include "text/text_range.h"
#include "text/text_view.h"
#include "zen/macros.h"
#include "zen/vector_extensions.h"

namespace zi {

class TextBuffer {
 public:
  TextBuffer();
  explicit TextBuffer(std::vector<char> text);
  ~TextBuffer();

  void InsertCharacter(size_t position, char c);
  void InsertText(size_t position, StringView text);
  void InsertText(size_t position, std::string text);
  void DeleteCharacterAfter(size_t position);
  void DeleteRange(const TextRange& range);

  // Returns std::string::npos if |c| is not found.
  size_t Find(char c, size_t pos = 0u);

  bool is_empty() const { return size() == 0u; }
  size_t size() const { return buffer_.size() - gap_size(); }

  std::string ToString() const;
  TextView GetText() const;
  TextView GetTextForRange(TextRange* range) const;

  void AddRange(TextRange* range);

  template <typename Iterator>
  void AddRanges(Iterator begin, Iterator end) {
    for (Iterator it = begin; it != end; ++it)
      AddRange(*it);
  }

  template <typename Iterator>
  void RemoveRanges(Iterator begin, Iterator end) {
    before_gap_.Erase(begin, end);
    EraseAllValues(across_gap_, begin, end);
    after_gap_.Erase(begin, end);
  }

#ifndef NDEBUG
  void DebugDumpRanges();
#endif

 private:
  void MoveInsertionPointTo(size_t position);
  void MoveInsertionPointForward(size_t offset);
  void MoveInsertionPointBackward(size_t offset);
  void DidMoveInsertionPointForward();
  void DidMoveInsertionPointBackward();

  void Expand(size_t required_gap_size);
  void DidInsert(size_t count);
  void DidDelete(size_t count);

  const char* data() const { return buffer_.data(); }
  size_t gap_size() const { return gap_end_ - gap_begin_; }
  size_t tail_size() const { return buffer_.size() - gap_end_; }

  size_t gap_begin_ = 0;
  size_t gap_end_ = 0;
  std::vector<char> buffer_;

  TextRangeQueue<TextRange::AscendingByEnd> before_gap_;
  std::vector<TextRange*> across_gap_;
  TextRangeQueue<TextRange::DescendingByBegin> after_gap_;

  DISALLOW_COPY_AND_ASSIGN(TextBuffer);
};

}  // namespace zi
