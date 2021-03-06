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
  TextBuffer empty_buffer;
  EXPECT_EQ(std::string::npos, empty_buffer.RFind('x', 4u));

  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter(TextPosition(0), 'x');
  buffer.InsertCharacter(TextPosition(1), 'y');
  buffer.InsertCharacter(TextPosition(2), 'z');

  EXPECT_EQ(std::string::npos, buffer.Find('q'));
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
  EXPECT_EQ(13u, buffer.Find('l', 13));
  EXPECT_EQ(std::string::npos, buffer.Find('l', 14));
  EXPECT_EQ(std::string::npos, buffer.Find('l', 15));
  EXPECT_EQ(std::string::npos, buffer.Find('l', 16));
  EXPECT_EQ(std::string::npos, buffer.Find('l', 17));
  EXPECT_EQ(14u, buffer.Find('d'));
}

TEST(TextBuffer, RFind) {
  TextBuffer empty_buffer;
  EXPECT_EQ(std::string::npos, empty_buffer.RFind('x'));

  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter(TextPosition(0), 'x');
  buffer.InsertCharacter(TextPosition(1), 'y');
  buffer.InsertCharacter(TextPosition(2), 'z');

  EXPECT_EQ(0u, buffer.RFind('x'));
  EXPECT_EQ(1u, buffer.RFind('y'));
  EXPECT_EQ(2u, buffer.RFind('z'));
  EXPECT_EQ(3u, buffer.RFind('H'));
  EXPECT_EQ(std::string::npos, buffer.RFind('l', 0));
  EXPECT_EQ(std::string::npos, buffer.RFind('l', 1));
  EXPECT_EQ(std::string::npos, buffer.RFind('l', 2));
  EXPECT_EQ(std::string::npos, buffer.RFind('l', 3));
  EXPECT_EQ(std::string::npos, buffer.RFind('l', 4));
  EXPECT_EQ(5u, buffer.RFind('l', 5));
  EXPECT_EQ(6u, buffer.RFind('l', 6));
  EXPECT_EQ(6u, buffer.RFind('l', 7));
  EXPECT_EQ(6u, buffer.RFind('l', 8));
  EXPECT_EQ(6u, buffer.RFind('l', 9));
  EXPECT_EQ(6u, buffer.RFind('l', 10));
  EXPECT_EQ(6u, buffer.RFind('l', 11));
  EXPECT_EQ(6u, buffer.RFind('l', 12));
  EXPECT_EQ(13u, buffer.RFind('l', 13));
  EXPECT_EQ(13u, buffer.RFind('l', 14));
  EXPECT_EQ(13u, buffer.RFind('l', 15));
  EXPECT_EQ(13u, buffer.RFind('l', 16));
  EXPECT_EQ(13u, buffer.RFind('l', 17));
  EXPECT_EQ(14u, buffer.RFind('d'));
}

TEST(TextBuffer, Insert) {
  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter(TextPosition(3), 'x');
  buffer.InsertCharacter(TextPosition(4), 'y');
  buffer.InsertCharacter(TextPosition(5), 'z');

  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.InsertCharacter(TextPosition(3), 'A');
  buffer.DeleteCharacterAfter(3);
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.InsertCharacter(TextPosition(4), 'A');
  buffer.DeleteCharacterAfter(4);
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());
  buffer.InsertCharacter(TextPosition(5), 'A');
  buffer.DeleteCharacterAfter(5);
  EXPECT_EQ("Helxyzlo, world", buffer.ToString());

  buffer.InsertText(TextPosition(4), "four score and seven years ago");
  EXPECT_EQ("Helxfour score and seven years agoyzlo, world", buffer.ToString());
}

TEST(TextBuffer, Range) {
  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());

  TextBuffer buffer(std::move(data));
  buffer.InsertCharacter(TextPosition(3), 'x');
  buffer.InsertCharacter(TextPosition(4), 'y');
  buffer.InsertCharacter(TextPosition(5), 'z');

  auto check = [&buffer](size_t begin, size_t end, std::string left,
                         std::string right) {
    TextBufferRange range(begin, end);
    auto pair = buffer.GetTextForRange(&range);
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

TEST(TextBuffer, DeleteRange) {
  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());
  TextBuffer buffer(std::move(data));
  buffer.DeleteRange(TextBufferRange(4, 4));
  EXPECT_EQ(text, buffer.ToString());
  buffer.DeleteRange(TextBufferRange(2, 4));
  EXPECT_EQ("Heo, world", buffer.ToString());
  buffer.DeleteRange(TextBufferRange(8, 12));
  EXPECT_EQ("Heo, wor", buffer.ToString());
  buffer.DeleteRange(TextBufferRange(9, 12));
  EXPECT_EQ("Heo, wor", buffer.ToString());
  buffer.DeleteRange(TextBufferRange(0, 7));
  EXPECT_EQ("r", buffer.ToString());
  buffer.DeleteRange(TextBufferRange(0, 2));
  EXPECT_EQ("", buffer.ToString());
}

TEST(TextBuffer, AddRange) {
  std::string text = "Hello, world";
  std::vector<char> data(text.begin(), text.end());
  TextBuffer buffer(std::move(data));

  TextBufferRange hello(0, 5);
  TextBufferRange world(7, 12);

  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForRange(&world).ToString());

  buffer.AddRange(&hello);
  buffer.AddRange(&world);
  buffer.InsertCharacter(TextPosition(3), 'x');
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(3);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("world", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForRange(&world).ToString());

  buffer.InsertCharacter(TextPosition(3), 'x');
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForRange(&world).ToString());

  buffer.InsertCharacter(TextPosition(6), ' ');
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForRange(&world).ToString());

  buffer.InsertCharacter(TextPosition(6), ',');
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Helxlo", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(3);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForRange(&world).ToString());

  buffer.InsertCharacter(TextPosition(7), 'w');
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForRange(&world).ToString());

  EXPECT_EQ(text, buffer.ToString());

  buffer.DeleteCharacterAfter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("orld", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("rld", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("ld", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("d", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_TRUE(world.is_dirty());
  world.MarkClean();
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ(7u, world.start());
  EXPECT_EQ(7u, world.end());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(7);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(6);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(5);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hello", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(4);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("Hell", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(0);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("ell", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(0);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("ll", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(1);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ("l", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(0);
  EXPECT_TRUE(hello.is_dirty());
  hello.MarkClean();
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ(0u, hello.start());
  EXPECT_EQ(0u, hello.end());
  EXPECT_EQ("", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  buffer.DeleteCharacterAfter(0);
  EXPECT_FALSE(hello.is_dirty());
  EXPECT_FALSE(world.is_dirty());
  EXPECT_EQ(0u, hello.start());
  EXPECT_EQ(0u, hello.end());
  EXPECT_EQ("", buffer.GetTextForRange(&hello).ToString());
  EXPECT_EQ("", buffer.GetTextForRange(&world).ToString());

  EXPECT_EQ("", buffer.ToString());
}

}  // namespace
}  // namespace zi
