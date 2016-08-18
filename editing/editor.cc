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

#include "editing/editor.h"

#include <algorithm>
#include <utility>

#include "terminal/term.h"

namespace zi {

Editor::Editor() {}

Editor::~Editor() {}

void Editor::SetText(std::unique_ptr<TextBuffer> text) {
  text_ = std::move(text);
  lines_.Clear();
}

void Editor::Display(CommandBuffer* commands) {
  lines_.UpdateLines(text_.get());
  for (size_t i = 0; i < lines_.size(); ++i) {
    commands->MoveCursorTo(0, i);
    *commands << text_->GetTextForRange(lines_.GetLine(i + base_line_));
  }
  if (height_ > lines_.size()) {
    *commands << term::kSetLowIntensity;
    for (size_t i = lines_.size(); i < height_; ++i) {
      commands->MoveCursorTo(0, i);
      *commands << "~" << term::kEraseToEndOfLine;
    }
    *commands << term::kClearCharacterAttributes;
  }
}

void Editor::UpdateCursor(CommandBuffer* commands) {
  commands->MoveCursorTo(cursor_col_, cursor_row_ - base_line_);
}

void Editor::Resize(size_t width, size_t height) {
  width_ = width;
  height_ = height;
}

void Editor::ScrollTo(size_t line) {
  base_line_ = line;
}

void Editor::ScrollBy(int delta) {
  if (delta < 0)
    ScrollTo(base_line_ - std::min(base_line_, static_cast<size_t>(-delta)));
  else
    ScrollTo(base_line_ + delta);
}

void Editor::InsertCharacter(char c) {
  text_->InsertCharacter(GetCurrentTextPosition(), c);
  SetCursorColumn(cursor_col_ + 1);
}

void Editor::InsertLineBreak() {
  text_->InsertCharacter(GetCurrentTextPosition(), '\n');
  ++cursor_row_;
  SetCursorColumn(0);
}

bool Editor::Backspace() {
  size_t position = GetCurrentTextPosition();
  if (position > 0) {
    if (cursor_col_ > 0) {
      SetCursorColumn(cursor_col_ - 1);
    } else {
      // We must not be in the first row because our position is non-zero and
      // our column is zero.
      --cursor_row_;
      SetCursorColumn(GetMaxCursorColumn());
    }
    text_->DeleteRange(TextRange(position - 1, position));
    return true;
  }
  return false;
}

void Editor::SetCursorMode(CursorMode mode) {
  cursor_mode_ = mode;
  cursor_col_ = std::min(cursor_col_, GetMaxCursorColumn());
}

bool Editor::MoveCursorLeft() {
  // TODO(abarth): Handle RTL.
  if (cursor_col_ > 0) {
    SetCursorColumn(cursor_col_ - 1);
    return true;
  }
  return false;
}

bool Editor::MoveCursorDown() {
  if (cursor_row_ + 1 < lines_.size()) {
    ++cursor_row_;
    EnsureCursorVisible();
    cursor_col_ = std::min(preferred_cursor_col_, GetMaxCursorColumn());
    return true;
  }
  return false;
}

bool Editor::MoveCursorUp() {
  if (cursor_row_ > 0) {
    --cursor_row_;
    EnsureCursorVisible();
    cursor_col_ = std::min(preferred_cursor_col_, GetMaxCursorColumn());
    return true;
  }
  return false;
}

bool Editor::MoveCursorRight() {
  // TODO(abarth): Handle RTL.
  if (cursor_col_ < GetMaxCursorColumn()) {
    SetCursorColumn(cursor_col_ + 1);
    return true;
  }
  return false;
}

void Editor::EnsureCursorVisible() {
  if (cursor_row_ < base_line_)
    ScrollTo(cursor_row_);
  else if (cursor_row_ > base_line_ + height_)
    ScrollTo(cursor_row_ - height_ + 1);
}

TextRange* Editor::GetCurrentLine() const {
  return lines_.GetLine(cursor_row_);
}

size_t Editor::GetMaxCursorColumn() const {
  size_t length = GetCurrentLine()->length();
  if (length > 0 && cursor_mode_ == CursorMode::Block)
    length -= 1;
  return length;
}

size_t Editor::GetCurrentTextPosition() {
  return GetCurrentLine()->start() + cursor_col_;
}

void Editor::SetCursorColumn(size_t column) {
  cursor_col_ = column;
  preferred_cursor_col_ = column;
}

}  // namespace zi
