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

#include <string>

namespace zi {

class StringView {
 public:
  StringView();
  StringView(const std::string& string);
  StringView(const char* begin, const char* end);
  ~StringView();

  const char* begin() const { return begin_; }
  const char* end() const { return end_; }

  const char* data() const { return begin_; }
  size_t length() const { return end_ - begin_; }

  bool is_empty() const { return begin_ == end_; }

  std::string ToString() const;

 private:
  const char* begin_ = nullptr;
  const char* end_ = nullptr;
};

}  // namespace zi
