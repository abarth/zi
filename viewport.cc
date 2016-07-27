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

#include "viewport.h"

#include <algorithm>
#include <utility>

#include "term.h"

namespace zi {

Viewport::Viewport() {}

Viewport::~Viewport() {}

void Viewport::SetText(std::unique_ptr<TextBuffer> text) {
  text_ = std::move(text);
  lines_.Clear();
}

void Viewport::Display(CommandBuffer* commands) {
  lines_.UpdateLines(text_.get());
  for (size_t i = 0; i < lines_.size(); ++i) {
    commands->MoveCursorTo(0, i);
    *commands << text_->GetTextForSpan(lines_.GetLine(i + base_line_));
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

void Viewport::UpdateCursor(CommandBuffer* commands) {
  commands->MoveCursorTo(cursor_col_, cursor_row_ - base_line_);
}

void Viewport::Resize(size_t width, size_t height) {
  width_ = width;
  height_ = height;
}

void Viewport::ScrollTo(size_t line) {
  base_line_ = line;
}

void Viewport::ScrollBy(int delta) {
  if (delta < 0)
    ScrollTo(base_line_ - std::min(base_line_, static_cast<size_t>(-delta)));
  else
    ScrollTo(base_line_ + delta);
}

void Viewport::MoveCursorLeft() {
  // TODO(abarth): Handle RTL.
  if (cursor_col_ > 0) {
    --cursor_col_;
    preferred_cursor_col_ = cursor_col_;
    UpdateTextCursor();
  } else {
    term::Put(term::kBell);
  }
}

void Viewport::MoveCursorDown() {
  if (cursor_row_ + 1 < lines_.size()) {
    ++cursor_row_;
    EnsureCursorVisible();
    cursor_col_ = std::min(preferred_cursor_col_, GetMaxCursorColumn());
    UpdateTextCursor();
  } else {
    term::Put(term::kBell);
  }
}

void Viewport::MoveCursorUp() {
  if (cursor_row_ > 0) {
    --cursor_row_;
    EnsureCursorVisible();
    cursor_col_ = std::min(preferred_cursor_col_, GetMaxCursorColumn());
    UpdateTextCursor();
  } else {
    term::Put(term::kBell);
  }
}

void Viewport::MoveCursorRight() {
  // TODO(abarth): Handle RTL.
  if (cursor_col_ < GetMaxCursorColumn()) {
    ++cursor_col_;
    preferred_cursor_col_ = cursor_col_;
    UpdateTextCursor();
  } else {
    term::Put(term::kBell);
  }
}

void Viewport::EnsureCursorVisible() {
  if (cursor_row_ < base_line_)
    ScrollTo(cursor_row_);
  else if (cursor_row_ > base_line_ + height_)
    ScrollTo(cursor_row_ - height_ + 1);
}

TextSpan* Viewport::GetCurrentLine() const {
  return lines_.GetLine(cursor_row_);
}

size_t Viewport::GetMaxCursorColumn() const {
  size_t length = GetCurrentLine()->length();
  if (length > 0)
    length -= 1;
  return length;
}

void Viewport::UpdateTextCursor() {
  text_->MoveCursorTo(GetCurrentLine()->begin() + cursor_col_);
}

}  // namespace zi
