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

#include <unistd.h>

#include <string>

namespace term {

constexpr char kBell[] = "\x07";
constexpr char kBackspace[] = "\x08";

constexpr char kReset[] = ESC "c";
constexpr char kEnableLineWrap[] = ESC "[7h";
constexpr char kDisableLineWrap[] = ESC "[7l";

constexpr char kSaveScreen[] = ESC "[?47h";
constexpr char kRestoreScreen[] = ESC "[?47l";

constexpr char kEraseToEndOfLine[] = ESC "[K";
constexpr char kEraseToStartOfLine[] = ESC "[1K";
constexpr char kEraseLine[] = ESC "[2K";

constexpr char kEraseToEndOfColumn[] = ESC "[J";
constexpr char kEraseToStartOfColumn[] = ESC "[1J";
constexpr char kEraseScreen[] = ESC "[2J";

constexpr char kMoveCursorHome[] = ESC "[H";
constexpr char kSaveCursorPosition[] = ESC "7";
constexpr char kRestoreCursorPosition[] = ESC "8";
constexpr char kHideCursor[] = ESC "[?25l";
constexpr char kShowCursor[] = ESC "[?25h";
constexpr char kMoveCursorUp[] = ESC "[A";
constexpr char kMoveCursorDown[] = ESC "[B";
constexpr char kMoveCursorRight[] = ESC "[C";
constexpr char kMoveCursorLeft[] = ESC "[D";

constexpr char kEnableScrolling[] = ESC "[r";
constexpr char kScrollUp[] = ESC "D";
constexpr char kScrollDown[] = ESC "M";

constexpr char kClearCharacterAttributes[] = ESC "[0m";
constexpr char kSetBold[] = ESC "[1m";
constexpr char kSetLowIntensity[] = ESC "[2m";
constexpr char kSetStandout[] = ESC "[3m";
constexpr char kSetUnderline[] = ESC "[4m";
constexpr char kSetBlink[] = ESC "[5m";
constexpr char kSetReverseVideo[] = ESC "[7m";
constexpr char kSetInvisibleText[] = ESC "[8m";

enum class Color { Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

extern const char* kForegroundColors[];
extern const char* kBackgroundColors[];

extern size_t cols;
extern size_t rows;

template <size_t n>
void Put(const char (&message)[n]) {
  write(STDOUT_FILENO, message, n);
}

void Put(const std::string& message);

bool Init();

}  // namespace term
