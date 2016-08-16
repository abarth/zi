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

#include "text/text_span_queue.h"

#include <string>

#include "gtest/gtest.h"

namespace zi {
namespace {

TEST(TextSpanQueue, Control) {
  TextSpanQueue<TextSpan::DescendingByBegin> queue;
  TextSpan a(1, 10);
  TextSpan b(2, 9);
  TextSpan c(3, 11);

  queue.push(&a);
  queue.push(&b);
  queue.push(&c);
  EXPECT_EQ(1u, queue.top()->begin());

  queue.ShiftForward(5);
  EXPECT_EQ(6u, queue.top()->begin());
  EXPECT_EQ(6u, a.begin());
  EXPECT_EQ(15u, a.end());
  EXPECT_EQ(7u, b.begin());
  EXPECT_EQ(8u, c.begin());

  queue.ShiftBackward(3);
  EXPECT_EQ(3u, queue.top()->begin());
  EXPECT_EQ(3u, a.begin());
  EXPECT_EQ(12u, a.end());
  EXPECT_EQ(4u, b.begin());
  EXPECT_EQ(5u, c.begin());

  std::vector<TextSpan*> doomed;
  doomed.push_back(&a);
  doomed.push_back(&c);
  queue.Erase(doomed.begin(), doomed.end());
  EXPECT_EQ(4u, queue.top()->begin());
}

}  // namespace
}  // namespace zi
