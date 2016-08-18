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

#include "editing/line_tracker.h"

namespace zi {

LineTracker::LineTracker() {}

LineTracker::~LineTracker() {}

void LineTracker::Clear() {
  lines_.clear();
}

void LineTracker::UpdateLines(TextBuffer* text) {
  Clear();
  if (lines_.empty()) {
    size_t offset = 0;
    const size_t text_size = text->size();
    while (offset < text_size) {
      size_t end = text->Find('\n', offset);
      std::unique_ptr<TextRange> line;
      if (end == std::string::npos) {
        if (offset < text_size)
          lines_.emplace_back(new TextRange(offset, text_size));
        return;
      }
      lines_.emplace_back(new TextRange(offset, end));
      offset = end + 1;
    }
    // } else {
    //   std::vector<std::unique_ptr<TextRange>> updated_lines;
    //   updated_lines.reserve(lines_.size());
    //   for (int i = 0; i < lines_.size(); ++i) {
    //     auto& line = lines_[i];
    //     if (!line.is_dirty()) {
    //       updated_lines.push_back(std::move(line));
    //     } else {
    //       size_t offset = line.begin();
    //       size_t limit = i + 1 < lines_.size() ? lines_[i].begin() :
    //       text->size();
    //       while (offset < limit) {
    //         size_t end = text_->Find('\n', offset);
    //         if (end )
    //       }
    //     }
    //   }
  }
}

TextRange* LineTracker::GetLine(size_t line_index) const {
  return lines_[line_index].get();
}

}  // namespace zi
