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

#include "text/text_range.h"

#include <string>

#include "gtest/gtest.h"

namespace zi {
namespace {

TEST(TextRange, Default) {
  TextRange range;
  EXPECT_TRUE(range.is_empty());
  EXPECT_EQ(0u, range.begin());
  EXPECT_EQ(0u, range.end());
  EXPECT_EQ(0u, range.length());
  EXPECT_FALSE(range.is_dirty());
}

TEST(TextRange, Control) {
  TextRange range(4, 7);
  EXPECT_FALSE(range.is_empty());
  EXPECT_EQ(4u, range.begin());
  EXPECT_EQ(7u, range.end());
  EXPECT_EQ(3u, range.length());
  EXPECT_FALSE(range.is_dirty());

  range.ShiftForward(2);
  EXPECT_EQ(6u, range.begin());
  EXPECT_EQ(9u, range.end());
  EXPECT_FALSE(range.is_dirty());

  range.ShiftBackward(1);
  EXPECT_EQ(5u, range.begin());
  EXPECT_EQ(8u, range.end());
  EXPECT_FALSE(range.is_dirty());

  range.PushFront(3);
  EXPECT_EQ(2u, range.begin());
  EXPECT_EQ(8u, range.end());
  EXPECT_TRUE(range.is_dirty());
  range.MarkClean();
  EXPECT_FALSE(range.is_dirty());

  range.PushFront(3);
  EXPECT_EQ(0u, range.begin());
  EXPECT_EQ(8u, range.end());
  EXPECT_TRUE(range.is_dirty());
  range.MarkClean();
  EXPECT_FALSE(range.is_dirty());

  range.ShiftBackward(1);
  EXPECT_EQ(0u, range.begin());
  EXPECT_EQ(8u, range.end());
  EXPECT_FALSE(range.is_dirty());

  range.PopFront(2);
  EXPECT_EQ(2u, range.begin());
  EXPECT_EQ(8u, range.end());
  EXPECT_TRUE(range.is_dirty());
  range.MarkClean();
  EXPECT_FALSE(range.is_dirty());

  range.PopFront(7);
  EXPECT_EQ(9u, range.begin());
  EXPECT_EQ(9u, range.end());
  EXPECT_TRUE(range.is_dirty());
  EXPECT_TRUE(range.is_empty());
  range.MarkClean();
  EXPECT_FALSE(range.is_dirty());

  range.PushBack(5);
  EXPECT_EQ(9u, range.begin());
  EXPECT_EQ(14u, range.end());
  EXPECT_TRUE(range.is_dirty());
  range.MarkClean();
  EXPECT_FALSE(range.is_dirty());

  range.PopBack(4);
  EXPECT_EQ(9u, range.begin());
  EXPECT_EQ(10u, range.end());
  EXPECT_TRUE(range.is_dirty());
  range.MarkClean();
  EXPECT_FALSE(range.is_dirty());

  range.PopBack(4);
  EXPECT_EQ(6u, range.begin());
  EXPECT_EQ(6u, range.end());
  EXPECT_TRUE(range.is_dirty());
  range.MarkClean();
  EXPECT_FALSE(range.is_dirty());
}

}  // namespace
}  // namespace zi
