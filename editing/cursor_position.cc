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

#include "editing/cursor_position.h"

#include <algorithm>

#include "text/text_buffer.h"

namespace zi {
namespace {

size_t CursorLengthForMode(CursorMode mode) {
  switch (mode) {
    case CursorMode::Block:
      return 1;
    case CursorMode::Line:
      return 0;
  }
}

}  // namespace

CursorPosition::CursorPosition(TextBuffer* text) : text_(text) {}

CursorPosition::~CursorPosition() = default;

bool CursorPosition::MoveLeft() {
  if (offset() == 0u || text_->At(offset() - 1) == '\n')
    return false;
  selection_.Shift(-1);
  current_column_ -= 1;
  preferred_column_ = current_column_;
  return true;
}

bool CursorPosition::MoveRight() {
  if (selection_.end_offset() >= text_->size() ||
      text_->At(selection_.end_offset()) == '\n')
    return false;
  selection_.Shift(1);
  current_column_ += 1;
  preferred_column_ = current_column_;
  return true;
}

bool CursorPosition::MoveUp() {
  return false;
}

bool CursorPosition::MoveDown() {
  size_t end_of_current_line = text_->Find('\n', offset());
  if (end_of_current_line == std::string::npos ||
      end_of_current_line + 1 == text_->size())
    return false;
  size_t next_line_break = text_->Find('\n', end_of_current_line + 1);
  size_t end_of_next_line = std::min(next_line_break, text_->size());
  size_t column_limit = end_of_next_line - end_of_current_line - 1;
  if (column_limit > 0 && mode_ == CursorMode::Block)
    column_limit -= 1;
  current_column_ = std::min(preferred_column_, column_limit);
  MoveCursorTo(end_of_current_line + current_column_);
  return true;
}

void CursorPosition::SetMode(CursorMode mode) {
  if (mode_ == mode)
    return;
  mode_ = mode;
  MoveCursorTo(offset());
  if (mode_ == CursorMode::Block && text_->At(offset()) == '\n')
    MoveLeft();
}

bool CursorPosition::SetOffset(size_t offset) {
  if (offset + CursorLengthForMode(mode_) >= text_->size())
    return false;
  MoveCursorTo(offset);
  return true;
}

void CursorPosition::MoveCursorTo(size_t offset) {
  selection_.SetRange(TextRange(offset, offset + CursorLengthForMode(mode_)));
}

}  // namespace zi
