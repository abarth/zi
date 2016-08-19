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

#include "text/text_buffer_range_queue.h"

#include <string>

#include "gtest/gtest.h"

namespace zi {
namespace {

TEST(TextBufferRangeQueue, Control) {
  TextBufferRangeQueue<TextBufferRange::DescendingByStart> queue;
  TextBufferRange a(1, 10);
  TextBufferRange b(2, 9);
  TextBufferRange c(3, 11);

  queue.push(&a);
  queue.push(&b);
  queue.push(&c);
  EXPECT_EQ(1u, queue.top()->start());

  queue.ShiftForward(5);
  EXPECT_EQ(6u, queue.top()->start());
  EXPECT_EQ(6u, a.start());
  EXPECT_EQ(15u, a.end());
  EXPECT_EQ(7u, b.start());
  EXPECT_EQ(8u, c.start());

  queue.ShiftBackward(3);
  EXPECT_EQ(3u, queue.top()->start());
  EXPECT_EQ(3u, a.start());
  EXPECT_EQ(12u, a.end());
  EXPECT_EQ(4u, b.start());
  EXPECT_EQ(5u, c.start());

  std::vector<TextBufferRange*> doomed;
  doomed.push_back(&a);
  doomed.push_back(&c);
  queue.Erase(doomed.begin(), doomed.end());
  EXPECT_EQ(4u, queue.top()->start());
}

TEST(TextBufferRangeQueue, AscendingByEnd) {
  TextBufferRangeQueue<TextBufferRange::AscendingByEnd> queue;
  TextBufferRange a(1, 10);
  TextBufferRange b(2, 9);
  TextBufferRange c(3, 11);

  queue.push(&a);
  queue.push(&b);
  queue.push(&c);
  EXPECT_EQ(11u, queue.top()->end());
}

}  // namespace
}  // namespace zi
