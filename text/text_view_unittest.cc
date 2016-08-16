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

#include <string>

#include "gtest/gtest.h"

namespace zi {
namespace {

TEST(TextView, Default) {
  TextView view;
  EXPECT_TRUE(view.is_empty());
  EXPECT_EQ(0u, view.length());
  EXPECT_TRUE(view.left().is_empty());
  EXPECT_TRUE(view.right().is_empty());
  EXPECT_EQ("", view.ToString());
}

TEST(TextView, Control) {
  std::string text1 = "Hello, ";
  std::string text2 = "world";
  StringView string_view1(text1);
  StringView string_view2(text2);
  TextView view(string_view1);
  EXPECT_FALSE(view.is_empty());
  EXPECT_EQ(7u, view.length());
  EXPECT_FALSE(view.left().is_empty());
  EXPECT_TRUE(view.right().is_empty());
  EXPECT_EQ(text1, view.ToString());

  view = TextView(StringView(), string_view1);
  EXPECT_FALSE(view.is_empty());
  EXPECT_EQ(7u, view.length());
  EXPECT_TRUE(view.left().is_empty());
  EXPECT_FALSE(view.right().is_empty());
  EXPECT_EQ(text1, view.ToString());

  view = TextView(string_view1, string_view2);
  EXPECT_FALSE(view.is_empty());
  EXPECT_EQ(12u, view.length());
  EXPECT_FALSE(view.left().is_empty());
  EXPECT_FALSE(view.right().is_empty());
  EXPECT_EQ(text1 + text2, view.ToString());
}

TEST(TextView, StringConstructor) {
  std::string text = "Hello, world";
  TextView view(text);
  EXPECT_FALSE(view.is_empty());
  EXPECT_EQ(12u, view.length());
  EXPECT_FALSE(view.left().is_empty());
  EXPECT_TRUE(view.right().is_empty());
  EXPECT_EQ(text, view.ToString());
}

}  // namespace
}  // namespace zi
