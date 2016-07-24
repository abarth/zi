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

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <utility>

#define ESC "\x1B"

namespace term {
namespace {

int cols = 0;
int rows = 0;

}  // namespace

constexpr char kSaveScreen[] = ESC "[?47h";
constexpr char kRestoreScreen[] = ESC "[?47l";
constexpr char kEraseScreen[] = ESC "[2J";
constexpr char kEraseToEndOfLine[] = ESC "[K";
// constexpr char kClearCharacterAttributes[] = ESC "[0m";
// constexpr char kSetBold[] = ESC "[1m";
// constexpr char kSetLowIntensity[] = ESC "[2m";
// constexpr char kSetUnderline[] = ESC "[4m";
// constexpr char kSetReverseVideo[] = ESC "[7m";
// constexpr char kSetInvisibleText[] = ESC "[8m";

template <size_t n>
void Put(const char (&message)[n]) {
  write(STDOUT_FILENO, message, n);
}

void Put(const std::string& message) {
  write(STDOUT_FILENO, message.data(), message.size());
}

bool GetSize() {
  struct winsize screen_size;
  int result = ioctl(STDIN_FILENO, TIOCGWINSZ, &screen_size);
  if (result == -1)
    return false;
  cols = screen_size.ws_col;
  rows = screen_size.ws_row;
  return true;
}

enum class Color { Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

const char* kForegroundColors[] = {
    "30", "31", "32", "33", "34", "35", "36", "37",
};

const char* kBackgroundColors[] = {
    "40", "41", "42", "43", "44", "45", "46", "47",
};

}  // namespace

namespace zi {

class CommandBuffer {
 public:
  template <typename T>
  CommandBuffer& operator<<(T&& value) {
    stream_ << value;
    return *this;
  }

  void MoveCursorTo(int x, int y);
  void Execute();

 private:
  std::ostringstream stream_;
};

void CommandBuffer::MoveCursorTo(int x, int y) {
  stream_ << ESC "[" << y << ";" << x << "H";
}

void CommandBuffer::Execute() {
  term::Put(stream_.str());
}

class Shell {
 public:
  void set_status(std::string status) { status_ = std::move(status); }

  void Display();
  void Run();

 private:
  std::string status_;
};

void Shell::Display() {
  CommandBuffer commands;
  commands << term::kEraseScreen;
  commands.MoveCursorTo(0, term::rows - 1);
  commands << status_ << term::kEraseToEndOfLine;
  commands.Execute();
}

void Shell::Run() {
  for (;;) {
    Display();

    int c = getchar();
    if (c == 'q')
      return;
  }
}

}  // zi

int main(int, char**) {
  if (!term::GetSize()) {
    fprintf(stderr, "error: Unable to get terminal size.\n");
    return 1;
  }
  term::Put(term::kSaveScreen);
  zi::Shell shell;
  shell.set_status("Hello, world.");
  shell.Run();
  term::Put(term::kRestoreScreen);
  return 0;
}
