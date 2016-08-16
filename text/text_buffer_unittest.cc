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

#include "text/text_buffer.h"

#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"

namespace zi {
namespace {

TEST(TextBuffer, Find) {
  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter(0, 'x');
  buffer.InsertCharacter(1, 'y');
  buffer.InsertCharacter(2, 'z');

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
  std::vector<char> data(text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter(3, 'x');
  buffer.InsertCharacter(4, 'y');
  buffer.InsertCharacter(5, 'z');

  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.InsertCharacter(3, 'A');
  buffer.DeleteCharacter(3);
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.InsertCharacter(4, 'A');
  buffer.DeleteCharacter(4);
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.InsertCharacter(5, 'A');
  buffer.DeleteCharacter(5);
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());

  buffer.InsertText(4, "four score and seven years ago");
  EXPECT_EQ("Helxfour score and seven years agoyzlo, world", buffer.ToString());
}

TEST(TextBuffer, Span) {
  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter(3, 'x');
  buffer.InsertCharacter(4, 'y');
  buffer.InsertCharacter(5, 'z');

  auto check = [&buffer](size_t begin, size_t end, std::string left,
                         std::string right) {
    TextSpan span(begin, end);
    auto pair = buffer.GetTextForSpan(&span);
    EXPECT_EQ(left, pair.left().ToString());
    EXPECT_EQ(right, pair.right().ToString());
  };

  check(0, 0, "", "");
  check(0, 1, "H", "");
  check(0, 2, "He", "");
  check(0, 3, "Hel", "");
  check(0, 4, "Helx", "");
  check(0, 5, "Helxy", "");
  check(0, 6, "Helxyz", "");
  check(0, 7, "Helxyz", "l");
  check(0, 8, "Helxyz", "lo");
  check(0, 9, "Helxyz", "lo,");
  check(1, 9, "elxyz", "lo,");
  check(2, 9, "lxyz", "lo,");
  check(3, 9, "xyz", "lo,");
  check(4, 9, "yz", "lo,");
  check(5, 9, "z", "lo,");
  check(6, 9, "lo,", "");
  check(7, 9, "o,", "");
  check(8, 9, ",", "");
  check(9, 9, "", "");
}

TEST(TextBuffer, AddSpan) {
  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());
  TextBuffer buffer(std::move(data));

  TextSpan hello(0, 5);
  TextSpan world(7, 12);

  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForSpan(&world).ToString());

  buffer.AddSpan(&hello);
  buffer.AddSpan(&world);
  buffer.InsertCharacter(3, 'x');
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(3);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForSpan(&world).ToString());

  buffer.InsertCharacter(3, 'x');
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForSpan(&world).ToString());

  buffer.InsertCharacter(6, ' ');
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForSpan(&world).ToString());

  buffer.InsertCharacter(6, ',');
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(3);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForSpan(&world).ToString());

  buffer.InsertCharacter(7, 'w');
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForSpan(&world).ToString());

  EXPECT_EQ(text, buffer.ToString());

  buffer.DeleteCharacter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("rld", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("ld", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("d", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ(0u, world.begin());
  EXPECT_EQ(0u, world.end());
  EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(6);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());

  buffer.DeleteCharacter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForSpan(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());

  // TODO(abarth): We go off the rails at this point.

  // buffer.DeleteCharacter(4);
  // EXPECT_TRUE(hello.is_dirty());
  // hello.MarkClean();
  // EXPECT_FALSE(world.is_dirty());
  // EXPECT_EQ("Hell", buffer.GetTextForSpan(&hello).ToString());
  // EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());

  // buffer.DeleteCharacter(0);
  // EXPECT_TRUE(hello.is_dirty());
  // hello.MarkClean();
  // EXPECT_FALSE(world.is_dirty());
  // EXPECT_EQ("ell", buffer.GetTextForSpan(&hello).ToString());
  // EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());
  //
  // buffer.DeleteCharacter(0);
  // EXPECT_TRUE(hello.is_dirty());
  // hello.MarkClean();
  // EXPECT_FALSE(world.is_dirty());
  // EXPECT_EQ("ll", buffer.GetTextForSpan(&hello).ToString());
  // EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());
  //
  // buffer.DeleteCharacter(1);
  // EXPECT_TRUE(hello.is_dirty());
  // hello.MarkClean();
  // EXPECT_FALSE(world.is_dirty());
  // EXPECT_EQ("l", buffer.GetTextForSpan(&hello).ToString());
  // EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());
  //
  // buffer.DeleteCharacter(0);
  // EXPECT_TRUE(hello.is_dirty());
  // hello.MarkClean();
  // EXPECT_FALSE(world.is_dirty());
  // EXPECT_EQ(0u, hello.begin());
  // EXPECT_EQ(0u, hello.end());
  // EXPECT_EQ("", buffer.GetTextForSpan(&hello).ToString());
  // EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());
  //
  // buffer.DeleteCharacter(0);
  // EXPECT_FALSE(hello.is_dirty());
  // EXPECT_FALSE(world.is_dirty());
  // EXPECT_EQ(0u, hello.begin());
  // EXPECT_EQ(0u, hello.end());
  // EXPECT_EQ("", buffer.GetTextForSpan(&hello).ToString());
  // EXPECT_EQ("", buffer.GetTextForSpan(&world).ToString());

  // EXPECT_EQ("", buffer.ToString());
}

}  // namespace
}  // namespace zi
