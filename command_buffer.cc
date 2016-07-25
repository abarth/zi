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

#include "command_buffer.h"

namespace zi {

CommandBuffer::CommandBuffer() {}

CommandBuffer::~CommandBuffer() {}

CommandBuffer& CommandBuffer::operator<<(const StringView& text) {
  Write(text.data(), text.length());
  return *this;
}

CommandBuffer& CommandBuffer::operator<<(
    std::pair<StringView, StringView> text) {
  if (!text.first.is_empty())
    Write(text.first.data(), text.first.length());
  if (!text.second.is_empty())
    Write(text.second.data(), text.second.length());
  return *this;
}

void CommandBuffer::Write(const char* buffer, size_t length) {
  stream_.write(buffer, length);
}

void CommandBuffer::MoveCursorTo(int x, int y) {
  stream_ << ESC "[" << y + 1 << ";" << x + 1 << "H";
}

void CommandBuffer::SetForegroundColor(term::Color color) {
  stream_ << ESC "[" << term::kForegroundColors[static_cast<int>(color)] << "m";
}

void CommandBuffer::SetBackgroundColor(term::Color color) {
  stream_ << ESC "[" << term::kBackgroundColors[static_cast<int>(color)] << "m";
}

void CommandBuffer::Execute() {
  term::Put(stream_.str());
}

}  // namespace zi
