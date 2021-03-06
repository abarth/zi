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

#include <sstream>
#include <utility>

#include "terminal/term.h"
#include "zen/string_view.h"
#include "text/text_view.h"
#include "zen/macros.h"

namespace zi {

class CommandBuffer {
 public:
  CommandBuffer();
  ~CommandBuffer();

  CommandBuffer& operator<<(const StringView& text);
  CommandBuffer& operator<<(const TextView& text);

  template <typename T>
  CommandBuffer& operator<<(const T& value) {
    stream_ << value;
    return *this;
  }

  void Write(const char* buffer, size_t length);
  void MoveCursorTo(int x, int y);
  void SetForegroundColor(term::Color color);
  void SetBackgroundColor(term::Color color);
  void Execute();

 private:
  std::ostringstream stream_;

  DISALLOW_COPY_AND_ASSIGN(CommandBuffer);
};

}  // namespace zi
