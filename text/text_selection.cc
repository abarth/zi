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

#include "text/text_selection.h"

#include "text/text_range.h"

namespace zi {

TextSelection::TextSelection() = default;

TextSelection::TextSelection(size_t base_offset,
                             size_t extent_offset,
                             TextAffinity affinity)
    : base_offset_(base_offset),
      extent_offset_(extent_offset),
      affinity_(affinity) {}

TextSelection::TextSelection(const TextRange& range, TextAffinity affinity)
    : TextSelection(range.start(), range.end(), affinity) {}

TextSelection::~TextSelection() = default;

}  // namespace zi
