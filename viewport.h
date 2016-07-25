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

#include "command_buffer.h"
#include "macros.h"
#include "text_span.h"
#include "text_buffer.h"

namespace zi {

class Viewport {
 public:
  Viewport();
  ~Viewport();

  void SetText(std::unique_ptr<TextBuffer> text);
  void Display(CommandBuffer* commands);

  void Resize(size_t width, size_t height);
  void ScrollTo(size_t first_line);
  void ScrollBy(int delta);

  TextBuffer* text() const { return text_.get(); }
  size_t width() const { return width_; }
  size_t height() const { return height_; }
  size_t first_line() const { return first_line_; }

 private:
  void UpdateLines();

  std::unique_ptr<TextBuffer> text_;
  std::vector<std::unique_ptr<TextSpan>> lines_;

  size_t width_ = 0;
  size_t height_ = 0;
  size_t first_line_ = 0;

  DISALLOW_COPY_AND_ASSIGN(Viewport);
};

}  // namespace zi
