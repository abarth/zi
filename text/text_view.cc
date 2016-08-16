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

#include "text/text_view.h"

namespace zi {

TextView::TextView() {}

TextView::TextView(const std::string& string) : left_(string) {}

TextView::TextView(const StringView& string_view) : left_(string_view) {}

TextView::TextView(const StringView& left, const StringView& right)
    : left_(left), right_(right) {}

TextView::~TextView() {}

std::string TextView::ToString() const {
  std::string result;
  const size_t left_length = left_.length();
  const size_t right_length = right_.length();
  result.resize(left_length + right_length);
  if (left_length)
    result.replace(0, left_length, left_.data(), left_length);
  if (right_length)
    result.replace(left_length, right_length, right_.data(), right_length);
  return result;
}

}  // namespace zi
