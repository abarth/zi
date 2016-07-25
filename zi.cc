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

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "command_buffer.h"
#include "macros.h"
#include "scoped_fd.h"
#include "term.h"
#include "text_buffer.h"

namespace zi {

std::vector<char> ReadFile(const std::string& path) {
  std::vector<char> result;
  ScopedFD fd(HANDLE_EINTR(open(path.c_str(), O_RDONLY)));
  // TODO(abarth): Add an error reporting mechanism.
  if (!fd.is_valid())
    return result;
  constexpr size_t kBufferSize = 1 << 16;
  char buffer[kBufferSize];
  for (;;) {
    int count = HANDLE_EINTR(read(fd.get(), buffer, kBufferSize));
    if (count == -1)
      return result;
    if (count == 0)
      break;
    result.insert(result.end(), buffer, buffer + count);
  }
  return result;
}

enum class Mode {
  Vi,
  Command,
  Input,
};

class Shell {
 public:
  Shell();
  ~Shell();

  void OpenFile(const std::string& path);

  int Run();

  void mark_needs_display() { needs_display_ = true; }

 private:
  void Display();

  void MoveCursorLeft();
  void MoveCursorDown();
  void MoveCursorUp();
  void MoveCursorRight();

  void HandleCharacterInViMode(char c);
  void HandleCharacterInCommandMode(char c);
  void HandleCharacterInInputMode(char c);

  void ExecuteCommand(const std::string& command);

  Mode mode_ = Mode::Vi;
  std::string status_;
  TextBuffer text_;

  bool should_quit_ = false;
  bool needs_display_ = false;

  DISALLOW_COPY_AND_ASSIGN(Shell);
};

Shell::Shell() {
  term::Put(term::kSaveScreen);
  term::Put(term::kMoveCursorHome);
}

Shell::~Shell() {
  term::Put(term::kRestoreScreen);
}

void Shell::OpenFile(const std::string& path) {
  text_.SetText(ReadFile(path));
}

int Shell::Run() {
  Display();
  while (!should_quit_) {
    char c = '\0';
    int count = read(STDIN_FILENO, &c, 1);
    if (count == -1)
      return 1;
    if (count == 0)
      continue;
    switch (mode_) {
      case Mode::Vi:
        HandleCharacterInViMode(c);
        break;
      case Mode::Command:
        HandleCharacterInCommandMode(c);
        break;
      case Mode::Input:
        HandleCharacterInInputMode(c);
        break;
    }
    if (needs_display_)
      Display();
  }
  return 0;
}

void Shell::MoveCursorLeft() {
  term::Put(term::kMoveCursorLeft);
  // TODO(abarth): Handle RTL.
  text_.MoveCursorBackward();
}

void Shell::MoveCursorDown() {
  // TODO(abarth): Understand line breaks.
  // term::Put(term::kMoveCursorDown);
}

void Shell::MoveCursorUp() {
  // TODO(abarth): Understand line breaks.
  // term::Put(term::kMoveCursorUp);
}

void Shell::MoveCursorRight() {
  term::Put(term::kMoveCursorRight);
  // TODO(abarth): Handle RTL.
  text_.MoveCursorForward();
}

void Shell::HandleCharacterInViMode(char c) {
  switch (c) {
    case 'h':
      term::Put(term::kMoveCursorLeft);
      break;
    case 'j':
      term::Put(term::kMoveCursorDown);
      break;
    case 'k':
      term::Put(term::kMoveCursorUp);
      break;
    case 'l':
      term::Put(term::kMoveCursorRight);
      break;
    case 'i':
      mode_ = Mode::Input;
      break;
    case 'Z':
      should_quit_ = true;
      break;
    case ':':
      mode_ = Mode::Command;
      status_ = ":";
      mark_needs_display();
      break;
    default:
      break;
  }
}

void Shell::HandleCharacterInCommandMode(char c) {
  if (c == '\n' || c == '\r') {
    ExecuteCommand(status_);
    status_.clear();
    mark_needs_display();
  } else if (c == '\x8') {
    status_.pop_back();
    mark_needs_display();
  } else if (c == '\x1b') {
    status_.clear();
    mode_ = Mode::Vi;
    mark_needs_display();
  } else {
    status_.push_back(c);
    mark_needs_display();
  }
}

void Shell::HandleCharacterInInputMode(char c) {
  if (c == '\x1b') {
    mode_ = Mode::Vi;
    mark_needs_display();
  } else {
    text_.InsertCharacter(c);
    mark_needs_display();
  }
}

void Shell::ExecuteCommand(const std::string& command) {
  if (command == ":q")
    should_quit_ = true;
}

void Shell::Display() {
  CommandBuffer commands;
  commands << term::kSaveCursorPosition << term::kEraseScreen
           << term::kMoveCursorHome;
  commands.MoveCursorTo(0, term::rows - 1);
  commands << status_ << term::kEraseToEndOfLine
           << term::kRestoreCursorPosition;
  commands.Execute();
}

}  // namespace zi

int main(int argc, char** argv) {
  if (!term::Init())
    return 1;
  zi::Shell shell;
  if (argc > 1) {
    std::string file_name = argv[1];
    shell.OpenFile(file_name);
  }
  return shell.Run();
}
