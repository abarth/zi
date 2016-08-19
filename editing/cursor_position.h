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

#include "editing/cursor_mode.h"
#include "text/text_selection.h"
#include "zen/macros.h"

namespace zi {
class TextBuffer;

class CursorPosition {
 public:
  explicit CursorPosition(TextBuffer* text);
  ~CursorPosition();

  bool MoveLeft();
  bool MoveRight();
  bool MoveUp();
  bool MoveDown();

  void SetMode(CursorMode mode);
  bool SetOffset(size_t offset);

  CursorMode mode() const { return mode_; }
  const TextSelection& selection() const { return selection_; }
  size_t offset() const { return selection_.base_offset(); }

 private:
  void MoveCursorTo(size_t offset);

  TextBuffer* text_;
  CursorMode mode_;
  TextSelection selection_;
  size_t current_column_ = 0;
  size_t preferred_column_ = 0;

  DISALLOW_COPY_AND_ASSIGN(CursorPosition);
};

}  // namespace zi
