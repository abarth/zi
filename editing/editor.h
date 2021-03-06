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

#include <memory>
#include <vector>

#include "editing/cursor_mode.h"
#include "editing/line_tracker.h"
#include "terminal/command_buffer.h"
#include "text/text_buffer.h"
#include "text/text_position.h"
#include "text/text_buffer_range.h"
#include "zen/macros.h"

namespace zi {

class Editor {
 public:
  Editor();
  ~Editor();

  void SetText(std::unique_ptr<TextBuffer> text);

  void Display(CommandBuffer* commands);
  void UpdateCursor(CommandBuffer* commands);

  void Resize(size_t width, size_t height);
  void ScrollTo(size_t first_line);
  void ScrollBy(int delta);

  TextBuffer* text() const { return text_.get(); }
  size_t width() const { return width_; }
  size_t height() const { return height_; }

  void InsertCharacter(char c);
  void InsertLineBreak();
  bool Backspace();

  void SetCursorMode(CursorMode mode);

  bool MoveCursorLeft();
  bool MoveCursorDown();
  bool MoveCursorUp();
  bool MoveCursorRight();

 private:
  TextBufferRange* GetCurrentLine() const;
  size_t GetMaxCursorColumn() const;
  void EnsureCursorVisible();
  TextPosition GetCurrentTextPosition();
  void UpdateLines();

  void SetCursorColumn(size_t column);

  std::unique_ptr<TextBuffer> text_;
  LineTracker lines_;

  size_t width_ = 0;
  size_t height_ = 0;

  size_t base_line_ = 0;

  CursorMode cursor_mode_ = CursorMode::Block;
  size_t cursor_col_ = 0;
  size_t cursor_row_ = 0;
  size_t preferred_cursor_col_ = 0;

  DISALLOW_COPY_AND_ASSIGN(Editor);
};

}  // namespace zi
