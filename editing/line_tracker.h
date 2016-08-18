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

#include "text/text_buffer.h"
#include "text/text_span.h"
#include "zen/macros.h"

namespace zi {

class LineTracker {
 public:
  LineTracker();
  ~LineTracker();

  void Clear();
  void UpdateLines(TextBuffer* text);
  TextSpan* GetLine(size_t line_index) const;

  size_t size() const { return lines_.size(); }

 private:
  std::vector<std::unique_ptr<TextSpan>> lines_;

  DISALLOW_COPY_AND_ASSIGN(LineTracker);
};

}  // namespace zi
