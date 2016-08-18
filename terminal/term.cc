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

#include "terminal/term.h"

#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "zen/macros.h"

namespace term {
namespace {

struct termios g_original_termios;

bool GetSize() {
  struct winsize screen_size;
  int result = ioctl(STDIN_FILENO, TIOCGWINSZ, &screen_size);
  if (result == -1) {
    fprintf(stderr, "error: Unable to get terminal size.\n");
    return false;
  }
  cols = static_cast<size_t>(screen_size.ws_col);
  rows = static_cast<size_t>(screen_size.ws_row);
  return true;
}

void RestoreOriginalTermios() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_original_termios) == -1)
    fprintf(stderr, "error: Failed to restore original terminal attributes.\n");
}

bool EnableRaw() {
  if (!isatty(STDIN_FILENO)) {
    fprintf(stderr, "error: stdin is not a tty.\n");
    return false;
  }
  if (tcgetattr(STDIN_FILENO, &g_original_termios) == -1) {
    fprintf(stderr, "error: Cannot get terminal attributes.\n");
    return false;
  }
  if (atexit(RestoreOriginalTermios) != 0) {
    fprintf(stderr, "error: Cannot register cleaup handler.\n");
    return false;
  }
  struct termios raw = g_original_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    fprintf(stderr, "error: Cannot enable raw terminal input.\n");
    return false;
  }
  return true;
}

}  // namspace

const char* kForegroundColors[] = {
    "30", "31", "32", "33", "34", "35", "36", "37",
};

const char* kBackgroundColors[] = {
    "40", "41", "42", "43", "44", "45", "46", "47",
};

size_t cols = 0;
size_t rows = 0;

void Put(const std::string& message) {
  write(STDOUT_FILENO, message.data(), message.size());
}

bool Init() {
  return term::GetSize() && term::EnableRaw();
}

}  // namespace term
