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

#include "text/text_span.h"

#include <string>

#include "gtest/gtest.h"

namespace zi {
namespace {

TEST(TextSpan, Default) {
  TextSpan span;
  EXPECT_TRUE(span.is_empty());
  EXPECT_EQ(0u, span.begin());
  EXPECT_EQ(0u, span.end());
  EXPECT_EQ(0u, span.length());
  EXPECT_FALSE(span.is_dirty());
}

TEST(TextSpan, Control) {
  TextSpan span(4, 7);
  EXPECT_FALSE(span.is_empty());
  EXPECT_EQ(4u, span.begin());
  EXPECT_EQ(7u, span.end());
  EXPECT_EQ(3u, span.length());
  EXPECT_FALSE(span.is_dirty());

  span.ShiftForward(2);
  EXPECT_EQ(6u, span.begin());
  EXPECT_EQ(9u, span.end());
  EXPECT_FALSE(span.is_dirty());

  span.ShiftBackward(1);
  EXPECT_EQ(5u, span.begin());
  EXPECT_EQ(8u, span.end());
  EXPECT_FALSE(span.is_dirty());

  span.PushFront(3);
  EXPECT_EQ(2u, span.begin());
  EXPECT_EQ(8u, span.end());
  EXPECT_TRUE(span.is_dirty());
  span.MarkClean();
  EXPECT_FALSE(span.is_dirty());

  span.PushFront(3);
  EXPECT_EQ(0u, span.begin());
  EXPECT_EQ(8u, span.end());
  EXPECT_TRUE(span.is_dirty());
  span.MarkClean();
  EXPECT_FALSE(span.is_dirty());

  span.ShiftBackward(1);
  EXPECT_EQ(0u, span.begin());
  EXPECT_EQ(8u, span.end());
  EXPECT_FALSE(span.is_dirty());

  span.PopFront(2);
  EXPECT_EQ(2u, span.begin());
  EXPECT_EQ(8u, span.end());
  EXPECT_TRUE(span.is_dirty());
  span.MarkClean();
  EXPECT_FALSE(span.is_dirty());

  span.PopFront(7);
  EXPECT_EQ(8u, span.begin());
  EXPECT_EQ(8u, span.end());
  EXPECT_TRUE(span.is_dirty());
  EXPECT_TRUE(span.is_empty());
  span.MarkClean();
  EXPECT_FALSE(span.is_dirty());

  span.PushBack(5);
  EXPECT_EQ(8u, span.begin());
  EXPECT_EQ(13u, span.end());
  EXPECT_TRUE(span.is_dirty());
  span.MarkClean();
  EXPECT_FALSE(span.is_dirty());

  span.PopBack(4);
  EXPECT_EQ(8u, span.begin());
  EXPECT_EQ(9u, span.end());
  EXPECT_TRUE(span.is_dirty());
  span.MarkClean();
  EXPECT_FALSE(span.is_dirty());

  span.PopBack(4);
  EXPECT_EQ(8u, span.begin());
  EXPECT_EQ(8u, span.end());
  EXPECT_TRUE(span.is_dirty());
  span.MarkClean();
  EXPECT_FALSE(span.is_dirty());
}

}  // namespace
}  // namespace zi
