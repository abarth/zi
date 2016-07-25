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
}

void Viewport::Display(CommandBuffer* commands) {
  lines_.clear();
  UpdateLines();
  for (size_t i = 0; i < lines_.size(); ++i) {
    commands->MoveCursorTo(0, i);
    *commands << text_->GetTextForSpan(lines_[i].get());
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

void Viewport::Resize(size_t width, size_t height) {
  width_ = width;
  height_ = height;
}

void Viewport::ScrollTo(size_t first_line) {
  first_line_ = first_line;
}

void Viewport::ScrollBy(int delta) {
  if (delta < 0)
    ScrollTo(first_line_ - std::min(first_line_, static_cast<size_t>(-delta)));
  else
    ScrollTo(first_line_ + delta);
}

void Viewport::UpdateLines() {
  size_t offset = 0;
  for (size_t i = 0; i < first_line_; ++i) {
    offset = text_->Find('\n', offset);
    if (offset == std::string::npos)
      return;
    ++offset;
  }
  for (size_t i = 0; i < height_; ++i) {
    size_t end = text_->Find('\n', offset);
    std::unique_ptr<TextSpan> line;
    if (end == std::string::npos) {
      if (offset < text_->size())
        lines_.emplace_back(new TextSpan(offset, text_->size()));
      return;
    }
    lines_.emplace_back(new TextSpan(offset, end + 1));
    offset = end + 1;
  }
}

}  // namespace zi
