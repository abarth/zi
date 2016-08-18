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

#include "zen/string_view.h"

namespace zi {

class TextView {
 public:
  TextView();
  TextView(const std::string& string);
  TextView(const StringView& view);
  TextView(const StringView& left, const StringView& right);
  ~TextView();

  size_t length() const { return left_.length() + right_.length(); }

  const StringView& left() const { return left_; }
  const StringView& right() const { return right_; }

  bool is_empty() const { return left_.is_empty() && right_.is_empty(); }

  std::string ToString() const;

 private:
  StringView left_;
  StringView right_;
};

}  // namespace zi
