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

#include "text_buffer.h"

#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace zi {
namespace {

TEST(TextBuffer, Find) {
  std::string text = "Hello, world";
  std::vector<char> data;
  data.insert(data.end(), text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter('x');
  buffer.InsertCharacter('y');
  buffer.InsertCharacter('z');

  EXPECT_EQ(0u, buffer.Find('x'));
  EXPECT_EQ(1u, buffer.Find('y'));
  EXPECT_EQ(2u, buffer.Find('z'));
  EXPECT_EQ(3u, buffer.Find('H'));
  EXPECT_EQ(5u, buffer.Find('l', 0));
  EXPECT_EQ(5u, buffer.Find('l', 1));
  EXPECT_EQ(5u, buffer.Find('l', 2));
  EXPECT_EQ(5u, buffer.Find('l', 3));
  EXPECT_EQ(5u, buffer.Find('l', 4));
  EXPECT_EQ(5u, buffer.Find('l', 5));
  EXPECT_EQ(6u, buffer.Find('l', 6));
  EXPECT_EQ(13u, buffer.Find('l', 7));
  EXPECT_EQ(14u, buffer.Find('d'));
}

TEST(TextBuffer, Insert) {
  std::string text = "Hello, world";
  std::vector<char> data;
  data.insert(data.end(), text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.MoveCursorBy(3);
  buffer.InsertCharacter('x');
  buffer.InsertCharacter('y');
  buffer.InsertCharacter('z');

  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.MoveCursorBy(-2);
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.MoveCursorForward();
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.MoveCursorBackward();
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());

  buffer.InsertText("four score and seven years ago");
  EXPECT_EQ("Helxfour score and seven years agoyzlo, world", buffer.ToString());
}

}  // namespace
}  // namespace zi
