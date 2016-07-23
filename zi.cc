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
#include <unistd.h>

#define ESC "\x1B"

namespace term {

template <size_t n>
void Put(const char (&message)[n]) {
  write(STDOUT_FILENO, message, n);
}

void SaveScreen() {
  Put(ESC "[?47h");
}

void RestoreScreen() {
  Put(ESC "[?47l");
}

void ClearScreen() {
  Put(ESC "[2J");
}

}  // namespace

void Run() {
  for (;;) {
    int c = getchar();
    if (c == 'q')
      return;
  }
}

int main(int, char**) {
  term::SaveScreen();
  term::ClearScreen();
  term::Put("hello, world\n");
  Run();
  term::RestoreScreen();
  return 0;
}
