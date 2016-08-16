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

#include <vector>

namespace zi {

template <typename T>
bool EraseValue(const std::vector<T> vector, const T& value) {
  auto end = vector.end();
  auto it = std::find(vector.begin(), end, value);
  if (it != end) {
    vector.erase(it);
    return true;
  }
  return false;
}

template <typename T, typename Iterator>
bool EraseAllValues(const std::vector<T> vector, Iterator begin, Iterator end) {
  bool modified = false;
  for (Iterator it = begin; it != end; ++it) {
    if (EraseValue(vector, *it))
      modified = true;
  }
  return modified;
}

}  // namespace zi
